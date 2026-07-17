#!/usr/bin/env node

import { readFileSync, statSync } from "node:fs";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
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
const firmwarePath = resolve(
  process.env.FLEXFOX_FIRMWARE_BIN ??
    join(repoRoot, "Software", "Huzzah", "tmp", "esp-build", "ARDF_Transmitter.ino.bin"),
);
const baseUrl = normalizeFlexFoxUrl(process.env.FLEXFOX_URL);
const expectedVersion = process.env.FLEXFOX_EXPECTED_ESP_VERSION ?? "2.6";
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

let before = await readStatus();
if (before.uptimeMillis < 10000) {
  await sleep(10000 - before.uptimeMillis);
  before = await readStatus();
}
if (before.filesystemProtected !== true) {
  throw new Error("device did not confirm that its firmware endpoint protects LittleFS");
}
if (before.cloneActive || before.updateActive || before.restartPending) {
  throw new Error(`device is busy: ${JSON.stringify(before)}`);
}
if (!Number.isInteger(before.maximumUpdateBytes) || firmware.length > before.maximumUpdateBytes) {
  throw new Error(`firmware is ${firmware.length} bytes; device limit is ${before.maximumUpdateBytes}`);
}

console.log(`Target: ${baseUrl.href}`);
console.log(`Before: ESP ${before.version}, sketch ${before.currentSketchBytes} bytes, MD5 ${before.currentSketchMd5}`);
console.log(`Upload: ${basename(firmwarePath)}, ${firmware.length} bytes, CRC32 ${firmwareCrc32}`);
console.log(`SHA-256: ${firmwareSha256}`);

const heartbeat = createBoundedFlexFoxHeartbeat(baseUrl, "firmware update verification");
process.once("exit", () => heartbeat.stop());
await heartbeat.start();
console.log("PASS bounded AVR update heartbeat established");

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
  heartbeat.poke();
  console.log(`PASS upload accepted with HTTP ${uploadResponse.status}`);
} catch (error) {
  console.warn(`Upload response was interrupted or ambiguous: ${error.message}`);
  console.warn("Polling the device; success will be reported only after installed-image verification.");
}

const deadline = Date.now() + verificationTimeoutMs;
let after;
let wrongImageAfterReboot;
while (Date.now() < deadline) {
  await sleep(1500);
  try {
    const candidate = await readStatus(3000);
    if (candidate.restartPending || candidate.updateActive) continue;
    if (!Number.isInteger(candidate.uptimeMillis) || candidate.uptimeMillis >= before.uptimeMillis) continue;
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
  heartbeat.stop();
  throw new Error(
    `device rebooted but reports ESP ${wrongImageAfterReboot.version}, sketch MD5 ` +
      `${wrongImageAfterReboot.currentSketchMd5}; the staged update was not installed`,
  );
}
if (!after) {
  heartbeat.stop();
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
heartbeat.stop();

const installedCandidate = installedMd5Candidates.find(
  ({ md5 }) => md5 === after.currentSketchMd5.toLowerCase(),
);
console.log(
  `PASS rebooted into ESP ${after.version}, sketch MD5 ${after.currentSketchMd5} ` +
  `(flash-mode header ${installedCandidate.flashMode})`,
);
console.log(`PASS uptime reset from ${before.uptimeMillis} ms to ${after.uptimeMillis} ms`);
console.log("PASS update endpoint still reports LittleFS protection");
