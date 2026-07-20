#!/usr/bin/env node

import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import {
  createFlexFoxAdbRelay,
  flexFoxRelayUrls,
  parseFlexFoxRelayPort,
} from "../../scripts/lib/flexfox-adb-relay.mjs";

assert.equal(parseFlexFoxRelayPort("18080", "HTTP relay"), 18080);
assert.throws(() => parseFlexFoxRelayPort("80", "HTTP relay"), /1024 through 65535/);
assert.throws(() => parseFlexFoxRelayPort("not-a-port", "HTTP relay"), /1024 through 65535/);
assert.deepEqual(flexFoxRelayUrls(18080, 18081), {
  httpUrl: "http://127.0.0.1:18080/",
  webSocketUrl: "ws://127.0.0.1:18081/",
});

const calls = [];
const listeners = new Set();
const adb = async (args) => {
  calls.push(args);
  if (args.join(" ") === "shell cmd wifi status") {
    return { code: 0, stdout: 'Wifi is connected to "Tx_7C2D6FD3"' };
  }
  if (args[0] === "shell" && args[1] === "toybox" && args[2] === "nc") {
    return { code: listeners.has(Number(args.at(-1))) ? 0 : 1, stdout: "" };
  }
  if (args[0] === "shell" && args.length === 2 && args[1].includes("toybox nc -s")) {
    const port = Number(args[1].match(/ -p (\d+) /)?.[1]);
    listeners.add(port);
    return { code: 0, stdout: "" };
  }
  return { code: 0, stdout: "" };
};
const relay = createFlexFoxAdbRelay({ adb, expectedSsid: "Tx_7C2D6FD3" });
await relay.waitForWifiAssociation(1000);
await relay.prepareRelays();
assert.equal(listeners.has(18080), true);
assert.equal(listeners.has(18081), true);
assert.equal(calls.some(args => args.join(" ") === "forward tcp:18080 tcp:18080"), true);
assert.equal(calls.some(args => args.join(" ") === "forward tcp:18081 tcp:18081"), true);

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const fleetWorkflow = readFileSync(resolve(repoRoot, "scripts/upgrade-flexfox-fleet-unit.mjs"), "utf8");
const debugWorkflow = readFileSync(resolve(repoRoot, "scripts/prepare-flexfox-adb-relay.mjs"), "utf8");
const justfile = readFileSync(resolve(repoRoot, "Justfile"), "utf8");
assert.match(fleetWorkflow, /createFlexFoxAdbRelay/);
assert.match(debugWorkflow, /createFlexFoxAdbRelay/);
assert.match(debugWorkflow, /for \(let attempt = 1; attempt <= 5; attempt\+\+\)/);
assert.match(debugWorkflow, /await relay\.prepareRelays\(\)/);
assert.doesNotMatch(debugWorkflow, /run-droidtether|sudo -n|route -n add/);
assert.match(justfile, /^wifi-adb-relay ssid:/m);
assert.match(justfile, /^wifi-adb-probe ssid:/m);
assert.match(justfile, /^wifi-adb-monitor ssid:/m);

console.log("PASS Android-assisted debugging and fleet updates share the localhost-only ADB relay");
