#!/usr/bin/env node

import { existsSync, readFileSync } from "node:fs";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { createInterface } from "node:readline/promises";
import {
  crc32,
  digest,
  fetchWithTimeout,
  multipartFileBody,
} from "./lib/flexfox-http.mjs";
import {
  normalizeMacDerivedDeviceSsid,
  unattendedUpdateNeedsExpectedSsid,
} from "./lib/flexfox-avr-update-identity.mjs";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const host = process.env.FLEXFOX_HOST || "73.73.73.73";
const baseUrl = `http://${host}`;
const manifestPath = resolve(process.env.FLEXFOX_AVR_MANIFEST || join(
  repoRoot,
  "Software", "AVR128DA48", "tmp", "avr-boot-chain",
  "FlexFox80-AVR-Release-Info.json",
));
const dryRun = process.env.FLEXFOX_AVR_UPDATE_DRY_RUN === "1";
const suppliedExpectedDeviceSsid = (process.env.FLEXFOX_EXPECTED_DEVICE_SSID || "").trim();
const expectedDeviceSsid = normalizeMacDerivedDeviceSsid(suppliedExpectedDeviceSsid);

function fail(message) {
  process.stderr.write(`FlexFox AVR WiFi update: ${message}\n`);
  process.exit(2);
}

function versionAtLeast(actual, required) {
  const a = actual.split(".").map(Number);
  const b = required.split(".").map(Number);
  for(let i = 0; i < Math.max(a.length, b.length); i++) {
    if((a[i] || 0) !== (b[i] || 0)) return (a[i] || 0) > (b[i] || 0);
  }
  return true;
}

if(!existsSync(manifestPath)) fail(`release manifest is missing: ${manifestPath}; run just avr-boot-chain-build`);
const manifest = JSON.parse(readFileSync(manifestPath, "utf8"));
if(manifest.format !== "flexfox-avr-update-v1" || manifest.applicationStart !== 0x4000 || manifest.pageSize !== 512) {
  fail("release manifest does not describe the permanent FlexFox boot layout");
}
const imagePath = resolve(dirname(manifestPath), manifest.updateFile);
if(!existsSync(imagePath)) fail(`update image is missing: ${imagePath}`);
const image = readFileSync(imagePath);
const actualSha256 = digest("sha256", image);
const actualCrc32 = `0x${crc32(image).toString(16).padStart(8, "0")}`;
if(image.length !== manifest.imageBytes || actualSha256 !== manifest.imageSha256 || actualCrc32 !== manifest.imageCrc32) {
  fail("update image does not match its release manifest");
}
if(!dryRun && process.env.FLEXFOX_AVR_UPDATE_CONFIRM !== `UPDATE-AVR-${manifest.applicationVersion}`) {
  fail(`set FLEXFOX_AVR_UPDATE_CONFIRM=UPDATE-AVR-${manifest.applicationVersion} to authorize staging this exact image`);
}
if(suppliedExpectedDeviceSsid && !expectedDeviceSsid) {
  fail("FLEXFOX_EXPECTED_DEVICE_SSID must be the complete MAC-derived Tx_XXXXXXXX device SSID");
}
if(unattendedUpdateNeedsExpectedSsid(dryRun, process.stdin.isTTY, expectedDeviceSsid)) {
  fail("an unattended update must set FLEXFOX_EXPECTED_DEVICE_SSID to prevent staging on the wrong unit");
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
const preflightResponse = await fetchWithTimeout(`${baseUrl}/avr-update/status`, { cache: "no-store" });
if(!preflightResponse.ok) fail(`AVR update status returned HTTP ${preflightResponse.status}`);
const preflight = await preflightResponse.json();
const preflightDeviceSsid = normalizeMacDerivedDeviceSsid(preflight.deviceSsid);
if(!preflightDeviceSsid) {
  fail("device did not report its complete MAC-derived Tx_XXXXXXXX SSID before staging");
}
if(expectedDeviceSsid && preflightDeviceSsid !== expectedDeviceSsid) {
  fail(`connected unit is ${preflight.deviceSsid}, not expected ${suppliedExpectedDeviceSsid}`);
}
process.stdout.write(`Device: ${preflight.deviceSsid}, ESP ${device.version}, AVR update state ${preflight.phase}\n`);
if(dryRun) {
  process.stdout.write("PASS: read-only AVR update preflight completed; no image was staged.\n");
  process.exit(0);
}

const multipart = multipartFileBody("firmware", basename(imagePath), image);
const stageUrl = `${baseUrl}/avr-update?confirm=STAGE&size=${image.length}&crc32=${actualCrc32.slice(2)}&version=${encodeURIComponent(manifest.applicationVersion)}`;
process.stdout.write("Staging and validating the complete image in ESP flash...\n");
const stageResponse = await fetchWithTimeout(stageUrl, {
  method: "POST",
  headers: multipart.headers,
  body: multipart.body,
}, 120000);
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

const stagedDeviceSsid = normalizeMacDerivedDeviceSsid(staged.deviceSsid);
if(!stagedDeviceSsid) {
  fail("device did not report its unique MAC-derived Tx_ SSID");
}
if(stagedDeviceSsid !== preflightDeviceSsid) {
  fail(`device identity changed from ${preflight.deviceSsid} to ${staged.deviceSsid} while staging`);
}
const expectedSsidSuffix = preflightDeviceSsid.slice(-4);
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
