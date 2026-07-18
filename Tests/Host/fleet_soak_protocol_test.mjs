#!/usr/bin/env node

import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "../..");
const read = path => readFileSync(join(repoRoot, path), "utf8");
const main = read("Software/Huzzah/ARDF_Transmitter/ARDF_Transmitter.ino");
const fleet = read("Software/Huzzah/ARDF_Transmitter/FleetSoak.cpp");
const names = read("Software/Huzzah/ARDF_Transmitter/fleet_soak.h");
const eventHeader = read("Software/Huzzah/ARDF_Transmitter/Event.h");

assert.match(eventHeader, /#define MAXIMUM_NUMBER_OF_EVENTS 25/);
assert.match(main, /fleetSoakPageIsAvailable\(g_IamMaster\)/);
assert.match(main, /masterCloneFileCount\(\)/);
assert.match(main, /fleetSoakStageReceivedEvent\(updatedFileName, path, &errorMessage\)/);
assert.match(main, /fleetSoakFinalizeStagedSuite\(g_slaveFleetSoakAssignment, &errorMessage\)/);
assert.match(main, /fleetSoakCleanupReservedEvents\(&removedCount, &errorMessage\)/);
assert.match(main, /targetOrdinaryEventCount \+ FLEET_SOAK_EVENT_COUNT <= MAXIMUM_NUMBER_OF_EVENTS/);

const ownershipValidator = fleet.match(
  /bool eventHasOwnedLayout\([\s\S]*?(?=\nString readStoredAssignment)/,
)?.[0];
assert.ok(ownershipValidator, "reserved event ownership validator must remain present");
assert.equal((ownershipValidator.match(/LittleFS\.open\(/g) || []).length, 1,
  "ownership validation should scan each event only once");
assert.match(ownershipValidator, /expectedNamePrefix/);
assert.match(ownershipValidator, /expectedCounts/);
assert.match(ownershipValidator, /expectedFrequencies/);
assert.match(fleet, /Event::validEventFile\(path\)/,
  "Fleet Soak ownership checks must retain mature event framing validation");

const sessionHandler = main.match(
  /else if \(msgHeader\.equalsIgnoreCase\(SOCK_COMMAND_FLEET_SOAK_SESSION\)\)([\s\S]*?)else if \(msgHeader\.equalsIgnoreCase\(SOCK_COMMAND_CLONE_PRUNE_EVENTS\)\)/,
)?.[1];
assert.ok(sessionHandler, "target Fleet Soak session handler must remain present");
assert.doesNotMatch(sessionHandler, /SLAVE_WAITING_FOR_FILES/, "assignment must not start transfer before clock sync finishes");
assert.match(sessionHandler, /currentEventFileIsActive\(&activeEventName\)/,
  "a target with an active event must reject Fleet Soak provisioning and cleanup");

const cleanup = fleet.match(
  /bool fleetSoakCleanupReservedEvents\([\s\S]*?\n\}/,
)?.[0];
assert.ok(cleanup, "scoped Fleet Soak cleanup must remain present");
assert.match(cleanup, /FLEET_SOAK_EVENT_PATHS\[index\]/);
assert.doesNotMatch(cleanup, /openDir|endsWith|FLEET_SOAK_ME_PATHS/);

for (let index = 1; index <= 12; index += 1) {
  const kind = index % 2 ? "Sprint" : "Classic";
  assert.match(names, new RegExp(`/FS${String(index).padStart(2, "0")}-${kind}\\.event`));
}

console.log("PASS Fleet Soak protocol remains opt-in, ordered, capacity-bounded, and exactly scoped");
