#!/usr/bin/env node

import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const eepromManagerPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "src",
  "eeprommanager.cpp",
);
const eepromManagerHeaderPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "include",
  "eeprommanager.h",
);
const driverInitHeaderPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "include",
  "driver_init.h",
);
const wifiProbePath = join(repoRoot, "scripts", "probe-flexfox-wifi.mjs");
const source = readFileSync(eepromManagerPath, "utf8");
const header = readFileSync(eepromManagerHeaderPath, "utf8");
const driverInitHeader = readFileSync(driverInitHeaderPath, "utf8");
const wifiProbe = readFileSync(wifiProbePath, "utf8");
const declaration = source.match(/extern\s+volatile\s+Fox_t\s+g_fox\s*\[\s*([^\]]+?)\s*\]\s*;/);

if (!declaration) {
  process.stderr.write("Firmware contract check failed: g_fox extern declaration was not found\n");
  process.exit(1);
}

const declaredExtent = declaration[1].replace(/\s+/g, "");
if (declaredExtent !== "EVENT_NUMBER_OF_EVENTS") {
  process.stderr.write(
    `Firmware contract check failed: g_fox extent is ${declaredExtent}; expected EVENT_NUMBER_OF_EVENTS\n`,
  );
  process.exit(1);
}

process.stdout.write("PASS g_fox declaration covers every Event_t value\n");

const i2cFailureField = header.match(/\buint16_t\s+i2c_failure_count\s*;/);
if (!i2cFailureField) {
  process.stderr.write(
    "Firmware contract check failed: i2c_failure_count is not declared as uint16_t\n",
  );
  process.exit(1);
}

const i2cFailureUpdateCase = source.match(
  /case\s+I2C_failure_count\s*:\s*\{([\s\S]*?)\}\s*break\s*;/,
);
if (!i2cFailureUpdateCase) {
  process.stderr.write(
    "Firmware contract check failed: I2C_failure_count update case was not found\n",
  );
  process.exit(1);
}

const failures = [];
if (
  !/avr_eeprom_write_word\s*\(\s*I2C_failure_count\s*,\s*\*\s*\(\s*uint16_t\s*\*\s*\)\s*val\s*\)/.test(
    i2cFailureUpdateCase[1],
  )
) {
  failures.push("I2C_failure_count update does not write its full uint16_t value");
}

if (
  !/g_i2c_failure_count\s*=\s*0\s*;\s*avr_eeprom_write_word\s*\(\s*I2C_failure_count\s*,\s*g_i2c_failure_count\s*\)/.test(
    source,
  )
) {
  failures.push("I2C_failure_count initialization does not write its uint16_t width");
}

if (failures.length > 0) {
  for (const failure of failures) {
    process.stderr.write(`Firmware contract check failed: ${failure}\n`);
  }
  process.exit(1);
}

process.stdout.write("PASS I2C failure count writes preserve the uint16_t EEPROM width\n");

if (/^\s*#include\s*[<"][^>"\r\n]*\\[^>"\r\n]*[>"]/m.test(driverInitHeader)) {
  process.stderr.write(
    "Firmware contract check failed: driver_init.h contains a Windows-only include path\n",
  );
  process.exit(1);
}

process.stdout.write("PASS generated driver include paths are host-portable\n");

const safeRequestsMatch = wifiProbe.match(/const\s+safeRequests\s*=\s*\[([^\]]+)\]/);
if (!safeRequestsMatch) {
  process.stderr.write("Firmware contract check failed: WiFi probe safe request list was not found\n");
  process.exit(1);
}

const safeRequests = [...safeRequestsMatch[1].matchAll(/["']([^"']+)["']/g)].map(
  (match) => match[1],
);
const expectedSafeRequests = ["SSID", "MAC", "SW_VERSIONS", "MASTER"];
if (JSON.stringify(safeRequests) !== JSON.stringify(expectedSafeRequests)) {
  process.stderr.write(
    `Firmware contract check failed: WiFi probe requests are ${safeRequests.join(", ")}; expected ${expectedSafeRequests.join(", ")}\n`,
  );
  process.exit(1);
}

const socketSendExpressions = [...wifiProbe.matchAll(/socket\.send\(([^)]+)\)/g)].map((match) =>
  match[1].replace(/\s+/g, ""),
);
const allowedSendExpressions = new Set(['"!&"', "request"]);
const unsafeSendExpressions = socketSendExpressions.filter(
  (expression) => !allowedSendExpressions.has(expression),
);
if (unsafeSendExpressions.length > 0) {
  process.stderr.write(
    `Firmware contract check failed: WiFi probe has unapproved sends: ${unsafeSendExpressions.join(", ")}\n`,
  );
  process.exit(1);
}

process.stdout.write("PASS WiFi smoke probe remains read-only\n");
