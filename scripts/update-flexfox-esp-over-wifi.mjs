#!/usr/bin/env node

import { existsSync, readFileSync, statSync } from "node:fs";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { spawnSync } from "node:child_process";
import {
  crc32,
  digest,
  esp8266SketchMd5Candidates,
  fetchWithTimeout,
  multipartFileBody,
  normalizeFlexFoxUrl,
  sleep,
} from "./lib/flexfox-http.mjs";
import { createBoundedFlexFoxHeartbeat } from "./lib/flexfox-heartbeat.mjs";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const espVersionHeader = readFileSync(
  join(repoRoot, "Software", "Huzzah", "ARDF_Transmitter", "esp8266.h"),
  "utf8",
);
const sourceVersion = espVersionHeader.match(/#define\s+WIFI_SW_VERSION\s+\("([^"]+)"\)/)?.[1];
if (!sourceVersion) {
  throw new Error("could not read WIFI_SW_VERSION from the ESP firmware source");
}
const firmwarePath = resolve(
  process.env.FLEXFOX_FIRMWARE_BIN ??
    join(repoRoot, "Software", "Huzzah", "tmp", "esp-build", "ARDF_Transmitter.ino.bin"),
);
const baseUrl = normalizeFlexFoxUrl(process.env.FLEXFOX_URL);
const expectedVersion = process.env.FLEXFOX_EXPECTED_ESP_VERSION ?? sourceVersion;
const expectedDeviceSsid = String(process.env.FLEXFOX_EXPECTED_DEVICE_SSID ?? "").trim();
const expectedPreUpdateDeviceSsid = String(
  process.env.FLEXFOX_EXPECTED_PREUPDATE_DEVICE_SSID ?? expectedDeviceSsid,
).trim();
const reconnectSsid = String(
  process.env.FLEXFOX_RECONNECT_SSID ?? expectedDeviceSsid,
).trim();
const expectFilesystemRecovery = process.env.FLEXFOX_EXPECTED_FILESYSTEM_RECOVERY === "1";
const adbSerial = String(process.env.FLEXFOX_ADB_SERIAL ?? "").trim();
const adbPath = String(process.env.FLEXFOX_ADB ?? "adb").trim();
const confirmation = process.env.FLEXFOX_UPDATE_CONFIRM;
const uploadTimeoutMs = Number.parseInt(
  process.env.FLEXFOX_UPDATE_UPLOAD_TIMEOUT_MS ?? "120000",
  10,
);
const verificationTimeoutMs = Number.parseInt(
  process.env.FLEXFOX_UPDATE_VERIFY_TIMEOUT_MS ?? "240000",
  10,
);

if (confirmation !== "UPDATE FLEXFOX ESP") {
  throw new Error("Set FLEXFOX_UPDATE_CONFIRM='UPDATE FLEXFOX ESP' to authorize the sketch update");
}
if (!Number.isInteger(uploadTimeoutMs) || uploadTimeoutMs < 30000 || uploadTimeoutMs > 300000) {
  throw new Error("FLEXFOX_UPDATE_UPLOAD_TIMEOUT_MS must be 30000 through 300000");
}
if (
  !Number.isInteger(verificationTimeoutMs) ||
  verificationTimeoutMs < 60000 ||
  verificationTimeoutMs > 300000
) {
  throw new Error("FLEXFOX_UPDATE_VERIFY_TIMEOUT_MS must be 60000 through 300000");
}
if (!statSync(firmwarePath).isFile()) {
  throw new Error(`firmware image is not a file: ${firmwarePath}`);
}
if (expectedDeviceSsid && !/^Tx_[0-9A-F]{8}$/.test(expectedDeviceSsid)) {
  throw new Error("FLEXFOX_EXPECTED_DEVICE_SSID must be the exact MAC-derived Tx_ plus eight uppercase hex digits");
}
if (expectedPreUpdateDeviceSsid && !/^Tx_[0-9A-F]{1,8}$/.test(expectedPreUpdateDeviceSsid)) {
  throw new Error("FLEXFOX_EXPECTED_PREUPDATE_DEVICE_SSID must be an exact legacy or canonical MAC-derived Tx_ identity");
}
if (reconnectSsid && reconnectSsid !== "Tx_Master" && !/^Tx_[0-9A-F]{8}$/.test(reconnectSsid)) {
  throw new Error("FLEXFOX_RECONNECT_SSID must be Tx_Master or an exact canonical device SSID");
}
if (reconnectSsid && !adbSerial) {
  throw new Error("FLEXFOX_RECONNECT_SSID requires FLEXFOX_ADB_SERIAL");
}
if (expectFilesystemRecovery) {
  const evidencePath = join(dirname(firmwarePath), "build-evidence.json");
  if (!existsSync(evidencePath) ||
      JSON.parse(readFileSync(evidencePath, "utf8")).recoveryQualification !== true) {
    throw new Error("filesystem recovery may be expected only for the dedicated qualification build artifact");
  }
}

const firmware = readFileSync(firmwarePath);
if (firmware.length < 4096 || firmware[0] !== 0xe9) {
  throw new Error("firmware image is not an uncompressed ESP8266 sketch .bin");
}
const firmwareCrc32 = crc32(firmware).toString(16).padStart(8, "0");
const firmwareMd5 = digest("md5", firmware);
const installedMd5Candidates = esp8266SketchMd5Candidates(firmware);
const installedMd5Values = new Set(installedMd5Candidates.map(({ md5 }) => md5));
const firmwareSha256 = digest("sha256", firmware);

async function readStatus(timeoutMs = 5000) {
  const statusUrl = new URL("firmware/status", baseUrl);
  statusUrl.searchParams.set("cache", String(Date.now()));
  const response = await fetchWithTimeout(statusUrl, { cache: "no-store" }, timeoutMs);
  if (!response.ok) throw new Error(`firmware status returned HTTP ${response.status}`);
  return response.json();
}

async function readLegacyDeviceSsid(timeoutMs = 5000) {
  const websocketUrl = new URL(baseUrl);
  websocketUrl.protocol = "ws:";
  websocketUrl.port = "81";
  return new Promise((resolvePromise, rejectPromise) => {
    const socket = new WebSocket(websocketUrl);
    const timer = setTimeout(() => {
      socket.close();
      rejectPromise(new Error("legacy device SSID query timed out"));
    }, timeoutMs);
    const finish = (error, ssid) => {
      clearTimeout(timer);
      if (socket.readyState < WebSocket.CLOSING) socket.close(1000, "identity verified");
      if (error) rejectPromise(error);
      else resolvePromise(ssid);
    };
    socket.addEventListener("open", () => {
      socket.send("!&");
      socket.send("SSID");
    });
    socket.addEventListener("message", (event) => {
      const message = String(event.data);
      if (message.startsWith("SSID,")) finish(undefined, message.slice(5));
    });
    socket.addEventListener("error", () => finish(new Error("legacy device SSID query failed")));
  });
}

function requestMotoReassociation() {
  if (!adbSerial || !reconnectSsid) return;
  spawnSync(
    adbPath,
    [
      "-s", adbSerial,
      "shell", "cmd", "wifi", "connect-network",
      reconnectSsid, "open", "-r", "none",
    ],
    { encoding: "utf8", timeout: 15000 },
  );
}

let before = await readStatus();
if (before.uptimeMillis < 10000) {
  await sleep(10000 - before.uptimeMillis);
  before = await readStatus();
}
if (before.filesystemProtected !== true) {
  throw new Error("device did not confirm that its firmware endpoint protects LittleFS");
}
const beforeDeviceSsid = before.deviceSsid ||
  (expectedPreUpdateDeviceSsid ? await readLegacyDeviceSsid() : undefined);
if (expectedPreUpdateDeviceSsid && beforeDeviceSsid !== expectedPreUpdateDeviceSsid) {
  throw new Error(`connected device is ${beforeDeviceSsid}; expected ${expectedPreUpdateDeviceSsid}`);
}
if (before.cloneActive || before.updateActive || before.restartPending) {
  throw new Error(`device is busy: ${JSON.stringify(before)}`);
}
if (!Number.isInteger(before.maximumUpdateBytes) || firmware.length > before.maximumUpdateBytes) {
  throw new Error(`firmware is ${firmware.length} bytes; device limit is ${before.maximumUpdateBytes}`);
}
const imageWasAlreadyInstalled = installedMd5Values.has(
  String(before.currentSketchMd5 ?? "").toLowerCase(),
);

console.log(`Target: ${baseUrl.href}`);
console.log(`Before: ESP ${before.version}, sketch ${before.currentSketchBytes} bytes, MD5 ${before.currentSketchMd5}`);
console.log(`Upload: ${basename(firmwarePath)}, ${firmware.length} bytes, CRC32 ${firmwareCrc32}`);
console.log(`SHA-256: ${firmwareSha256}`);

let heartbeat;
if (before.recoveryMode === true) {
  console.log("PASS recovery-mode updater uses the ESP-internal bounded AVR maintenance lease");
} else {
  heartbeat = createBoundedFlexFoxHeartbeat(baseUrl, "firmware update verification");
  process.once("exit", () => heartbeat?.stop());
  await heartbeat.start();
  console.log("PASS bounded AVR update heartbeat established");
}

const updateUrl = new URL("firmware", baseUrl);
updateUrl.searchParams.set("confirm", "UPDATE");
updateUrl.searchParams.set("size", String(firmware.length));
updateUrl.searchParams.set("crc32", firmwareCrc32);
const multipart = multipartFileBody("firmware", basename(firmwarePath), firmware);

let uploadResponse;
try {
  uploadResponse = await fetchWithTimeout(updateUrl, {
    method: "POST",
    headers: multipart.headers,
    body: multipart.body,
    redirect: "manual",
  }, uploadTimeoutMs);
  const responseText = await uploadResponse.text();
  if (!uploadResponse.ok) {
    throw new Error(`update rejected with HTTP ${uploadResponse.status}: ${responseText.trim()}`);
  }
  heartbeat?.poke();
  console.log(`PASS upload accepted with HTTP ${uploadResponse.status}`);
} catch (error) {
  console.warn(`Upload response was interrupted or ambiguous: ${error.message}`);
  console.warn("Polling the device; success will be reported only after installed-image verification.");
}

const deadline = Date.now() + verificationTimeoutMs;
let after;
let wrongImageAfterReboot;
let lastReassociationMillis = 0;
while (Date.now() < deadline) {
  if (adbSerial && Date.now() - lastReassociationMillis >= 3000) {
    requestMotoReassociation();
    lastReassociationMillis = Date.now();
  }
  await sleep(1500);
  try {
    const candidate = await readStatus(3000);
    if (candidate.restartPending || candidate.updateActive) continue;
    if (!Number.isInteger(candidate.uptimeMillis)) continue;
    /* A changed exact MD5 proves the new boot even if slow AP/HTTP startup makes
     * its observed uptime exceed the old image's short pre-update uptime. When
     * reinstalling the same bytes, retain the stricter uptime-reset proof. */
    if (imageWasAlreadyInstalled && candidate.uptimeMillis >= before.uptimeMillis) continue;
    if (!installedMd5Values.has(candidate.currentSketchMd5?.toLowerCase())) {
      wrongImageAfterReboot = candidate;
      break;
    }
    after = candidate;
    break;
  } catch {
    // A short disconnect is expected while the ESP reboots and recreates its AP services.
  }
}

if (wrongImageAfterReboot) {
  heartbeat?.stop();
  throw new Error(
    `device rebooted but reports ESP ${wrongImageAfterReboot.version}, sketch MD5 ` +
      `${wrongImageAfterReboot.currentSketchMd5}; the staged update was not installed`,
  );
}
if (!after) {
  heartbeat?.stop();
  throw new Error(
    `device did not return within ${verificationTimeoutMs / 1000} seconds with an installed ` +
      `sketch MD5 derived from ${firmwareMd5}`,
  );
}
if (after.version !== expectedVersion) {
  throw new Error(`installed ESP version is ${after.version}; expected ${expectedVersion}`);
}
if (after.filesystemProtected !== true) {
  throw new Error("rebooted device no longer reports sketch-only update protection");
}
if (expectedDeviceSsid && after.deviceSsid !== expectedDeviceSsid) {
  throw new Error(`rebooted device is ${after.deviceSsid}; expected ${expectedDeviceSsid}`);
}
if (expectFilesystemRecovery) {
  if (after.filesystemMounted !== false || after.recoveryMode !== true ||
      after.filesystemRecoveryReason !== "qualification") {
    throw new Error("qualification image did not enter the expected filesystem recovery mode");
  }
} else if (after.filesystemMounted !== true || after.recoveryMode !== false) {
  throw new Error("ESP sketch installed, but LittleFS did not mount; the device remains in filesystem recovery mode");
}
heartbeat?.stop();

const installedCandidate = installedMd5Candidates.find(
  ({ md5 }) => md5 === after.currentSketchMd5.toLowerCase(),
);
console.log(
  `PASS rebooted into ESP ${after.version}, sketch MD5 ${after.currentSketchMd5} ` +
  `(flash-mode header ${installedCandidate.flashMode})`,
);
console.log(`PASS uptime reset from ${before.uptimeMillis} ms to ${after.uptimeMillis} ms`);
console.log("PASS update endpoint still reports LittleFS protection");
console.log(expectFilesystemRecovery ?
  "PASS dedicated image entered filesystem recovery without mounting LittleFS" :
  "PASS LittleFS mounted without entering recovery mode");
