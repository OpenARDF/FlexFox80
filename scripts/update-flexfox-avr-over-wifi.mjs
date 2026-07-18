#!/usr/bin/env node

import { existsSync, mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { spawnSync } from "node:child_process";
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
import {
  FLEXFOX_AVR_IMAGE_FORMAT,
  inspectFlexFoxAvrUpdateImage,
} from "./lib/flexfox-avr-update-image.mjs";

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
const qualificationEspRestartPage = Number(process.env.FLEXFOX_AVR_QUALIFICATION_ESP_RESTART_PAGE || 0);
const qualificationAvrResetPage = Number(process.env.FLEXFOX_AVR_QUALIFICATION_AVR_RESET_PAGE || 0);
const qualificationFinalReadback = process.env.FLEXFOX_AVR_QUALIFICATION_FINAL_READBACK === "1";
const defaultVerifyTimeoutMs = 30 * 60 * 1000;
const supportedBootloaderBauds = new Set([9600, 19200, 38400, 57600, 115200]);

function fail(message) {
  process.stderr.write(`FlexFox AVR WiFi update: ${message}\n`);
  process.exit(2);
}

const verifyTimeoutMs = Number(
  process.env.FLEXFOX_AVR_VERIFY_TIMEOUT_MS || defaultVerifyTimeoutMs,
);
if(!Number.isInteger(verifyTimeoutMs) || verifyTimeoutMs < 60000 || verifyTimeoutMs > 3600000) {
  fail("FLEXFOX_AVR_VERIFY_TIMEOUT_MS must be an integer from 60000 through 3600000");
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
if(manifest.format !== FLEXFOX_AVR_IMAGE_FORMAT || manifest.protocolVersion !== 2 ||
   !supportedBootloaderBauds.has(manifest.bootloaderBaud) ||
   manifest.applicationStart !== 0x4000 || manifest.pageSize !== 512) {
  fail("release manifest does not describe the permanent FlexFox boot layout");
}
const imagePath = resolve(dirname(manifestPath), manifest.updateFile);
if(!existsSync(imagePath)) fail(`update image is missing: ${imagePath}`);
const image = readFileSync(imagePath);
let imageInfo;
try {
  imageInfo = inspectFlexFoxAvrUpdateImage(image);
} catch(error) {
  fail(error.message);
}
const actualSha256 = digest("sha256", image);
const actualCrc32 = `0x${crc32(image).toString(16).padStart(8, "0")}`;
if(image.length !== manifest.imageBytes || actualSha256 !== manifest.imageSha256 || actualCrc32 !== manifest.imageCrc32 ||
   imageInfo.applicationVersion !== manifest.applicationVersion ||
   imageInfo.applicationPayloadBytes !== manifest.applicationPayloadBytes ||
   `0x${imageInfo.applicationPayloadCrc32.toString(16).padStart(8, "0")}` !== manifest.applicationPayloadCrc32) {
  fail("update image does not match its release manifest");
}
if(qualificationEspRestartPage &&
   (!Number.isInteger(qualificationEspRestartPage) || qualificationEspRestartPage < 1 ||
    process.env.FLEXFOX_AVR_QUALIFICATION_CONFIRM !== "ARM-ESP-RESTART")) {
  fail("qualification ESP restart requires an integer page and FLEXFOX_AVR_QUALIFICATION_CONFIRM=ARM-ESP-RESTART");
}
if(qualificationAvrResetPage &&
   (!Number.isInteger(qualificationAvrResetPage) || qualificationAvrResetPage < 1 ||
    process.env.FLEXFOX_AVR_QUALIFICATION_CONFIRM !== "ARM-AVR-RESET")) {
  fail("qualification AVR reset requires an integer page and FLEXFOX_AVR_QUALIFICATION_CONFIRM=ARM-AVR-RESET");
}
if(qualificationEspRestartPage && qualificationAvrResetPage) {
  fail("select only one qualification interruption per update run");
}
if(qualificationAvrResetPage || qualificationFinalReadback) {
  const avrdudeProbe = spawnSync("avrdude", ["-?"], { encoding: "utf8" });
  if(avrdudeProbe.error?.code === "ENOENT") {
    fail("qualification AVR reset requires avrdude on PATH");
  }
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
process.stdout.write(`Image: ${basename(imagePath)} (${image.length} bytes, ${actualCrc32}), bootloader ${manifest.bootloaderBaud} baud\n`);
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
const evidenceRoot = resolve(process.env.FLEXFOX_AVR_EVIDENCE_DIR || join(
  repoRoot,
  "Software", "AVR128DA48", "tmp", "avr-update-evidence",
  `${new Date().toISOString().replaceAll(":", "-")}-${preflightDeviceSsid}`,
));
mkdirSync(evidenceRoot, { recursive: true });
writeFileSync(join(evidenceRoot, "release-manifest.json"), `${JSON.stringify(manifest, null, 2)}\n`);

function runFinalAtmelIceReadback() {
  const flashPath = join(evidenceRoot, "flash-after.bin");
  const eepromPath = join(evidenceRoot, "eeprom-after.bin");
  const fusesPath = join(evidenceRoot, "fuses-after.bin");
  process.stdout.write("Reading back flash, EEPROM, and fuses through Atmel-ICE...\n");
  const readback = spawnSync(
    "avrdude",
    [
      "-c", "atmelice_updi", "-p", "128da48", "-P", "usb", "-B", "10",
      "-U", `flash:r:${flashPath}:r`,
      "-U", `eeprom:r:${eepromPath}:r`,
      "-U", `fuses:r:${fusesPath}:r`,
    ],
    { encoding: "utf8", timeout: 120000, maxBuffer: 8 * 1024 * 1024 },
  );
  writeFileSync(join(evidenceRoot, "atmel-ice-readback.stdout.txt"), readback.stdout || "");
  writeFileSync(join(evidenceRoot, "atmel-ice-readback.stderr.txt"), readback.stderr || "");
  if(readback.error || readback.status !== 0) {
    fail(`final Atmel-ICE readback failed; evidence retained at ${evidenceRoot}`);
  }
  const flash = readFileSync(flashPath);
  const eeprom = readFileSync(eepromPath);
  const fuses = readFileSync(fusesPath);
  const bootloaderPath = resolve(dirname(manifestPath), "..", "bootloader-release", "FlexFox80Bootloader.bin");
  if(!existsSync(bootloaderPath)) fail(`expected bootloader binary is missing for readback comparison: ${bootloaderPath}`);
  const bootloader = readFileSync(bootloaderPath);
  const expectedBootSection = Buffer.alloc(0x4000, 0xff);
  bootloader.copy(expectedBootSection);
  if(flash.length !== 128 * 1024 ||
     !flash.subarray(0, expectedBootSection.length).equals(expectedBootSection)) {
    fail(`final readback did not match the exact expected resident bootloader; evidence retained at ${evidenceRoot}`);
  }
  if(!flash.subarray(manifest.applicationStart, manifest.applicationStart + image.length).equals(image)) {
    fail(`final readback did not match the exact wireless AVR image; evidence retained at ${evidenceRoot}`);
  }
  if(eeprom.length !== 512 || eeprom[511] !== 0xff) {
    fail(`final readback found an invalid EEPROM recovery marker; evidence retained at ${evidenceRoot}`);
  }
  if(fuses.length !== 16 || fuses[7] !== 0x00 || fuses[8] !== 0x20) {
    fail(`final readback found incorrect CODESIZE/BOOTSIZE fuses; evidence retained at ${evidenceRoot}`);
  }
  writeFileSync(join(evidenceRoot, "readback-verification.json"), `${JSON.stringify({
    flashSha256: digest("sha256", flash),
    eepromSha256: digest("sha256", eeprom),
    fusesSha256: digest("sha256", fuses),
    bootloaderBytesCompared: expectedBootSection.length,
    applicationImageBytesCompared: image.length,
    eepromRecoveryMarker: "0xff",
    codeSizeFuse: "0x00",
    bootSizeFuse: "0x20",
  }, null, 2)}\n`);
  process.stdout.write("PASS: exact bootloader/application readback, EEPROM marker, and boot fuses verified.\n");
}

let latestJournal = "";
async function captureEvidence(status) {
  if(status) writeFileSync(join(evidenceRoot, "latest-status.json"), `${JSON.stringify(status, null, 2)}\n`);
  try {
    const response = await fetchWithTimeout(`${baseUrl}/avr-update/log`, { cache: "no-store" }, 5000);
    if(response.ok) {
      latestJournal = await response.text();
      writeFileSync(join(evidenceRoot, "avr-update.log"), latestJournal);
    }
  } catch(error) {
    /* A missing route is expected while the ESP or access point is restarting. */
  }
}
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
  if(qualificationEspRestartPage) {
    startBody.set("qualificationConfirm", "ARM-ESP-RESTART");
    startBody.set("qualificationRestartPage", String(qualificationEspRestartPage));
  }
  if(qualificationAvrResetPage) {
    startBody.set("qualificationConfirm", "ARM-AVR-RESET");
    startBody.set("qualificationResetPage", String(qualificationAvrResetPage));
  }
  const startResponse = await fetchWithTimeout(`${baseUrl}/avr-update/start`, {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: startBody,
  }, 15000);
  if(startResponse.status !== 202) fail(`bootloader entry returned HTTP ${startResponse.status}: ${await startResponse.text()}`);
} catch(error) {
  process.stdout.write("The ESP powered down before the HTTP response completed; continuing recovery polling.\n");
}

process.stdout.write(`Waiting up to ${Math.ceil(verifyTimeoutMs / 60000)} minutes for programming and application verification...\n`);
const deadline = Date.now() + verifyTimeoutMs;
let avrResetInjected = false;
let avrResetDroppedEspPower = false;
while(Date.now() < deadline) {
  await new Promise((resolvePromise) => setTimeout(
    resolvePromise,
    qualificationAvrResetPage && !avrResetInjected ? 1000 : 5000,
  ));
  try {
    if(qualificationAvrResetPage && !avrResetInjected) {
      const qualificationResponse = await fetchWithTimeout(
        `${baseUrl}/avr-update/qualification/status`,
        { cache: "no-store" },
        2000,
      );
      if(qualificationResponse.ok) {
        const qualification = await qualificationResponse.json();
        if(qualification.resetReady) {
          writeFileSync(
            join(evidenceRoot, "avr-reset-qualification-ready.json"),
            `${JSON.stringify(qualification, null, 2)}\n`,
          );
          if(qualification.armedPage !== qualificationAvrResetPage) {
            fail(`device paused at qualification page ${qualification.armedPage}, expected ${qualificationAvrResetPage}`);
          }
          process.stdout.write(`Injecting an Atmel-ICE reset after verified page ${qualificationAvrResetPage}...\n`);
          const reset = spawnSync(
            "avrdude",
            ["-c", "atmelice_updi", "-p", "128da48", "-P", "usb", "-B", "10", "-n", "-v"],
            { encoding: "utf8", timeout: 45000, maxBuffer: 4 * 1024 * 1024 },
          );
          writeFileSync(join(evidenceRoot, "atmel-ice-reset.stdout.txt"), reset.stdout || "");
          writeFileSync(join(evidenceRoot, "atmel-ice-reset.stderr.txt"), reset.stderr || "");
          writeFileSync(join(evidenceRoot, "atmel-ice-reset-result.json"), `${JSON.stringify({
            status: reset.status,
            signal: reset.signal,
            error: reset.error?.message || "",
          }, null, 2)}\n`);
          if(reset.error || reset.status !== 0) {
            fail(`Atmel-ICE reset injection failed; evidence retained at ${evidenceRoot}`);
          }
          avrResetInjected = true;
          try {
            const continueResponse = await fetchWithTimeout(
              `${baseUrl}/avr-update/qualification/continue`,
              {
                method: "POST",
                headers: { "Content-Type": "application/x-www-form-urlencoded" },
                body: new URLSearchParams({ confirm: "CONTINUE-AFTER-AVR-RESET" }),
              },
              5000,
            );
            if(!continueResponse.ok) {
              fail(`device rejected continuation after the injected AVR reset: HTTP ${continueResponse.status}`);
            }
            process.stdout.write("AVR reset completed without dropping ESP power; qualification pause released.\n");
          } catch(error) {
            /* AVR reset can pull its ESP regulator-enable output low before the
             * bootloader restores it. Loss of this HTTP exchange is therefore
             * an expected, stronger interruption; persistent state must replay. */
            avrResetDroppedEspPower = true;
            writeFileSync(join(evidenceRoot, "avr-reset-esp-power-interruption.txt"),
              "The Atmel-ICE reset made the ESP qualification endpoint unreachable; cold replay was required.\n");
            process.stdout.write("AVR reset also interrupted ESP power; waiting for autonomous cold replay...\n");
          }
          process.stdout.write("Atmel-ICE reset recorded; waiting for autonomous replay and verification...\n");
        }
      }
    }
    const response = await fetchWithTimeout(`${baseUrl}/avr-update/status`, { cache: "no-store" }, 5000);
    if(!response.ok) continue;
    const status = await response.json();
    await captureEvidence(status);
    process.stdout.write(`Recovery state: ${status.phase}${status.pageCount ? ` (${status.nextPage}/${status.pageCount})` : ""}\n`);
    if(status.phase === "complete" && status.targetVersion === manifest.applicationVersion) {
      if(qualificationAvrResetPage && !avrResetInjected) {
        fail("AVR update completed without reaching the armed Atmel-ICE reset hook");
      }
      if(qualificationEspRestartPage &&
         !latestJournal.includes(`qualification-esp-restart page=${qualificationEspRestartPage}`)) {
        fail("AVR update completed without journal evidence of the armed ESP restart");
      }
      if((qualificationEspRestartPage || qualificationAvrResetPage) &&
         (latestJournal.match(/bootloader-session-begun/g)?.length ?? 0) < 2) {
        fail("AVR update completed without journal evidence of a full post-interruption session replay");
      }
      if(qualificationAvrResetPage && !avrResetDroppedEspPower &&
         !latestJournal.includes("failure operation=")) {
        fail("AVR update completed without recording the expected interrupted AVR transaction");
      }
      if(qualificationFinalReadback) runFinalAtmelIceReadback();
      process.stdout.write(`PASS: AVR ${manifest.applicationVersion} programmed, booted, and reported its installed version.\n`);
      process.stdout.write(`Evidence: ${evidenceRoot}\n`);
      process.exit(0);
    }
    if(status.diagnostic) {
      fail(`AVR programming stopped at page ${status.nextPage}/${status.pageCount}: ${status.diagnostic}; keep power connected and retry only after inspection`);
    }
    if(status.phase === "staged") fail("AVR rejected bootloader entry; verify that the event and manual transmitter are idle");
  } catch(error) {
    /* Power cycling and AP reassociation are expected during the transaction. */
  }
}

fail(`verification window expired after ${Math.ceil(verifyTimeoutMs / 60000)} minutes; programming may still be active, so keep power connected and reconnect to inspect /avr-update/status`);
