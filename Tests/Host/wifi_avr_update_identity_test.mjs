#!/usr/bin/env node

import assert from "node:assert/strict";
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

console.log("PASS AVR WiFi updater identity guards are exact and unattended-safe");
