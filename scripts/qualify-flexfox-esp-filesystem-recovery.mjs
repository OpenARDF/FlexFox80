#!/usr/bin/env node

import { existsSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { spawnSync } from "node:child_process";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const updater = join(repoRoot, "scripts", "update-flexfox-esp-over-wifi.mjs");
const recoveryImage = join(
  repoRoot,
  "Software",
  "Huzzah",
  "tmp",
  "esp-build-filesystem-recovery",
  "ARDF_Transmitter.ino.bin",
);
const normalImage = join(
  repoRoot,
  "Software",
  "Huzzah",
  "tmp",
  "esp-build",
  "ARDF_Transmitter.ino.bin",
);
const expectedSsid = process.env.FLEXFOX_EXPECTED_DEVICE_SSID;

if (process.env.FLEXFOX_ESP_RECOVERY_QUALIFICATION_CONFIRM !== "QUALIFY ESP FILESYSTEM RECOVERY") {
  throw new Error(
    "Set FLEXFOX_ESP_RECOVERY_QUALIFICATION_CONFIRM='QUALIFY ESP FILESYSTEM RECOVERY' to authorize the two-image qualification",
  );
}
if (!expectedSsid || !/^Tx_[0-9A-F]{8}$/.test(expectedSsid)) {
  throw new Error("Set FLEXFOX_EXPECTED_DEVICE_SSID to the exact MAC-derived Tx_ device SSID");
}
for (const image of [recoveryImage, normalImage]) {
  if (!existsSync(image)) throw new Error(`required ESP build is missing: ${image}`);
}

function install(image, expectedRecovery) {
  const result = spawnSync(process.execPath, [updater], {
    cwd: repoRoot,
    env: {
      ...process.env,
      FLEXFOX_FIRMWARE_BIN: image,
      FLEXFOX_UPDATE_CONFIRM: "UPDATE FLEXFOX ESP",
      FLEXFOX_EXPECTED_FILESYSTEM_RECOVERY: expectedRecovery ? "1" : "0",
    },
    encoding: "utf8",
    maxBuffer: 16 * 1024 * 1024,
  });
  if (result.stdout) process.stdout.write(result.stdout);
  if (result.stderr) process.stderr.write(result.stderr);
  if (result.error) throw result.error;
  if (result.status !== 0) {
    throw new Error(`ESP ${expectedRecovery ? "recovery" : "normal"} installation failed with status ${result.status}`);
  }
}

console.log(`Qualification target: ${expectedSsid}`);
console.log("Installing the non-destructive recovery-only image...");
install(recoveryImage, true);
console.log("Reinstalling the normal mounted-filesystem image through the recovery updater...");
install(normalImage, false);
console.log("PASS filesystem recovery service and return-to-normal update path completed");
