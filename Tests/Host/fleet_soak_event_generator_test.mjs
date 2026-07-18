#!/usr/bin/env node

import assert from "node:assert/strict";
import { createHash } from "node:crypto";
import { mkdtempSync, readFileSync, readdirSync, rmSync } from "node:fs";
import { tmpdir } from "node:os";
import { dirname, join, resolve } from "node:path";
import { spawnSync } from "node:child_process";
import { fileURLToPath } from "node:url";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const generator = join(repoRoot, "scripts", "generate-flexfox-fleet-soak-events.mjs");
const temporaryRoot = mkdtempSync(join(tmpdir(), "flexfox-fleet-soak-test-"));
const outputRoot = join(temporaryRoot, "bundle");
const scheduleStart = "2099-01-01T00:00:00Z";

function runGenerator(argumentsList) {
  return spawnSync(process.execPath, [generator, ...argumentsList], {
    cwd: repoRoot,
    encoding: "utf8",
  });
}

function eventFields(contents) {
  const fields = new Map();
  for (const line of contents.trim().split(/\r?\n/)) {
    const comma = line.indexOf(",");
    if (comma < 0) continue;
    const key = line.slice(0, comma).trim();
    const value = line.slice(comma + 1).trim().replace(/^"|"$/g, "");
    fields.set(key, value);
  }
  return fields;
}

try {
  const generated = runGenerator(["--start", scheduleStart, "--output", outputRoot]);
  assert.equal(generated.status, 0, generated.stderr);
  assert.match(generated.stdout, /PASS: generated 12 events/);

  const filenames = readdirSync(outputRoot).sort();
  const eventFilenames = filenames.filter((name) => name.endsWith(".event"));
  assert.equal(eventFilenames.length, 12);
  assert.deepEqual(filenames.filter((name) => !name.endsWith(".event")), ["README.txt", "fleet-soak-schedule.json"]);

  const manifest = JSON.parse(readFileSync(join(outputRoot, "fleet-soak-schedule.json"), "utf8"));
  assert.equal(manifest.format, "flexfox-fleet-soak-events-v1");
  assert.equal(manifest.scheduleStartUtc, scheduleStart);
  assert.equal(manifest.scheduleFinishUtc, "2099-01-01T04:41:00Z");
  assert.equal(manifest.totalMinutes, 281);
  assert.equal(manifest.eventCount, 12);
  assert.equal(manifest.sprintEventCount, 6);
  assert.equal(manifest.classicEventCount, 6);
  assert.equal(manifest.events.length, 12);
  assert.equal(manifest.assignmentPlan.units1Through5, "0:0 through 0:4");
  assert.equal(manifest.assignmentPlan.units6Through10, "1:0 through 1:4");
  assert.deepEqual(manifest.assignmentPlan.roleFrequenciesHz, [3520000, 3560000, 3540000, 3600000]);

  const expectedDurations = [10, 12, 15, 10, 12, 15, 10, 12, 15, 10, 12, 15];
  const expectedGaps = [10, 12, 15, 10, 12, 15, 10, 12, 15, 10, 12, 0];
  for (const [index, event] of manifest.events.entries()) {
    const expectedKind = index % 2 === 0 ? "Sprint" : "Classic";
    assert.equal(event.kind, expectedKind);
    assert.equal(event.durationMinutes, expectedDurations[index]);
    assert.equal(event.gapAfterMinutes, expectedGaps[index]);
    assert.ok(event.filename.length <= 31);
    assert.ok(event.bytes > 0);

    const contents = readFileSync(join(outputRoot, event.filename), "utf8");
    const lines = contents.trim().split(/\r?\n/);
    const fields = eventFields(contents);
    assert.equal(lines[0], "EVENT_START");
    assert.equal(lines.at(-1), "EVENT_END");
    assert.ok(lines.length <= 200);
    assert.equal(fields.get("EVENT_NAME"), event.name);
    assert.equal(fields.get("EVENT_VERSION"), "fleet-soak-v1");
    assert.equal(fields.get("EVENT_START_DATE_TIME"), event.startUtc);
    assert.equal(fields.get("EVENT_FINISH_DATE_TIME"), event.finishUtc);
    assert.equal((Date.parse(event.finishUtc) - Date.parse(event.startUtc)) / 60000, event.durationMinutes);
    assert.equal(createHash("sha256").update(contents).digest("hex"), event.sha256);
    assert.equal(lines.reduce((sum, line) => sum + line.trim().length, 0), event.transferLengthChecksum);
    assert.equal(lines.some((line) => line.startsWith("CHECK,")), false);

    if (expectedKind === "Sprint") {
      assert.equal(fields.get("EVENT_NUMBER_OF_TX_TYPES"), "4");
      assert.equal(fields.get("TYPE1_ROLE_NAME"), "Slow");
      assert.equal(fields.get("TYPE2_ROLE_NAME"), "Fast");
      assert.equal(fields.get("TYPE3_ROLE_NAME"), "Spectator");
      assert.equal(fields.get("TYPE4_ROLE_NAME"), "Beacon");
      assert.equal(fields.get("TYPE1_TX_COUNT"), "5");
      assert.equal(fields.get("TYPE2_TX_COUNT"), "5");
      assert.equal(fields.get("TYPE1_FREQ"), "3520000");
      assert.equal(fields.get("TYPE2_FREQ"), "3560000");
      assert.equal(fields.get("TYPE3_FREQ"), "3540000");
      assert.equal(fields.get("TYPE4_FREQ"), "3600000");
      assert.equal(fields.get("TYPE1_POWER_LEVEL"), "300");
      assert.equal(fields.get("TYPE2_POWER_LEVEL"), "300");
    } else {
      assert.equal(fields.get("EVENT_NUMBER_OF_TX_TYPES"), "4");
      assert.equal(fields.get("TYPE1_ROLE_NAME"), "Classic Fox");
      assert.equal(fields.get("TYPE2_ROLE_NAME"), "Alternate Classic Fox");
      assert.equal(fields.get("TYPE3_ROLE_NAME"), "Spectator");
      assert.equal(fields.get("TYPE4_ROLE_NAME"), "Beacon");
      assert.equal(fields.get("TYPE1_TX_COUNT"), "5");
      assert.equal(fields.get("TYPE2_TX_COUNT"), "5");
      assert.equal(fields.get("TYPE3_TX_COUNT"), "1");
      assert.equal(fields.get("TYPE4_TX_COUNT"), "1");
      assert.equal(fields.get("TYPE1_FREQ"), "3520000");
      assert.equal(fields.get("TYPE2_FREQ"), "3560000");
      assert.equal(fields.get("TYPE3_FREQ"), "3540000");
      assert.equal(fields.get("TYPE4_FREQ"), "3600000");
      assert.equal(fields.get("TYPE1_POWER_LEVEL"), "5000");
      assert.equal(fields.get("TYPE2_POWER_LEVEL"), "5000");
      assert.equal(fields.get("TYPE3_POWER_LEVEL"), "5000");
      assert.equal(fields.get("TYPE4_POWER_LEVEL"), "5000");
      assert.equal(fields.get("TYPE3_TX1_PATTERN"), "S ");
      for (let slot = 1; slot <= 5; slot += 1) {
        assert.equal(fields.get(`TYPE2_TX${slot}_PATTERN`), fields.get(`TYPE1_TX${slot}_PATTERN`));
        assert.equal(fields.get(`TYPE2_TX${slot}_ON_TIME`), "60");
        assert.equal(fields.get(`TYPE2_TX${slot}_OFF_TIME`), "240");
        assert.equal(fields.get(`TYPE2_TX${slot}_DELAY_TIME`), String((slot - 1) * 60));
      }
    }

    if (index + 1 < manifest.events.length) {
      const nextEvent = manifest.events[index + 1];
      assert.equal((Date.parse(nextEvent.startUtc) - Date.parse(event.finishUtc)) / 60000, event.gapAfterMinutes);
    }
  }

  const overwriteAttempt = runGenerator(["--start", scheduleStart, "--output", outputRoot]);
  assert.equal(overwriteAttempt.status, 2);
  assert.match(overwriteAttempt.stderr, /output directory already exists/);

  const tooSoonDate = new Date(Date.now() + 60_000);
  tooSoonDate.setUTCMilliseconds(0);
  const tooSoon = tooSoonDate.toISOString().replace(".000Z", "Z");
  const shortLeadAttempt = runGenerator(["--start", tooSoon, "--output", join(temporaryRoot, "too-soon")]);
  assert.equal(shortLeadAttempt.status, 2);
  assert.match(shortLeadAttempt.stderr, /at least 30 minutes in the future/);

  process.stdout.write("PASS fleet soak generator creates a deterministic 12-event, ten-unit schedule\n");
} finally {
  rmSync(temporaryRoot, { recursive: true, force: true });
}
