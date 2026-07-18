/*
 *  MIT License
 *
 *  Copyright (c) 2026 Digital Confections LLC
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

/*
 * Persistent staging, restart recovery, and raw USART AVR programming.
 */

#include "AvrFirmwareUpdate.h"
#include "Blinkies.h"

#include <LittleFS.h>

extern Blinkies *lights;

static const uint32_t AVR_UPDATE_STATE_MAGIC = 0x31565541UL; /* AUV1 */
static const uint16_t AVR_UPDATE_STATE_SCHEMA = 1U;
static const uint8_t AVR_UPDATE_RECOVERY_ATTEMPTS = 3U;
static const uint16_t AVR_UPDATE_PROBE_CAPTURE_BYTES = 96U;
static String g_avrUpdateProbeHex;
static uint32_t g_avrUpdateProbeBytes;

static char hexDigit(uint8_t value)
{
  value &= 0x0fU;
  return value < 10U ? (char)('0' + value) : (char)('a' + value - 10U);
}

static void captureProbeByte(uint8_t value)
{
  if(g_avrUpdateProbeBytes < AVR_UPDATE_PROBE_CAPTURE_BYTES)
  {
    g_avrUpdateProbeHex += hexDigit(value >> 4);
    g_avrUpdateProbeHex += hexDigit(value);
  }
  g_avrUpdateProbeBytes++;
}

static void resetProbeCapture(void)
{
  g_avrUpdateProbeHex = "";
  g_avrUpdateProbeHex.reserve(AVR_UPDATE_PROBE_CAPTURE_BYTES * 2U);
  g_avrUpdateProbeBytes = 0;
}

uint32_t avrUpdateCrc32(uint32_t crc, const uint8_t *data, size_t length)
{
  while(length--)
  {
    crc ^= *data++;
    for(uint8_t bit = 0; bit < 8; bit++) crc = (crc >> 1) ^ ((crc & 1U) ? 0xedb88320UL : 0U);
  }
  return crc;
}

static uint32_t stateCrc(const AvrUpdateState& state)
{
  return avrUpdateCrc32(0xffffffffUL, reinterpret_cast<const uint8_t *>(&state),
                        sizeof(state) - sizeof(state.stateCrc32)) ^ 0xffffffffUL;
}

static bool stateIsValid(const AvrUpdateState& state)
{
  return state.magic == AVR_UPDATE_STATE_MAGIC &&
         state.schema == AVR_UPDATE_STATE_SCHEMA &&
         state.stateCrc32 == stateCrc(state) &&
         state.imageBytes > 0 && state.imageBytes <= AVR_UPDATE_MAX_IMAGE_BYTES &&
         (state.imageBytes % AVR_UPDATE_PAGE_SIZE) == 0 &&
         state.pageCount == state.imageBytes / AVR_UPDATE_PAGE_SIZE;
}

static bool readStateFile(const char *path, AvrUpdateState *state)
{
  File file = LittleFS.open(path, "r");
  if(!file || file.size() != sizeof(*state) || file.read(reinterpret_cast<uint8_t *>(state), sizeof(*state)) != sizeof(*state))
  {
    if(file) file.close();
    return false;
  }
  file.close();
  return stateIsValid(*state);
}

static bool saveState(AvrUpdateState *state)
{
  AvrUpdateState candidate = *state;
  candidate.generation = state->generation + 1U;
  candidate.stateCrc32 = stateCrc(candidate);
  const char *target = (candidate.generation & 1U) ? AVR_UPDATE_STATE_A_PATH : AVR_UPDATE_STATE_B_PATH;
  LittleFS.remove(AVR_UPDATE_STATE_TEMP_PATH);
  File file = LittleFS.open(AVR_UPDATE_STATE_TEMP_PATH, "w");
  if(!file || file.write(reinterpret_cast<const uint8_t *>(&candidate), sizeof(candidate)) != sizeof(candidate))
  {
    if(file) file.close();
    LittleFS.remove(AVR_UPDATE_STATE_TEMP_PATH);
    return false;
  }
  file.flush();
  file.close();
  LittleFS.remove(target);
  if(!LittleFS.rename(AVR_UPDATE_STATE_TEMP_PATH, target)) return false;
  *state = candidate;
  return true;
}

bool avrUpdateLoadState(AvrUpdateState *state)
{
  if(!state) return false;
  AvrUpdateState a, b;
  const bool haveA = readStateFile(AVR_UPDATE_STATE_A_PATH, &a);
  const bool haveB = readStateFile(AVR_UPDATE_STATE_B_PATH, &b);
  if(!haveA && !haveB) return false;
  *state = !haveB || (haveA && (int32_t)(a.generation - b.generation) > 0) ? a : b;
  return true;
}

bool avrUpdateCommitStagedImage(uint32_t imageBytes, uint32_t imageCrc32, const String& targetVersion, String *error)
{
  if(!imageBytes || imageBytes > AVR_UPDATE_MAX_IMAGE_BYTES || (imageBytes % AVR_UPDATE_PAGE_SIZE) != 0)
  {
    if(error) *error = "AVR image must contain complete 512-byte pages";
    return false;
  }
  File file = LittleFS.open(AVR_UPDATE_STAGING_PATH, "r");
  if(!file || file.size() != imageBytes)
  {
    if(file) file.close();
    if(error) *error = "Staged AVR image size changed";
    return false;
  }
  uint8_t resetVector[2] = {0xff, 0xff};
  if(file.read(resetVector, sizeof(resetVector)) != sizeof(resetVector) ||
     (resetVector[0] == 0xff && resetVector[1] == 0xff))
  {
    file.close();
    if(error) *error = "AVR image has no application reset vector";
    return false;
  }
  file.close();

  LittleFS.remove(AVR_UPDATE_IMAGE_PATH);
  if(!LittleFS.rename(AVR_UPDATE_STAGING_PATH, AVR_UPDATE_IMAGE_PATH))
  {
    if(error) *error = "Unable to commit the staged AVR image";
    return false;
  }

  AvrUpdateState state = {};
  AvrUpdateState previousState;
  if(avrUpdateLoadState(&previousState)) state.generation = previousState.generation;
  state.magic = AVR_UPDATE_STATE_MAGIC;
  state.schema = AVR_UPDATE_STATE_SCHEMA;
  state.phase = AVR_UPDATE_STAGED;
  state.imageBytes = imageBytes;
  state.imageCrc32 = imageCrc32;
  state.nextPage = 1;
  state.pageCount = imageBytes / AVR_UPDATE_PAGE_SIZE;
  targetVersion.substring(0, sizeof(state.targetVersion) - 1).toCharArray(state.targetVersion, sizeof(state.targetVersion));
  if(!saveState(&state))
  {
    if(error) *error = "Unable to persist AVR update recovery state";
    return false;
  }
  LittleFS.remove(AVR_UPDATE_DIAGNOSTIC_PATH);
  return true;
}

bool avrUpdateMarkEnteringBootloader(String *error)
{
  AvrUpdateState state;
  if(!avrUpdateLoadState(&state) ||
     (state.phase != AVR_UPDATE_STAGED && state.phase != AVR_UPDATE_PROGRAMMING))
  {
    if(error) *error = "No validated AVR image is available for bootloader entry";
    return false;
  }
  state.phase = AVR_UPDATE_ENTERING_BOOTLOADER;
  state.nextPage = 1;
  LittleFS.remove(AVR_UPDATE_DIAGNOSTIC_PATH);
  if(!saveState(&state))
  {
    if(error) *error = "Unable to persist bootloader-entry state";
    return false;
  }
  return true;
}

bool avrUpdateRestoreStaged(String *error)
{
  AvrUpdateState state;
  if(!avrUpdateLoadState(&state))
  {
    if(error) *error = "AVR recovery state is unavailable";
    return false;
  }
  state.phase = AVR_UPDATE_STAGED;
  state.nextPage = 1;
  if(!saveState(&state))
  {
    if(error) *error = "Unable to restore staged state";
    return false;
  }
  return true;
}

bool avrUpdateHasStagedImage(void)
{
  AvrUpdateState state;
  return avrUpdateLoadState(&state) &&
         (state.phase == AVR_UPDATE_STAGED || state.phase == AVR_UPDATE_PROGRAMMING) &&
         LittleFS.exists(AVR_UPDATE_IMAGE_PATH);
}

static bool filesystemInfo(FSInfo *info)
{
  return info && LittleFS.info(*info) && info->blockSize > 0 && info->usedBytes <= info->totalBytes;
}

bool avrUpdateCanStageImage(uint32_t imageBytes, String *error)
{
  AvrUpdateState existing;
  if(avrUpdateLoadState(&existing) &&
     (existing.phase == AVR_UPDATE_ENTERING_BOOTLOADER ||
      existing.phase == AVR_UPDATE_PROGRAMMING ||
      existing.phase == AVR_UPDATE_VERIFYING_APPLICATION))
  {
    if(error) *error = "An AVR update is already in its bootloader recovery phase";
    return false;
  }
  FSInfo info;
  if(!filesystemInfo(&info))
  {
    if(error) *error = "Unable to measure LittleFS capacity before AVR staging";
    return false;
  }
  const uint32_t roundedImageBytes =
    ((imageBytes + (uint32_t)info.blockSize - 1U) / (uint32_t)info.blockSize) * (uint32_t)info.blockSize;
  const uint32_t requiredFreeBytes = roundedImageBytes +
    (uint32_t)info.blockSize * AVR_UPDATE_FS_RESERVE_BLOCKS;
  const uint32_t freeBytes = (uint32_t)(info.totalBytes - info.usedBytes);
  if(freeBytes < requiredFreeBytes)
  {
    if(error)
    {
      *error = String("LittleFS has ") + freeBytes + " bytes free; AVR staging requires " +
               requiredFreeBytes + " bytes including the recovery reserve";
    }
    return false;
  }
  return true;
}

static const char *phaseName(uint8_t phase)
{
  switch(phase)
  {
    case AVR_UPDATE_STAGED: return "staged";
    case AVR_UPDATE_ENTERING_BOOTLOADER: return "entering-bootloader";
    case AVR_UPDATE_PROGRAMMING: return "programming";
    case AVR_UPDATE_VERIFYING_APPLICATION: return "verifying-application";
    case AVR_UPDATE_COMPLETE: return "complete";
    default: return "none";
  }
}

String avrUpdateStatusJson(const String& deviceSsid)
{
  String identity = String(",\"deviceSsid\":\"") + deviceSsid + "\"" +
                    ",\"probeBytes\":" + String(g_avrUpdateProbeBytes) +
                    ",\"probeHex\":\"" + g_avrUpdateProbeHex + "\"";
  FSInfo info;
  if(filesystemInfo(&info))
  {
    identity += String(",\"filesystemTotalBytes\":") + info.totalBytes +
                ",\"filesystemUsedBytes\":" + info.usedBytes +
                ",\"filesystemFreeBytes\":" + (info.totalBytes - info.usedBytes) +
                ",\"filesystemBlockSize\":" + info.blockSize +
                ",\"filesystemStagingReserveBytes\":" +
                (info.blockSize * AVR_UPDATE_FS_RESERVE_BLOCKS);
  }
  File diagnostic = LittleFS.open(AVR_UPDATE_DIAGNOSTIC_PATH, "r");
  if(diagnostic)
  {
    String detail = diagnostic.readString();
    diagnostic.close();
    detail.replace("\\", "_");
    detail.replace("\"", "'");
    detail.replace("\r", " ");
    detail.replace("\n", " ");
    identity += String(",\"diagnostic\":\"") + detail + "\"";
  }
  identity += "}";
  AvrUpdateState state;
  if(!avrUpdateLoadState(&state)) return String("{\"phase\":\"none\",\"staged\":false") + identity;
  String response = String("{\"phase\":\"") + phaseName(state.phase) + "\",\"staged\":" +
                    (LittleFS.exists(AVR_UPDATE_IMAGE_PATH) ? "true" : "false") +
                    ",\"targetVersion\":\"" + state.targetVersion + "\",\"imageBytes\":" +
                    String(state.imageBytes) + ",\"nextPage\":" + String(state.nextPage) +
                    ",\"pageCount\":" + String(state.pageCount) + identity;
  return response;
}

static uint16_t crc16Update(uint16_t crc, uint8_t value)
{
  crc ^= (uint16_t)value << 8;
  for(uint8_t bit = 0; bit < 8; bit++) crc = (crc & 0x8000U) ? (uint16_t)((crc << 1) ^ 0x1021U) : (uint16_t)(crc << 1);
  return crc;
}

static bool readLine(String *line, uint32_t timeoutMs)
{
  line->remove(0);
  uint32_t started = millis();
  while((uint32_t)(millis() - started) < timeoutMs)
  {
    while(Serial.available())
    {
      char value = (char)Serial.read();
      captureProbeByte((uint8_t)value);
      if(value == '\n') { line->trim(); return line->length() > 0; }
      if(value != '\r' && line->length() < 180) *line += value;
    }
    if(lights) lights->blinkLEDs(100, RED_BLUE_ALTERNATING, true);
    yield();
  }
  return false;
}

static bool waitForResponse(const char *prefix, String *response, uint32_t timeoutMs = 2500)
{
  uint32_t started = millis();
  String line;
  while((uint32_t)(millis() - started) < timeoutMs)
  {
    if(readLine(&line, 250) && line.startsWith(prefix))
    {
      if(response) *response = line;
      return true;
    }
  }
  return false;
}

static void writeAddressFrame(char command, uint32_t address)
{
  uint8_t frame[7];
  frame[0] = (uint8_t)command;
  frame[1] = (uint8_t)address;
  frame[2] = (uint8_t)(address >> 8);
  frame[3] = (uint8_t)(address >> 16);
  frame[4] = (uint8_t)(address >> 24);
  uint16_t crc = 0xffffU;
  for(uint8_t i = 0; i < 5; i++) crc = crc16Update(crc, frame[i]);
  frame[5] = (uint8_t)crc;
  frame[6] = (uint8_t)(crc >> 8);
  for(uint8_t i = 0; i < sizeof(frame); i++)
  {
    Serial.write(frame[i]);
    Serial.flush();
    delayMicroseconds(250);
  }
}

static void writePaced(const uint8_t *data, size_t length)
{
  while(length--)
  {
    Serial.write(*data++);
    Serial.flush();
    delayMicroseconds(250);
  }
}

static bool erasePage(uint32_t address)
{
  for(uint8_t attempt = 0; attempt < 5; attempt++)
  {
    writeAddressFrame('E', address);
    if(waitForResponse("OK erase", NULL)) return true;
    delay(50);
  }
  return false;
}

static bool writePage(uint32_t address, const uint8_t *page)
{
  uint8_t header[5] = {'W', (uint8_t)address, (uint8_t)(address >> 8), (uint8_t)(address >> 16), (uint8_t)(address >> 24)};
  uint16_t crc = 0xffffU;
  for(uint8_t i = 0; i < sizeof(header); i++) crc = crc16Update(crc, header[i]);
  for(uint16_t i = 0; i < AVR_UPDATE_PAGE_SIZE; i++) crc = crc16Update(crc, page[i]);
  for(uint8_t attempt = 0; attempt < 5; attempt++)
  {
    writePaced(header, sizeof(header));
    writePaced(page, AVR_UPDATE_PAGE_SIZE);
    uint8_t expectedCrc[2] = {(uint8_t)crc, (uint8_t)(crc >> 8)};
    writePaced(expectedCrc, sizeof(expectedCrc));
    if(waitForResponse("OK write", NULL)) return true;
    delay(50);
  }
  return false;
}

static uint16_t pageCrc(const uint8_t *page)
{
  uint16_t crc = 0xffffU;
  for(uint16_t i = 0; i < AVR_UPDATE_PAGE_SIZE; i++) crc = crc16Update(crc, page[i]);
  return crc;
}

static bool verifyPage(uint32_t address, const uint8_t *page)
{
  String response;
  writeAddressFrame('C', address);
  if(!waitForResponse("OK crc ", &response)) return false;
  int separator = response.lastIndexOf(' ');
  if(separator < 0) return false;
  return (uint16_t)strtoul(response.substring(separator + 1).c_str(), NULL, 16) == pageCrc(page);
}

static bool bootloaderPresent(void)
{
  while(Serial.available()) (void)Serial.read();
  for(uint8_t attempt = 0; attempt < 20; attempt++)
  {
    Serial.write('?'); Serial.flush();
    String response;
    if(waitForResponse("FlexFox80 BL", &response, 500) &&
       response.indexOf("app=0x4000") >= 0 && response.indexOf("page=512") >= 0) return true;
    delay(100);
  }
  return false;
}

bool avrUpdateResidentBootloaderPresent(void)
{
  resetProbeCapture();
  Serial.flush();
  Serial.end();
  delay(20);
  Serial.begin(AVR_UPDATE_BAUD);
  delay(100);
  return bootloaderPresent();
}

static bool applicationPresent(void)
{
  while(Serial.available()) (void)Serial.read();
  /* Refresh ESP power even if the application's reply cannot be decoded yet. */
  Serial.print("$ESP,Z;\n");
  Serial.print("$UPD?\n");
  Serial.flush();
  uint32_t started = millis();
  String response;
  while((uint32_t)(millis() - started) < 3000U)
  {
    if(readLine(&response, 250) &&
       (response.startsWith("!UPD,") || response.startsWith("!NAK"))) return true;
  }
  return false;
}

static bool validateImage(File *image, const AvrUpdateState& state, String *failure)
{
  if(!*image)
  {
    if(failure) *failure = "open";
    return false;
  }
  if(image->size() != state.imageBytes)
  {
    if(failure) *failure = String("size expected=") + String(state.imageBytes) +
                           String(" actual=") + String(image->size());
    return false;
  }
  uint8_t buffer[128];
  uint32_t crc = 0xffffffffUL;
  if(!image->seek(0, SeekSet))
  {
    if(failure) *failure = "initial-seek";
    return false;
  }
  uint32_t remaining = state.imageBytes;
  while(remaining)
  {
    const size_t requested = remaining < sizeof(buffer) ? remaining : sizeof(buffer);
    const size_t count = image->read(buffer, requested);
    if(count != requested)
    {
      if(failure) *failure = String("read remaining=") + String(remaining) +
                             String(" requested=") + String(requested) +
                             String(" actual=") + String(count);
      return false;
    }
    crc = avrUpdateCrc32(crc, buffer, requested);
    remaining -= requested;
    yield();
  }
  const uint32_t actualCrc32 = crc ^ 0xffffffffUL;
  if(actualCrc32 != state.imageCrc32)
  {
    if(failure) *failure = String("crc expected=0x") + String(state.imageCrc32, HEX) +
                           String(" actual=0x") + String(actualCrc32, HEX);
    return false;
  }
  if(!image->seek(0, SeekSet))
  {
    if(failure) *failure = "rewind";
    return false;
  }
  return true;
}

static void recordOperationFailure(const char *operation, uint32_t address)
{
  LittleFS.remove(AVR_UPDATE_DIAGNOSTIC_PATH);
  File diagnostic = LittleFS.open(AVR_UPDATE_DIAGNOSTIC_PATH, "w");
  if(!diagnostic) return;
  diagnostic.print(operation);
  diagnostic.print(" address=0x");
  diagnostic.print(address, HEX);
  diagnostic.print(" probeBytes=");
  diagnostic.print(g_avrUpdateProbeBytes);
  diagnostic.print(" probeHex=");
  diagnostic.print(g_avrUpdateProbeHex);
  diagnostic.flush();
  diagnostic.close();
}

static void restartAfterOperationFailure(File *image, const char *operation, uint32_t address)
{
  recordOperationFailure(operation, address);
  if(*image) image->close();
  delay(250);
  ESP.restart();
  for(;;) { delay(1000); yield(); }
}

void avrUpdateResumeIfRequired(bool bootloaderAlreadyReady)
{
  AvrUpdateState state;
  if(!avrUpdateLoadState(&state) ||
     (state.phase != AVR_UPDATE_ENTERING_BOOTLOADER && state.phase != AVR_UPDATE_PROGRAMMING)) return;
  if(LittleFS.exists(AVR_UPDATE_DIAGNOSTIC_PATH)) return;

  File image = LittleFS.open(AVR_UPDATE_IMAGE_PATH, "r");
  String validationFailure;
  if(!validateImage(&image, state, &validationFailure))
  {
    if(state.phase == AVR_UPDATE_ENTERING_BOOTLOADER)
    {
      if(image) image.close();
      LittleFS.remove(AVR_UPDATE_DIAGNOSTIC_PATH);
      File diagnostic = LittleFS.open(AVR_UPDATE_DIAGNOSTIC_PATH, "w");
      if(diagnostic)
      {
        diagnostic.print("validate-image ");
        diagnostic.print(validationFailure);
        diagnostic.flush();
        diagnostic.close();
      }
      state.phase = AVR_UPDATE_STAGED;
      state.nextPage = 1;
      while(!saveState(&state)) { delay(250); yield(); }
      return;
    }
    /* Programming has begun and the reset vector may be invalid. Stay powered for recovery. */
    for(;;) { delay(1000); yield(); }
  }
  resetProbeCapture();
  /* A direct resident-bootloader probe leaves a proven UART session ready for
   * the first flash command.  Do not close and reopen that session merely to
   * repeat the same probe; installed hardware has shown that the second probe
   * can miss an otherwise healthy resident bootloader. */
  bool bootloaderReady = bootloaderAlreadyReady;
  uint8_t recoveryAttempts = 0;
  while(!bootloaderReady)
  {
    Serial.flush(); Serial.end(); delay(20); Serial.begin(AVR_UPDATE_BAUD); delay(100);
    bootloaderReady = bootloaderPresent();
    if(!bootloaderReady)
    {
      Serial.flush(); Serial.end(); delay(20); Serial.begin(9600); delay(100);
      if(applicationPresent())
      {
        if(state.phase == AVR_UPDATE_ENTERING_BOOTLOADER)
        {
          state.phase = AVR_UPDATE_STAGED;
          state.nextPage = 1;
          while(!saveState(&state)) { delay(250); yield(); }
          return;
        }
      }
      recoveryAttempts++;
      if(recoveryAttempts >= AVR_UPDATE_RECOVERY_ATTEMPTS)
      {
        if(state.phase == AVR_UPDATE_ENTERING_BOOTLOADER)
        {
          /*
           * No application page has been erased in this phase.  A lost handoff
           * must not suppress HTTP forever; leave the validated image staged so
           * the operator can retry after inspecting the running AVR.
           */
          state.phase = AVR_UPDATE_STAGED;
          state.nextPage = 1;
          while(!saveState(&state)) { delay(250); yield(); }
        }
        /*
         * Preserve a programming-phase record and image, but do not leave the
         * service interface dark forever when neither endpoint can be decoded.
         * An authorized retry resets nextPage before requesting bootloader entry.
         */
        image.close();
        return;
      }
    }
    if(!bootloaderReady) { delay(500); yield(); }
  }

  state.phase = AVR_UPDATE_PROGRAMMING;
  if(state.nextPage < 1 || state.nextPage > state.pageCount) state.nextPage = 1;
  while(!saveState(&state)) { delay(250); yield(); }

  uint8_t page[AVR_UPDATE_PAGE_SIZE];
  if(!erasePage(AVR_UPDATE_APP_START))
    restartAfterOperationFailure(&image, "erase-reset-vector", AVR_UPDATE_APP_START);
  for(uint16_t pageIndex = state.nextPage; pageIndex < state.pageCount; pageIndex++)
  {
    image.seek((uint32_t)pageIndex * AVR_UPDATE_PAGE_SIZE, SeekSet);
    if(image.read(page, sizeof(page)) != sizeof(page)) for(;;) { delay(1000); yield(); }
    const uint32_t address = AVR_UPDATE_APP_START + (uint32_t)pageIndex * AVR_UPDATE_PAGE_SIZE;
    if(!erasePage(address)) restartAfterOperationFailure(&image, "erase", address);
    if(!writePage(address, page)) restartAfterOperationFailure(&image, "write", address);
    if(!verifyPage(address, page)) restartAfterOperationFailure(&image, "verify", address);
    state.nextPage = pageIndex + 1;
    while(!saveState(&state)) { delay(250); yield(); }
    yield();
  }

  image.seek(0, SeekSet);
  if(image.read(page, sizeof(page)) != sizeof(page)) for(;;) { delay(1000); yield(); }
  if(!writePage(AVR_UPDATE_APP_START, page))
    restartAfterOperationFailure(&image, "write-reset-vector", AVR_UPDATE_APP_START);
  if(!verifyPage(AVR_UPDATE_APP_START, page))
    restartAfterOperationFailure(&image, "verify-reset-vector", AVR_UPDATE_APP_START);
  image.close();

  state.phase = AVR_UPDATE_VERIFYING_APPLICATION;
  state.nextPage = state.pageCount;
  while(!saveState(&state)) { delay(250); yield(); }
  Serial.write('R'); Serial.flush();
  delay(500);
  ESP.restart();
  for(;;) { delay(1000); yield(); }
}

bool avrUpdateObserveApplicationVersion(const String& version)
{
  AvrUpdateState state;
  if(!avrUpdateLoadState(&state) || state.phase != AVR_UPDATE_VERIFYING_APPLICATION) return false;
  if(version != String(state.targetVersion)) return false;
  /* The physical success indication must mean that no live staged image remains,
   * not merely that the application reported the requested version. If state
   * persistence then needs a retry, the verified application can report again. */
  if(LittleFS.exists(AVR_UPDATE_IMAGE_PATH) && !LittleFS.remove(AVR_UPDATE_IMAGE_PATH)) return false;
  state.phase = AVR_UPDATE_COMPLETE;
  return saveState(&state);
}
