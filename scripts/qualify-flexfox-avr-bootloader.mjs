#!/usr/bin/env node

import { existsSync, readFileSync } from "node:fs";
import { spawnSync } from "node:child_process";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { fetchWithTimeout } from "./lib/flexfox-http.mjs";
import { normalizeMacDerivedDeviceSsid } from "./lib/flexfox-avr-update-identity.mjs";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const updater = join(repoRoot, "scripts", "update-flexfox-avr-over-wifi.mjs");
const manifestPath = resolve(process.env.FLEXFOX_AVR_MANIFEST || join(
  repoRoot,
  "Software", "AVR128DA48", "tmp", "avr-boot-chain",
  "FlexFox80-AVR-Release-Info.json",
));
const host = process.env.FLEXFOX_HOST || "73.73.73.73";
const expectedSsid = normalizeMacDerivedDeviceSsid(process.env.FLEXFOX_EXPECTED_DEVICE_SSID || "");

function fail(message) {
  process.stderr.write(`FlexFox bootloader qualification: ${message}\n`);
  process.exit(2);
}

if(process.env.FLEXFOX_AVR_QUALIFICATION_CONFIRM !== "QUALIFY-BL0.3") {
  fail("set FLEXFOX_AVR_QUALIFICATION_CONFIRM=QUALIFY-BL0.3 to authorize three same-version pilot updates");
}
if(!expectedSsid) {
  fail("FLEXFOX_EXPECTED_DEVICE_SSID must be the complete MAC-derived Tx_XXXXXXXX pilot SSID");
}
if(!existsSync(manifestPath)) fail(`release manifest is missing: ${manifestPath}`);
const manifest = JSON.parse(readFileSync(manifestPath, "utf8"));
if(manifest.bootloaderVersion !== "BL0.3" || manifest.protocolVersion !== 2) {
  fail("the selected manifest is not the BL0.3 protocol-2 qualification candidate");
}

async function waitForPilot(label) {
  const deadline = Date.now() + 10 * 60 * 1000;
  process.stdout.write(`Waiting for ${expectedSsid} before ${label}...\n`);
  while(Date.now() < deadline) {
    try {
      const response = await fetchWithTimeout(`http://${host}/firmware/status`, { cache: "no-store" }, 3000);
      if(response.ok) return;
    } catch(error) {
      /* The saved Moto association and ESP access point may still be returning. */
    }
    await new Promise((resolvePromise) => setTimeout(resolvePromise, 5000));
  }
  fail(`pilot did not become reachable before ${label}; reconnect the Moto to ${expectedSsid} and rerun`);
}

function runProfile(profile) {
  process.stdout.write(`\n=== ${profile.label} ===\n`);
  const environment = {
    ...process.env,
    FLEXFOX_AVR_MANIFEST: manifestPath,
    FLEXFOX_EXPECTED_DEVICE_SSID: expectedSsid,
    FLEXFOX_AVR_SSID_SUFFIX: expectedSsid.slice(-4),
    FLEXFOX_AVR_UPDATE_CONFIRM: `UPDATE-AVR-${manifest.applicationVersion}`,
    FLEXFOX_AVR_QUALIFICATION_FINAL_READBACK: "1",
  };
  delete environment.FLEXFOX_AVR_QUALIFICATION_ESP_RESTART_PAGE;
  delete environment.FLEXFOX_AVR_QUALIFICATION_AVR_RESET_PAGE;
  if(profile.espRestartPage) {
    environment.FLEXFOX_AVR_QUALIFICATION_CONFIRM = "ARM-ESP-RESTART";
    environment.FLEXFOX_AVR_QUALIFICATION_ESP_RESTART_PAGE = String(profile.espRestartPage);
  } else if(profile.avrResetPage) {
    environment.FLEXFOX_AVR_QUALIFICATION_CONFIRM = "ARM-AVR-RESET";
    environment.FLEXFOX_AVR_QUALIFICATION_AVR_RESET_PAGE = String(profile.avrResetPage);
  } else {
    delete environment.FLEXFOX_AVR_QUALIFICATION_CONFIRM;
  }
  const result = spawnSync(process.execPath, [updater], {
    cwd: repoRoot,
    env: environment,
    stdio: ["ignore", "inherit", "inherit"],
    timeout: 40 * 60 * 1000,
  });
  if(result.error) fail(`${profile.label} could not run: ${result.error.message}`);
  if(result.status !== 0) fail(`${profile.label} failed with status ${result.status}`);
}

const middlePage = Math.max(2, Math.floor(manifest.imageBytes / manifest.pageSize / 2));
const profiles = [
  { label: "uninterrupted protocol-2 update plus exact readback" },
  { label: "ESP restart after verified page 8", espRestartPage: 8 },
  { label: `Atmel-ICE AVR reset after verified page ${middlePage}`, avrResetPage: middlePage },
];

for(const profile of profiles) {
  await waitForPilot(profile.label);
  runProfile(profile);
}

process.stdout.write("\nPASS: uninterrupted, ESP-restart, and AVR-reset qualification profiles completed with exact readback.\n");
process.stdout.write("A true removal/restoration of unit power during a separate update remains the only manual interruption gate.\n");
