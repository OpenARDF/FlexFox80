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
const clockObserverPath = join(repoRoot, "scripts", "observe-flexfox-clock.mjs");
const clockSyncTestPath = join(repoRoot, "scripts", "test-flexfox-clock-sync.mjs");
const clockPhaseTestPath = join(repoRoot, "scripts", "test-flexfox-clock-phase.mjs");
const cloneControlTestPath = join(repoRoot, "scripts", "test-flexfox-clone-controls.mjs");
const linkbusBoundsTestPath = join(repoRoot, "scripts", "test-flexfox-linkbus-rx-bounds.mjs");
const linkbusPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "src",
  "linkbus.cpp",
);
const linkbusHeaderPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "include",
  "linkbus.h",
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
const goertzelPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "src",
  "Goertzel.cpp",
);
const avrMainPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "main.cpp",
);
const tcbPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "src",
  "tcb.cpp",
);
const ds3231Path = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "src",
  "ds3231.cpp",
);
const ds3231HeaderPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "include",
  "ds3231.h",
);
const espMainPath = join(
  repoRoot,
  "Software",
  "Huzzah",
  "ARDF_Transmitter",
  "ARDF_Transmitter.ino",
);
const espHeaderPath = join(
  repoRoot,
  "Software",
  "Huzzah",
  "ARDF_Transmitter",
  "Transmitter.h",
);
const source = readFileSync(eepromManagerPath, "utf8");
const header = readFileSync(eepromManagerHeaderPath, "utf8");
const driverInitHeader = readFileSync(driverInitHeaderPath, "utf8");
const wifiProbe = readFileSync(wifiProbePath, "utf8");
const clockObserver = readFileSync(clockObserverPath, "utf8");
const clockSyncTest = readFileSync(clockSyncTestPath, "utf8");
const clockPhaseTest = readFileSync(clockPhaseTestPath, "utf8");
const cloneControlTest = readFileSync(cloneControlTestPath, "utf8");
const linkbusBoundsTest = readFileSync(linkbusBoundsTestPath, "utf8");
const linkbus = readFileSync(linkbusPath, "utf8");
const linkbusHeader = readFileSync(linkbusHeaderPath, "utf8");
const serialbus = readFileSync(serialbusPath, "utf8");
const driverIsr = readFileSync(driverIsrPath, "utf8");
const goertzel = readFileSync(goertzelPath, "utf8");
const avrMain = readFileSync(avrMainPath, "utf8");
const tcb = readFileSync(tcbPath, "utf8");
const ds3231 = readFileSync(ds3231Path, "utf8");
const ds3231Header = readFileSync(ds3231HeaderPath, "utf8");
const espMain = readFileSync(espMainPath, "utf8");
const espHeader = readFileSync(espHeaderPath, "utf8");
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

if (
  !goertzel.includes("int testData[MAXN];") ||
  goertzel.includes("malloc(") ||
  goertzel.includes("free(")
) {
  process.stderr.write(
    "Firmware contract check failed: fixed-size Goertzel samples still depend on fallible heap allocation\n",
  );
  process.exit(1);
}

process.stdout.write("PASS Goertzel sample storage is fixed at its existing maximum\n");

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

const clockObserverSends = [...clockObserver.matchAll(/socket\.send\(([^)]+)\)/g)].map(
  (match) => match[1].replace(/\s+/g, ""),
);
if (
  clockObserverSends.length !== 2 ||
  clockObserverSends.some((expression) => expression !== '"!&"')
) {
  process.stderr.write("Firmware contract check failed: clock observer sends more than heartbeat\n");
  process.exit(1);
}

process.stdout.write("PASS WiFi clock observer sends heartbeat only\n");

if (
  !clockSyncTest.includes('process.env.FLEXFOX_ALLOW_CLOCK_SET !== "1"') ||
  !clockSyncTest.includes('const message = `SYNC,${iso}`;') ||
  clockSyncTest.includes("PASS,") ||
  [...clockSyncTest.matchAll(/socket\.send\(([^)]+)\)/g)].length !== 3
) {
  process.stderr.write("Firmware contract check failed: clock sync test safety boundary changed\n");
  process.exit(1);
}

process.stdout.write("PASS WiFi clock sync test requires opt-in and sends only time/heartbeat\n");

if (
  !clockPhaseTest.includes('process.env.FLEXFOX_CLOCK_PHASE_TEST === "1"') ||
  !clockPhaseTest.includes('quiet: "$ESP,C;"') ||
  !clockPhaseTest.includes('edge: "$ESP,S;"') ||
  !clockPhaseTest.includes('resume: "$ESP,R;"') ||
  !clockPhaseTest.includes('`$TIM,${iso},C;`') ||
  !clockPhaseTest.includes("await bestEffortRestore") ||
  !clockPhaseTest.includes("if (quietRequested && socket?.readyState === WebSocket.OPEN)") ||
  clockPhaseTest.includes("$EVT,") ||
  clockPhaseTest.includes("$RF,")
) {
  process.stderr.write(
    "Firmware contract check failed: clock-phase test is not opt-in, bounded, and fail-safe\n",
  );
  process.exit(1);
}

process.stdout.write("PASS WiFi clock-phase test is opt-in and fail-safe\n");

if (
  !cloneControlTest.includes('process.env.FLEXFOX_CLONE_CONTROL_TEST === "1"') ||
  !cloneControlTest.includes('quiet: "$ESP,C;"') ||
  !cloneControlTest.includes('edge: "$ESP,S;"') ||
  !cloneControlTest.includes('resume: "$ESP,R;"') ||
  !cloneControlTest.includes("if (quietRequested && socket?.readyState === WebSocket.OPEN)") ||
  cloneControlTest.includes("$TIM,") ||
  (cloneControlTest.includes("SYNC,") && !cloneControlTest.includes('startsWith("SYNC,")'))
) {
  process.stderr.write(
    "Firmware contract check failed: clone-control live test is not opt-in, bounded, and fail-safe\n",
  );
  process.exit(1);
}

process.stdout.write("PASS WiFi clone-control test is opt-in and fail-safe\n");

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

process.stdout.write("PASS Linkbus receive parser guards ID length, field count, and field length\n");

if (!/enum\s+LBMessageID\s*:\s*uint32_t/.test(linkbusHeader)) {
  process.stderr.write("Firmware contract check failed: Linkbus message IDs are not collision-free uint32_t values\n");
  process.exit(1);
}
const canonicalLinkbusIds = [
  "LB_MESSAGE_BAND = LINKBUS_ID3('B', 'N', 'D')",
  "LB_MESSAGE_TX_MOD = LINKBUS_ID3('M', 'O', 'D')",
  "LB_MESSAGE_VER = LINKBUS_ID3('V', 'E', 'R')",
  "LB_MESSAGE_BAT = LINKBUS_ID3('B', 'A', 'T')",
  "LB_MESSAGE_TEMP = LINKBUS_ID3('T', 'E', 'M')",
  "LB_MESSAGE_SET_FREQ = LINKBUS_ID3('F', 'R', 'E')",
  "LB_MESSAGE_CLOCK = LINKBUS_ID3('T', 'I', 'M')",
  "LB_MESSAGE_STARTFINISH = LINKBUS_ID2('S', 'F')",
  "LB_MESSAGE_PERM = LINKBUS_ID3('P', 'R', 'M')",
  "LB_MESSAGE_TX_POWER = LINKBUS_ID3('P', 'O', 'W')",
  "LB_MESSAGE_SET_STATION_ID = LINKBUS_ID2('I', 'D')",
  "LB_MESSAGE_SET_PATTERN = LINKBUS_ID2('P', 'A')",
  "LB_MESSAGE_CODE_SPEED = LINKBUS_ID3('S', 'P', 'D')",
  "LB_MESSAGE_TIME_INTERVAL = LINKBUS_ID1('T')",
  "LB_MESSAGE_ESP_COMM = LINKBUS_ID3('E', 'S', 'P')",
  "LB_MESSAGE_GO = LINKBUS_ID2('G', 'O')",
  "LB_MESSAGE_KEY = LINKBUS_ID3('K', 'E', 'Y')",
  "LB_MESSAGE_RESET = LINKBUS_ID3('R', 'S', 'T')",
  "LB_MESSAGE_WIFI = LINKBUS_ID2('W', 'I')",
];
const missingCanonicalLinkbusIds = canonicalLinkbusIds.filter(
  (definition) => !linkbusHeader.includes(definition),
);
if (missingCanonicalLinkbusIds.length > 0) {
  process.stderr.write(
    `Firmware contract check failed: ${missingCanonicalLinkbusIds.length} Linkbus ID definition(s) do not preserve their wire bytes\n`,
  );
  process.exit(1);
}
if (!driverIsr.includes("linkbus_rx_id_append")) {
  process.stderr.write("Firmware contract check failed: Linkbus receive parser lacks collision-free ID accumulation\n");
  process.exit(1);
}

process.stdout.write("PASS Linkbus message IDs use collision-free byte encoding\n");

const expectedMalformedFrames =
  '["$ZZZ,ABCDEFGHIJKLMNOPQRSTU;", "$ZZZ,A,B,C,D;", "$AZRX?"]';
if (!linkbusBoundsTest.includes(`const unansweredFrames = ${expectedMalformedFrames};`)) {
  process.stderr.write("Firmware contract check failed: Linkbus live test unanswered frames changed\n");
  process.exit(1);
}
if (!linkbusBoundsTest.includes('const aliasProbe = "$RXW?";')) {
  process.stderr.write("Firmware contract check failed: Linkbus live test alias probe changed\n");
  process.exit(1);
}
if (!linkbusBoundsTest.includes('const recoveryQueries = ["$TEM?", "$BAT?"];')) {
  process.stderr.write("Firmware contract check failed: Linkbus live test recovery queries changed\n");
  process.exit(1);
}

const approvedLinkbusTestSends = [
  'socket.send("!&")',
  'socket.send(`PASS,${frame}`)',
  'socket.send(`PASS,${aliasProbe}`)',
  'socket.send(`PASS,${query}`)',
];
const linkbusTestSendCount = [...linkbusBoundsTest.matchAll(/socket\.send\(/g)].length;
const heartbeatSendCount = linkbusBoundsTest.split('socket.send("!&")').length - 1;
if (
  linkbusTestSendCount !== 5 ||
  heartbeatSendCount !== 2 ||
  approvedLinkbusTestSends.some((send) => !linkbusBoundsTest.includes(send))
) {
  process.stderr.write("Firmware contract check failed: Linkbus live test has unapproved sends\n");
  process.exit(1);
}

process.stdout.write("PASS Linkbus live test remains limited to approved malformed frames and read-only recovery\n");

const cloneQuietControls = ["f1 == 'C'", "f1 == 'S'", "f1 == 'R'"];
const missingCloneQuietControls = cloneQuietControls.filter(
  (control) => !avrMain.includes(control),
);
if (missingCloneQuietControls.length > 0) {
  process.stderr.write(
    `Firmware contract check failed: AVR clone quiet control lacks ${missingCloneQuietControls.join(", ")}\n`,
  );
  process.exit(1);
}

if (
  !avrMain.includes("g_clone_sync_report_armed") ||
  !avrMain.includes("g_clone_sync_report_ready") ||
  !avrMain.includes("g_clone_sync_epoch")
) {
  process.stderr.write(
    "Firmware contract check failed: clone sync is not armed in the RTC ISR and serviced in foreground\n",
  );
  process.exit(1);
}

const rtcIsr = avrMain.match(/ISR\(PORTA_PORT_vect\)\s*\{([\s\S]*?)\n\}/);
if (!rtcIsr || /lb_send_msg\s*\(/.test(rtcIsr[1])) {
  process.stderr.write(
    "Firmware contract check failed: RTC ISR performs Linkbus transmission\n",
  );
  process.exit(1);
}

const rtcBootSuccess = avrMain.match(
  /if\s*\(\s*code\s*==\s*ERROR_CODE_RTC_NONRESPONSIVE\s*\)\s*\{[\s\S]*?\}\s*else\s*\{([\s\S]*?)g_event_scheduled\s*=\s*eventScheduled\(\)\s*;\s*\}/,
);
if (
  !rtcBootSuccess ||
  !rtcBootSuccess[1].includes("syncSystemTimeToRTC()") ||
  rtcBootSuccess[1].includes("ds3231_get_epoch") ||
  rtcBootSuccess[1].includes("set_system_time")
) {
  process.stderr.write(
    "Firmware contract check failed: AVR boot does not align system time to an RTC edge\n",
  );
  process.exit(1);
}

process.stdout.write("PASS AVR boot aligns system time to an RTC edge\n");

if (
  !/VPORTA\.INTFLAGS\s*=\s*x[\s\S]*rtcElapsedEdges\(\)/.test(rtcIsr[1]) ||
  !/do\s*\{[\s\S]*system_tick\(\)[\s\S]*\}\s*while\s*\(\s*--elapsed_seconds\s*\)/.test(
    rtcIsr[1],
  )
) {
  process.stderr.write(
    "Firmware contract check failed: RTC ISR does not preserve and replay coalesced edges\n",
  );
  process.exit(1);
}

if (
  !/ISR\(TCB2_INT_vect\)[\s\S]*rtcEdgeTrackerObserve\s*\(\s*&g_rtc_edge_tracker\s*,\s*PORTA_get_pin_level\(RTC_SQW\)\s*\)/.test(
    tcb,
  ) ||
  !/uint8_t\s+rtcElapsedEdges\(\)[\s\S]*TCB2\.INTCTRL\s*=\s*0[\s\S]*rtcEdgeTrackerObserve[\s\S]*rtcEdgeTrackerTake[\s\S]*TCB2\.INTCTRL\s*=\s*interrupt_control/.test(
    tcb,
  )
) {
  process.stderr.write(
    "Firmware contract check failed: RTC edge observation is missing or not race-safe\n",
  );
  process.exit(1);
}

if (
  !/g_clone_sync_report_armed\s*&&\s*\(\s*elapsed_seconds\s*==\s*1\s*\)/.test(
    rtcIsr[1],
  )
) {
  process.stderr.write(
    "Firmware contract check failed: clone sync can report the oldest recovered RTC edge\n",
  );
  process.exit(1);
}

process.stdout.write("PASS delayed RTC edges are counted and replayed\n");

if (!/if\s*\(\s*g_report_seconds\s*&&\s*!g_clone_quiet\s*\)/.test(avrMain)) {
  process.stderr.write(
    "Firmware contract check failed: ordinary time reports are not suppressed during clone quiet mode\n",
  );
  process.exit(1);
}

const quietGatedReports = [
  "g_last_error_code && !g_clone_quiet",
  "g_last_status_code && !g_clone_quiet",
  "g_check_for_next_event && !g_clone_quiet",
];
const missingQuietGates = quietGatedReports.filter((gate) => !avrMain.includes(gate));
if (missingQuietGates.length > 0) {
  process.stderr.write(
    `Firmware contract check failed: clone quiet mode lacks ${missingQuietGates.join(", ")} gate(s)\n`,
  );
  process.exit(1);
}

if (!/while\s*\(\s*\(lb_buff = nextFullLBRxBuffer\(\)\)\s*\)\s*\{\s*bool send_ack = true;/.test(avrMain)) {
  process.stderr.write(
    "Firmware contract check failed: Linkbus ACK decision is not reset for every received frame\n",
  );
  process.exit(1);
}

process.stdout.write("PASS clone quiet mode arms at an RTC edge and transmits only from foreground\n");

if (
  !/bool\s+ds3231_set_date_time\s*\(/.test(ds3231Header) ||
  !/bool\s+ds3231_set_date_time\s*\(/.test(ds3231) ||
  !avrMain.includes('sprintf(g_tempStr, "C,%lu"')
) {
  process.stderr.write(
    "Firmware contract check failed: clone time write does not expose write/readback verification\n",
  );
  process.exit(1);
}

process.stdout.write("PASS clone time writes require RTC write success and emit an epoch readback\n");

const requiredEspCloneFrames = [
  '#define SLAVE_SYNC_READY "S"',
  '#define LB_MESSAGE_ESP_CLONE_QUIET "$ESP,C;"',
  '#define LB_MESSAGE_ESP_CLONE_SYNC "$ESP,S;"',
  '#define LB_MESSAGE_ESP_CLONE_RESUME "$ESP,R;"',
];
const missingEspCloneFrames = requiredEspCloneFrames.filter(
  (frame) => !espHeader.includes(frame),
);
if (missingEspCloneFrames.length > 0) {
  process.stderr.write(
    `Firmware contract check failed: ESP clone protocol lacks ${missingEspCloneFrames.join(", ")}\n`,
  );
  process.exit(1);
}

if (
  !espMain.includes("setAVRCloneQuietMode(true)") ||
  !espMain.includes("setAVRCloneQuietMode(false)") ||
  !espMain.includes("serviceMasterCloneHandshake()") ||
  !espMain.includes("g_LBOutputBuff->put(LB_MESSAGE_ESP_CLONE_SYNC)") ||
  !espMain.includes("endMasterCloneSession()") ||
  !espMain.includes("millis() - g_slaveSyncReadyStartedMillis")
) {
  process.stderr.write(
    "Firmware contract check failed: ESP clone session does not drain, quiet, confirm, and resume deterministically\n",
  );
  process.exit(1);
}

if (
  !espMain.includes("g_cloneClockVerified") ||
  !/g_cloneClockVerified\s*&&\s*g_LBOutputBuff->empty\(\)/.test(espMain) ||
  !espMain.includes('String msgOut = String(String(LB_MESSAGE_TIME_SET) + p + ",C;")') ||
  !espMain.includes("g_LBOutputBuff->put(msgOut)")
) {
  process.stderr.write(
    "Firmware contract check failed: ESP clone clock completion is not tied to a queued, verified RTC readback\n",
  );
  process.exit(1);
}

if (
  !espMain.includes("cloneTimeReply") ||
  !espMain.includes("type.equals(LB_MESSAGE_NACK)") ||
  !espMain.includes('payload.startsWith("C,")')
) {
  process.stderr.write(
    "Firmware contract check failed: ESP clone mode cannot consume NAK and clone-specific time readback\n",
  );
  process.exit(1);
}

process.stdout.write("PASS ESP clone handshake drains queues and requires matching RTC readback\n");

if (
  espMain.includes("g_webSocketServer.isRunning()") ||
  (espMain.match(/g_webSocketServer\.close\(\)/g) || []).length !== 1 ||
  !espMain.includes("bool g_webSocketServerStarted = false;") ||
  !espMain.includes("void stopWebSocketServer()") ||
  !espMain.includes("g_webSocketServerStarted = true;") ||
  !espMain.includes("g_webSocketServerStarted = false;")
) {
  process.stderr.write(
    "Firmware contract check failed: ESP WebSocket lifecycle depends on an untracked library extension\n",
  );
  process.exit(1);
}

process.stdout.write("PASS ESP WebSocket lifecycle uses source-owned state\n");
