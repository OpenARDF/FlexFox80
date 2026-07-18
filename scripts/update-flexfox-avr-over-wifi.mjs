#!/usr/bin/env node

import { createHash } from "node:crypto";
import { existsSync, readFileSync } from "node:fs";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { createInterface } from "node:readline/promises";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const host = process.env.FLEXFOX_HOST || "73.73.73.73";
const baseUrl = `http://${host}`;
const manifestPath = resolve(process.env.FLEXFOX_AVR_MANIFEST || join(
  repoRoot,
  "Software", "AVR128DA48", "tmp", "avr-boot-chain",
  "FlexFox80-AVR-Release-Info.json",
));

function fail(message) {
  process.stderr.write(`FlexFox AVR WiFi update: ${message}\n`);
  process.exit(2);
}

function crc32(buffer) {
  let crc = 0xffffffff;
  for(const byte of buffer) {
    crc ^= byte;
    for(let bit = 0; bit < 8; bit++) crc = (crc >>> 1) ^ ((crc & 1) ? 0xedb88320 : 0);
  }
  return (crc ^ 0xffffffff) >>> 0;
}

function versionAtLeast(actual, required) {
  const a = actual.split(".").map(Number);
  const b = required.split(".").map(Number);
  for(let i = 0; i < Math.max(a.length, b.length); i++) {
    if((a[i] || 0) !== (b[i] || 0)) return (a[i] || 0) > (b[i] || 0);
  }
  return true;
}

async function fetchWithTimeout(url, options = {}, timeoutMs = 15000) {
  const controller = new AbortController();
  const timer = setTimeout(() => controller.abort(), timeoutMs);
  try {
    return await fetch(url, { ...options, signal: controller.signal });
  } finally {
    clearTimeout(timer);
  }
}

if(!existsSync(manifestPath)) fail(`release manifest is missing: ${manifestPath}; run just avr-boot-chain-build`);
const manifest = JSON.parse(readFileSync(manifestPath, "utf8"));
if(manifest.format !== "flexfox-avr-update-v1" || manifest.applicationStart !== 0x4000 || manifest.pageSize !== 512) {
  fail("release manifest does not describe the permanent FlexFox boot layout");
}
const imagePath = resolve(dirname(manifestPath), manifest.updateFile);
if(!existsSync(imagePath)) fail(`update image is missing: ${imagePath}`);
const image = readFileSync(imagePath);
const actualSha256 = createHash("sha256").update(image).digest("hex");
const actualCrc32 = `0x${crc32(image).toString(16).padStart(8, "0")}`;
if(image.length !== manifest.imageBytes || actualSha256 !== manifest.imageSha256 || actualCrc32 !== manifest.imageCrc32) {
  fail("update image does not match its release manifest");
}
if(process.env.FLEXFOX_AVR_UPDATE_CONFIRM !== `UPDATE-AVR-${manifest.applicationVersion}`) {
  fail(`set FLEXFOX_AVR_UPDATE_CONFIRM=UPDATE-AVR-${manifest.applicationVersion} to authorize staging this exact image`);
}

process.stdout.write(`Target: ${baseUrl}\n`);
process.stdout.write(`Image: ${basename(imagePath)} (${image.length} bytes, ${actualCrc32})\n`);
const deviceResponse = await fetchWithTimeout(`${baseUrl}/firmware/status`, { cache: "no-store" });
if(!deviceResponse.ok) fail(`device status returned HTTP ${deviceResponse.status}`);
const device = await deviceResponse.json();
if(!versionAtLeast(device.version, manifest.minimumEspVersion)) {
  fail(`ESP ${device.version} cannot perform AVR updates; install ESP ${manifest.minimumEspVersion} or later first`);
}
if(device.cloneActive || device.updateActive || device.restartPending || device.linkbusEventTransactionActive) {
  fail("device is busy with another firmware, clone, or Linkbus transaction");
}

const form = new FormData();
form.append("firmware", new Blob([image], { type: "application/octet-stream" }), basename(imagePath));
const stageUrl = `${baseUrl}/avr-update?confirm=STAGE&size=${image.length}&crc32=${actualCrc32.slice(2)}&version=${encodeURIComponent(manifest.applicationVersion)}`;
process.stdout.write("Staging and validating the complete image in ESP flash...\n");
const stageResponse = await fetchWithTimeout(stageUrl, { method: "POST", body: form }, 120000);
const stageText = await stageResponse.text();
if(!stageResponse.ok) fail(`staging failed with HTTP ${stageResponse.status}: ${stageText}`);
const stagedResponse = await fetchWithTimeout(`${baseUrl}/avr-update/status`, { cache: "no-store" });
const staged = await stagedResponse.json();
if(staged.phase !== "staged" || staged.targetVersion !== manifest.applicationVersion || staged.imageBytes !== image.length) {
  fail(`device did not retain the expected staged state: ${JSON.stringify(staged)}`);
}
if(Number.isFinite(staged.filesystemFreeBytes)) {
  process.stdout.write(`LittleFS free after staging: ${Math.floor(staged.filesystemFreeBytes / 1024)} KiB\n`);
}

if(typeof staged.deviceSsid !== "string" || !staged.deviceSsid.startsWith("Tx_") || staged.deviceSsid.length < 7) {
  fail("device did not report its unique MAC-derived Tx_ SSID");
}
const expectedSsidSuffix = staged.deviceSsid.slice(-4).toUpperCase();
let suppliedSsidSuffix;
if(process.stdin.isTTY) {
  const prompt = createInterface({ input: process.stdin, output: process.stdout });
  suppliedSsidSuffix = await prompt.question(
    `Make sure no event is active. Enter the final four characters of ${staged.deviceSsid} to start: `,
  );
  prompt.close();
} else {
  suppliedSsidSuffix = process.env.FLEXFOX_AVR_SSID_SUFFIX;
}
suppliedSsidSuffix = (suppliedSsidSuffix || "").trim().toUpperCase();
if(suppliedSsidSuffix !== expectedSsidSuffix) {
  fail(`authorization must match the final four characters of ${staged.deviceSsid}`);
}

process.stdout.write("Requesting AVR bootloader entry. A temporary WiFi disconnect is expected...\n");
try {
  const startBody = new URLSearchParams({ confirm: "START", unlock: suppliedSsidSuffix });
  const startResponse = await fetchWithTimeout(`${baseUrl}/avr-update/start`, {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: startBody,
  }, 15000);
  if(startResponse.status !== 202) fail(`bootloader entry returned HTTP ${startResponse.status}: ${await startResponse.text()}`);
} catch(error) {
  process.stdout.write("The ESP powered down before the HTTP response completed; continuing recovery polling.\n");
}

const deadline = Date.now() + Number(process.env.FLEXFOX_AVR_VERIFY_TIMEOUT_MS || 600000);
while(Date.now() < deadline) {
  await new Promise((resolvePromise) => setTimeout(resolvePromise, 5000));
  try {
    const response = await fetchWithTimeout(`${baseUrl}/avr-update/status`, { cache: "no-store" }, 5000);
    if(!response.ok) continue;
    const status = await response.json();
    process.stdout.write(`Recovery state: ${status.phase}${status.pageCount ? ` (${status.nextPage}/${status.pageCount})` : ""}\n`);
    if(status.phase === "complete" && status.targetVersion === manifest.applicationVersion) {
      process.stdout.write(`PASS: AVR ${manifest.applicationVersion} programmed, booted, and reported its installed version.\n`);
      process.exit(0);
    }
    if(status.phase === "staged") fail("AVR rejected bootloader entry; verify that the event and manual transmitter are idle");
  } catch(error) {
    /* Power cycling and AP reassociation are expected during the transaction. */
  }
}

fail("verification timed out; keep power connected and reconnect to the FlexFox AP to inspect /avr-update/status");
