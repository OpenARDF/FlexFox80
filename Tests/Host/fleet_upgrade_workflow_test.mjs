#!/usr/bin/env node

import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import {
  espStatusMatchesArtifact,
  expectedMasterValue,
  fleetWebFiles,
  normalizeFleetUnitId,
  normalizeFlexFoxSsid,
  parseAdbDevices,
  parseProbeReplies,
  probeTemperatureIsPlausible,
  selectAdbDevice,
  wifiStatusMatchesSsid,
} from "../../scripts/lib/flexfox-fleet-upgrade.mjs";

function pass(name) {
  console.log(`PASS ${name}`);
}

assert.equal(normalizeFleetUnitId("fox-01"), "fox-01");
assert.equal(normalizeFleetUnitId("beacon"), "beacon");
assert.equal(normalizeFleetUnitId("fox 01"), undefined);
assert.equal(normalizeFleetUnitId("../fox"), undefined);
pass("fleet unit labels are safe for evidence paths");

assert.equal(normalizeFlexFoxSsid("Tx_Master"), "Tx_Master");
assert.equal(normalizeFlexFoxSsid("Tx_7C2D6FD3"), "Tx_7C2D6FD3");
assert.equal(normalizeFlexFoxSsid("Tx_7c2d6fd3"), undefined);
assert.equal(normalizeFlexFoxSsid("Tx_Master; reboot"), undefined);
pass("target SSIDs are exact FlexFox names");

const adbOutput = `List of devices attached
10.233.164.103:5555 device product:fogo_g model:moto_g device:fogo transport_id:6
emulator-5554 offline transport_id:7
`;
assert.deepEqual(parseAdbDevices(adbOutput), ["10.233.164.103:5555"]);
assert.equal(selectAdbDevice(["10.233.164.103:5555"], ""), "10.233.164.103:5555");
assert.throws(() => selectAdbDevice([], ""), /exactly one/);
assert.throws(() => selectAdbDevice(["a", "b"], ""), /exactly one/);
assert.throws(() => selectAdbDevice(["a"], "b"), /unavailable/);
pass("ADB selection fails closed when the Moto is ambiguous");

assert.equal(
  wifiStatusMatchesSsid('Wifi is connected to "Tx_7C2D6FD3"', "Tx_7C2D6FD3"),
  true,
);
assert.equal(
  wifiStatusMatchesSsid('Wifi is connected to "Tx_7C2D6FD4"', "Tx_7C2D6FD3"),
  false,
);
pass("WiFi association requires the exact requested SSID");

const artifact = {
  version: "2.22",
  bytes: 563888,
  installedMd5Values: new Set(["ce7ed4ed788edb346e05f8e87b36047e"]),
};
const matchingStatus = {
  version: "2.22",
  filesystemProtected: true,
  currentSketchBytes: 563888,
  currentSketchMd5: "CE7ED4ED788EDB346E05F8E87B36047E",
};
assert.equal(espStatusMatchesArtifact(matchingStatus, artifact), true);
assert.equal(espStatusMatchesArtifact({ ...matchingStatus, version: "2.21" }, artifact), false);
assert.equal(espStatusMatchesArtifact({ ...matchingStatus, filesystemProtected: false }, artifact), false);
assert.equal(espStatusMatchesArtifact({ ...matchingStatus, currentSketchBytes: 1 }, artifact), false);
assert.equal(espStatusMatchesArtifact({ ...matchingStatus, currentSketchMd5: "wrong" }, artifact), false);
pass("ESP skip requires version, exact installed MD5, size, and LittleFS protection");

const replies = parseProbeReplies(`PASS HTTP 200
RECV TEMP,29.0C
RECV SSID,Tx_Master
RECV MAC,90:B9:F9:C5:BB:22
RECV SW_VERSIONS,2.22,0.208
RECV MASTER,1
`);
assert.equal(replies.SSID, "Tx_Master");
assert.equal(replies.MAC, "90:B9:F9:C5:BB:22");
assert.equal(replies.SW_VERSIONS, "2.22,0.208");
assert.equal(replies.MASTER, "1");
assert.equal(expectedMasterValue("Tx_Master"), "1");
assert.equal(expectedMasterValue("Tx_7C2D6FD3"), "0");
pass("probe replies retain exact identity, versions, and role");

assert.equal(probeTemperatureIsPlausible("25.0C"), true);
assert.equal(probeTemperatureIsPlausible("-40.5C"), true);
assert.equal(probeTemperatureIsPlausible("230.0C"), false);
assert.equal(probeTemperatureIsPlausible("-273.0C"), false);
assert.equal(probeTemperatureIsPlausible("25.0"), false);
pass("fleet telemetry rejects impossible or malformed AVR temperatures");

assert.deepEqual(fleetWebFiles, ["events.html", "radio.html", "test.html"]);
assert.equal(fleetWebFiles.every((name) => /^[A-Za-z0-9._-]+$/.test(name)), true);
assert.equal(fleetWebFiles.some((name) => name.endsWith(".event") || name.endsWith(".me")), false);
pass("fleet overlay is restricted to the three approved web files");

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const workflowSource = readFileSync(
  resolve(repoRoot, "scripts", "upgrade-flexfox-fleet-unit.mjs"),
  "utf8",
);
for (const recipe of [
  "wifi-esp-update",
  "avr-provision-boot-chain",
  "wifi-web-deploy",
  "wifi-probe",
]) {
  assert.equal(workflowSource.includes(recipe), true, `missing reused recipe ${recipe}`);
}
assert.equal(workflowSource.includes("FLEXFOX_FLEET_UPGRADE_CONFIRM"), true);
assert.equal(workflowSource.includes("FLEXFOX_PROVISION_SKIP_IF_CURRENT"), true);
assert.equal(workflowSource.includes("runWithWifiRecovery"), true);
assert.equal(workflowSource.includes("remoteFileMatches"), true);
assert.equal(workflowSource.includes("ARDF_Transmitter.littlefs.bin"), false);
const espPhase = workflowSource.indexOf("Updating ESP wirelessly");
const avrPhase = workflowSource.indexOf("Verifying or provisioning");
const webPhase = workflowSource.indexOf("summary.phases.web = {}", avrPhase);
const finalPhase = workflowSource.indexOf("Running final combined firmware");
assert.equal(espPhase < avrPhase && avrPhase < webPhase && webPhase < finalPhase, true);
pass("fleet wrapper reuses guarded recipes in ESP, AVR, web, and final-probe order");
