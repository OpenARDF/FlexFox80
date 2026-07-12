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
const linkbusBoundsTestPath = join(repoRoot, "scripts", "test-flexfox-linkbus-rx-bounds.mjs");
const linkbusPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "src",
  "linkbus.cpp",
);
const serialbusPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "src",
  "serialbus.cpp",
);
const driverIsrPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "driver_isr.cpp",
);
const source = readFileSync(eepromManagerPath, "utf8");
const header = readFileSync(eepromManagerHeaderPath, "utf8");
const driverInitHeader = readFileSync(driverInitHeaderPath, "utf8");
const wifiProbe = readFileSync(wifiProbePath, "utf8");
const linkbusBoundsTest = readFileSync(linkbusBoundsTestPath, "utf8");
const linkbus = readFileSync(linkbusPath, "utf8");
const serialbus = readFileSync(serialbusPath, "utf8");
const driverIsr = readFileSync(driverIsrPath, "utf8");
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

const rfPowerField = header.match(/\buint16_t\s+rf_power\s*;/);
if (!rfPowerField) {
  process.stderr.write("Firmware contract check failed: rf_power is not declared as uint16_t\n");
  process.exit(1);
}

const rfPowerUpdateCase = source.match(/case\s+RF_Power\s*:\s*\{([\s\S]*?)\}\s*break\s*;/);
if (!rfPowerUpdateCase) {
  process.stderr.write("Firmware contract check failed: RF_Power update case was not found\n");
  process.exit(1);
}

const rfPowerFailures = [];
if (
  !/avr_eeprom_write_word\s*\(\s*RF_Power\s*,\s*\*\s*\(\s*uint16_t\s*\*\s*\)\s*val\s*\)/.test(
    rfPowerUpdateCase[1],
  )
) {
  rfPowerFailures.push("RF_Power update does not write its uint16_t width");
}

if (
  !/g_80m_power_level_mW\s*=\s*EEPROM_TX_80M_POWER_MW_DEFAULT\s*;\s*avr_eeprom_write_word\s*\(\s*RF_Power\s*,\s*g_80m_power_level_mW\s*\)/.test(
    source,
  )
) {
  rfPowerFailures.push("RF_Power initialization does not write its uint16_t width");
}

if (rfPowerFailures.length > 0) {
  for (const failure of rfPowerFailures) {
    process.stderr.write(`Firmware contract check failed: ${failure}\n`);
  }
  process.exit(1);
}

process.stdout.write("PASS RF power writes preserve the uint16_t EEPROM width\n");

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

const unsafeTextSends = [
  ["Linkbus", linkbus],
  ["Serialbus", serialbus],
].filter(([, busSource]) => /sprintf\s*\(\s*\*\s*buff\s*,\s*text\s*\)/.test(busSource));

if (unsafeTextSends.length > 0) {
  process.stderr.write(
    `Firmware contract check failed: ${unsafeTextSends.map(([name]) => name).join(" and ")} text send treats data as a format string\n`,
  );
  process.exit(1);
}

const unboundedTextSends = [
  ["Linkbus", linkbus],
  ["Serialbus", serialbus],
].filter(
  ([, busSource]) =>
    !/copy_text_to_buffer\s*\(\s*\*\s*buff\s*,\s*sizeof\s*\(\s*\*\s*buff\s*\)\s*,\s*text\s*\)/.test(
      busSource,
    ),
);

if (unboundedTextSends.length > 0) {
  process.stderr.write(
    `Firmware contract check failed: ${unboundedTextSends.map(([name]) => name).join(" and ")} text send does not enforce its destination buffer size\n`,
  );
  process.exit(1);
}

process.stdout.write("PASS text send helpers copy literal data within destination bounds\n");

const requiredLinkbusRxGuards = [
  "linkbus_rx_id_can_append",
  "linkbus_rx_field_can_terminate",
  "linkbus_rx_can_start_next_field",
  "linkbus_rx_field_can_append",
];
const missingLinkbusRxGuards = requiredLinkbusRxGuards.filter(
  (guard) => !driverIsr.includes(guard),
);

if (missingLinkbusRxGuards.length > 0) {
  process.stderr.write(
    `Firmware contract check failed: Linkbus receive parser lacks ${missingLinkbusRxGuards.join(", ")} bounds guard(s)\n`,
  );
  process.exit(1);
}

process.stdout.write("PASS Linkbus receive parser guards field count and field length\n");

const expectedMalformedFrames =
  '["$ZZZ,ABCDEFGHIJKLMNOPQRSTU;", "$ZZZ,A,B,C,D;", "$AZRX?"]';
if (!linkbusBoundsTest.includes(`const malformedFrames = ${expectedMalformedFrames};`)) {
  process.stderr.write("Firmware contract check failed: Linkbus live test malformed frames changed\n");
  process.exit(1);
}
if (!linkbusBoundsTest.includes('const recoveryQuery = "$TEM?";')) {
  process.stderr.write("Firmware contract check failed: Linkbus live test recovery query is not $TEM?\n");
  process.exit(1);
}

const approvedLinkbusTestSends = [
  'socket.send("!&")',
  'socket.send(`PASS,${frame}`)',
  'socket.send(`PASS,${recoveryQuery}`)',
];
const linkbusTestSendCount = [...linkbusBoundsTest.matchAll(/socket\.send\(/g)].length;
const heartbeatSendCount = linkbusBoundsTest.split('socket.send("!&")').length - 1;
if (
  linkbusTestSendCount !== 4 ||
  heartbeatSendCount !== 2 ||
  approvedLinkbusTestSends.some((send) => !linkbusBoundsTest.includes(send))
) {
  process.stderr.write("Firmware contract check failed: Linkbus live test has unapproved sends\n");
  process.exit(1);
}

process.stdout.write("PASS Linkbus live test remains limited to malformed ZZZ frames and read-only recovery\n");
