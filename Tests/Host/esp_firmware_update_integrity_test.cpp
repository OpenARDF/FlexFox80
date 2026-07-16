#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>

#include "firmware_update_integrity.h"

int main()
{
  size_t size = 0;
  assert(firmwareUpdateParseSize("502948", 0x200000, &size));
  assert(size == 502948);
  assert(!firmwareUpdateParseSize("", 0x200000, &size));
  assert(!firmwareUpdateParseSize("-1", 0x200000, &size));
  assert(!firmwareUpdateParseSize("502948x", 0x200000, &size));
  assert(!firmwareUpdateParseSize("9", 8, &size));
  assert(!firmwareUpdateParseSize("999999999999999999999", 0x200000, &size));

  uint32_t crc = 0;
  assert(firmwareUpdateParseCrc32("cbf43926", &crc));
  assert(crc == 0xcbf43926UL);
  assert(firmwareUpdateParseCrc32("CBF43926", &crc));
  assert(!firmwareUpdateParseCrc32("cbf4392", &crc));
  assert(!firmwareUpdateParseCrc32("cbf439260", &crc));
  assert(!firmwareUpdateParseCrc32("cbf43z26", &crc));

  const uint8_t check[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
  const uint32_t computed =
    firmwareUpdateCrc32(0xffffffffUL, check, sizeof(check)) ^ 0xffffffffUL;
  assert(computed == 0xcbf43926UL);

  assert(firmwareUpdateHasBinExtension("ARDF_Transmitter.ino.bin"));
  assert(firmwareUpdateHasBinExtension("FIRMWARE.BIN"));
  assert(!firmwareUpdateHasBinExtension("filesystem.bin.gz"));
  assert(!firmwareUpdateHasBinExtension("events.html"));

  std::cout << "PASS ESP sketch-update size, CRC32, and filename guards\n";
  return 0;
}
