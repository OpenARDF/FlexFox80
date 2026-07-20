#!/usr/bin/env node

import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import {
  normalizeMacDerivedDeviceSsid,
  unattendedUpdateNeedsExpectedSsid,
} from "../../scripts/lib/flexfox-avr-update-identity.mjs";

assert.equal(normalizeMacDerivedDeviceSsid("Tx_7C2D6FD3"), "TX_7C2D6FD3");
assert.equal(normalizeMacDerivedDeviceSsid("tx_7c2d6fd3"), "TX_7C2D6FD3");
assert.equal(normalizeMacDerivedDeviceSsid("Tx_Master"), null);
assert.equal(normalizeMacDerivedDeviceSsid("Tx_6FD3"), null);
assert.equal(normalizeMacDerivedDeviceSsid("Tx_7C2D6FDZ"), null);
assert.equal(normalizeMacDerivedDeviceSsid(null), null);

assert.equal(unattendedUpdateNeedsExpectedSsid(false, false, ""), true);
assert.equal(unattendedUpdateNeedsExpectedSsid(false, false, "TX_7C2D6FD3"), false);
assert.equal(unattendedUpdateNeedsExpectedSsid(false, true, ""), false);
assert.equal(unattendedUpdateNeedsExpectedSsid(true, false, ""), false);

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const updater = readFileSync(join(repoRoot, "scripts", "update-flexfox-avr-over-wifi.mjs"), "utf8");
assert.match(updater, /process\.env\.FLEXFOX_ADB_SERIAL \|\| qualificationAdbSerial/);
assert.match(updater, /process\.env\.FLEXFOX_RECONNECT_SSID/);
assert.match(updater, /const reconnectSsid = suppliedReconnectSsid \|\| preflight\.deviceSsid/);
assert.match(updater, /"cmd", "wifi", "connect-network"/);
assert.match(updater, /reconnectSsid, "open", "-r", "none"/);
assert.match(updater, /Date\.now\(\) - lastReassociationMillis >= 3000/);

console.log("PASS AVR WiFi updater identity guards and Moto reassociation are unattended-safe");
