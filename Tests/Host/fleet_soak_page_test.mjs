#!/usr/bin/env node

import assert from "node:assert/strict";
import { spawnSync } from "node:child_process";
import { existsSync, mkdtempSync, readFileSync, rmSync } from "node:fs";
import { tmpdir } from "node:os";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "../..");
const optionalPagePath = join(repoRoot, "Software/Huzzah/optional/fleet-soak.html");
const baselinePagePath = join(repoRoot, "Software/Huzzah/ARDF_Transmitter/data/fleet-soak.html");
const source = readFileSync(optionalPagePath, "utf8");

assert.equal(existsSync(baselinePagePath), false, "Fleet Soak must remain absent from the baseline LittleFS image");
assert.match(source, /const reservedNames = Array\.from\(\{length: 12\}/);
assert.match(source, /id="scheduleStart" type="datetime-local"/);
assert.match(source, /Prepare master and arm fox provisioning/);
assert.match(source, /id="nextAssignment" class="instruction"/);
assert.match(source, /id="provisionConfirmation" class="confirmation muted"/);
assert.match(source, /id="removeSelf" class="danger" disabled/);
assert.match(source, /id="abortRun" class="danger" disabled/);
assert.match(source, /TURN ON NOW: \$\{plan\.label\.toUpperCase\(\)\}/);
assert.match(source, /\$\{entry\.label\.toUpperCase\(\)\} COMPLETE/);
assert.match(source, /The page will not advance to the next role/);
assert.match(source, /entry\.cleanupStatus === "ok"/);
assert.match(source, /all required foxes and the master are clean/);
assert.match(source, /ABORT_RESERVED_SUITE/);
assert.match(source, /FLEET_SOAK_ABORT,\$\{entry\.ssid\}/);
assert.match(source, /TURN ON FOR ABORT/);
assert.match(source, /Not in abort roster; no stop or deletion command sent/);
assert.match(source, /Re-arm target abort/);
assert.match(source, /armCleanup"\)\.disabled = provisioningState\.abortStarted/);
assert.match(source, /new URLSearchParams\(\{name: "\/fleet-soak\.html"\}\)/);
assert.match(source, /result\.includes\("File deleted successfully!"\)/);
assert.match(source, /FLEET_SOAK_MODE,\$\{mode\}/);
assert.match(source, /FLEET_SOAK_ASSIGN,\$\{entry\.ssid\},\$\{entry\.assignment\}/);
assert.match(source, /location\.hostname === "127\.0\.0\.1" && location\.port === "18080"/);
assert.match(source, /\? "18081" : "81"/);
assert.match(source, /candidate\.send\("!&"\)/);
assert.match(source, /}, 5000\);/);
assert.match(source, /if \(socket !== candidate\) return/);
assert.match(source, /\{label: "Slow 1", assignment: "0:0", frequency: "3\.520 MHz"\}/);
assert.match(source, /\{label: "Fast 5", assignment: "1:4", frequency: "3\.560 MHz"\}/);
assert.match(source, /\/fleet-soak\/activate/);
assert.match(source, /\/fleet-soak\/cleanup/);
assert.match(source, /\/fleet-soak\/abort/);
assert.doesNotMatch(source, /CLONE_PRUNE_EVENTS/);
assert.equal((source.match(/fetch\("\/delete"/g) || []).length, 1,
  "Fleet Soak page may invoke the existing delete handler only once with its fixed self path");
assert.doesNotMatch(source, /\/delete\.html/);

const script = source.match(/<script>([\s\S]*?)<\/script>/)?.[1];
assert.ok(script, "Fleet Soak page must contain an inline script");
new Function(script);

const provisioningPlanSource = script.match(/const provisioningPlan = (\[[\s\S]*?\]);/)?.[1];
assert.ok(provisioningPlanSource, "Fleet Soak page must define its deterministic provisioning plan");
const provisioningPlan = new Function(`return ${provisioningPlanSource};`)();
assert.deepEqual(
  provisioningPlan.map(({label, assignment, frequency}) => [label, assignment, frequency]),
  [
    ["Slow 1", "0:0", "3.520 MHz"],
    ["Slow 2", "0:1", "3.520 MHz"],
    ["Slow 3", "0:2", "3.520 MHz"],
    ["Slow 4", "0:3", "3.520 MHz"],
    ["Slow 5", "0:4", "3.520 MHz"],
    ["Fast 1", "1:0", "3.560 MHz"],
    ["Fast 2", "1:1", "3.560 MHz"],
    ["Fast 3", "1:2", "3.560 MHz"],
    ["Fast 4", "1:3", "3.560 MHz"],
    ["Fast 5", "1:4", "3.560 MHz"],
  ],
);

const cleanupProgressSource = script.match(
  /function cleanupProgress\(state, normalTargetCount\) \{[\s\S]*?\n\}(?=\nfunction currentCleanupProgress)/,
)?.[0];
assert.ok(cleanupProgressSource, "Fleet Soak page must expose its pure cleanup completion policy");
const cleanupProgress = new Function(`${cleanupProgressSource}\nreturn cleanupProgress;`)();
const normalEntries = provisioningPlan.map((plan, index) => ({...plan, ssid: `Tx_${index}`, status: "ok", cleanupStatus: "ok"}));
assert.equal(cleanupProgress({entries: normalEntries}, 10).complete, true);
assert.equal(cleanupProgress({entries: normalEntries.slice(0, 9)}, 10).requirementKnown, false);
const abortEntries = normalEntries.slice(0, 2).map(entry => ({...entry, status: "pending"}));
const partialAbort = cleanupProgress({
  abortStarted: true,
  abortExpectedSsids: abortEntries.map(entry => entry.ssid),
  entries: [{...abortEntries[0], cleanupStatus: "ok"}, {...abortEntries[1], cleanupStatus: "pending"}, normalEntries[2]],
}, 10);
assert.equal(partialAbort.requiredCount, 2);
assert.equal(partialAbort.completedCount, 1);
assert.equal(partialAbort.complete, false);
assert.equal(cleanupProgress({
  abortStarted: true,
  abortExpectedSsids: abortEntries.map(entry => entry.ssid),
  entries: abortEntries.map(entry => ({...entry, cleanupStatus: "ok"})),
}, 10).complete, true);
assert.equal(cleanupProgress({abortStarted: true, abortExpectedSsids: [], entries: []}, 10).complete, true);

const generatorBlock = script.match(
  /\/\/ BEGIN FLEET_SOAK_GENERATOR([\s\S]*?)\/\/ END FLEET_SOAK_GENERATOR/,
)?.[1];
assert.ok(generatorBlock, "Fleet Soak page must expose its pure browser generator for parity testing");
const buildFleetSoakEvents = new Function(`${generatorBlock}\nreturn buildFleetSoakEvents;`)();
const startUtc = "2031-08-09T14:00:00Z";
const pageEvents = buildFleetSoakEvents(Date.parse(startUtc));
assert.equal(pageEvents.length, 12);

const temporaryRoot = mkdtempSync(join(tmpdir(), "flexfox-fleet-soak-page-"));
const outputRoot = join(temporaryRoot, "bundle");
try {
  const generated = spawnSync(
    process.execPath,
    [
      join(repoRoot, "scripts/generate-flexfox-fleet-soak-events.mjs"),
      "--start", startUtc,
      "--output", outputRoot,
      "--allow-short-lead",
    ],
    {encoding: "utf8"},
  );
  assert.equal(generated.status, 0, generated.stderr || generated.stdout);
  for (const event of pageEvents) {
    assert.equal(
      event.contents,
      readFileSync(join(outputRoot, event.filename), "utf8"),
      `${event.filename} differs from the checked-in host generator`,
    );
  }
} finally {
  rmSync(temporaryRoot, {recursive: true, force: true});
}

console.log("PASS optional Fleet Soak page generates the host-tested suite and assigns foxes in fixed order");
