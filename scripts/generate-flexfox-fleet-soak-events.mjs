#!/usr/bin/env node

import { createHash } from "node:crypto";
import { existsSync, mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const eventDataRoot = join(repoRoot, "Software", "Huzzah", "ARDF_Transmitter", "data");
const sprintTemplatePath = join(eventDataRoot, "Sprint80m.event");
const classicTemplatePath = join(eventDataRoot, "Classic80m.event");
const durationsMinutes = [10, 12, 15];
const gapsMinutes = [10, 12, 15];
const eventCount = 12;
const maximumEventFilenameLength = 31;
const maximumEventLines = 200;

function fail(message) {
  process.stderr.write(`FlexFox fleet soak events: ${message}\n`);
  process.exit(2);
}

function usage() {
  process.stdout.write(
    "Usage: node scripts/generate-flexfox-fleet-soak-events.mjs " +
      "--start YYYY-MM-DDTHH:MM:SSZ [--output DIRECTORY] [--allow-short-lead]\n",
  );
}

function parseArguments(argv) {
  const options = { allowShortLead: false };
  for (let index = 0; index < argv.length; index += 1) {
    const argument = argv[index];
    if (argument === "--start" || argument === "--output") {
      const value = argv[index + 1];
      if (!value || value.startsWith("--")) fail(`${argument} requires a value`);
      options[argument === "--start" ? "start" : "output"] = value;
      index += 1;
    } else if (argument === "--allow-short-lead") {
      options.allowShortLead = true;
    } else if (argument === "--help" || argument === "-h") {
      usage();
      process.exit(0);
    } else {
      fail(`unknown argument: ${argument}`);
    }
  }
  if (!options.start) fail("--start is required");
  return options;
}

function formatUtc(epochMillis) {
  return new Date(epochMillis).toISOString().replace(".000Z", "Z");
}

function strictUtcEpoch(value) {
  if (!/^\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z$/.test(value)) {
    fail(`invalid UTC start time: ${value}`);
  }
  const epoch = Date.parse(value);
  if (!Number.isFinite(epoch) || formatUtc(epoch) !== value) fail(`invalid UTC start time: ${value}`);
  return epoch;
}

function sourceLines(path) {
  const lines = readFileSync(path, "utf8").split(/\r?\n/).filter((line) => line.length > 0);
  if (lines[0] !== "EVENT_START" || lines.at(-1) !== "EVENT_END") {
    fail(`template is not a framed event file: ${path}`);
  }
  return lines;
}

function replaceExactlyOne(lines, prefix, replacement) {
  const indexes = [];
  for (let index = 0; index < lines.length; index += 1) {
    if (lines[index].startsWith(prefix)) indexes.push(index);
  }
  if (indexes.length !== 1) fail(`expected one ${prefix} line, found ${indexes.length}`);
  lines[indexes[0]] = replacement;
}

function scheduledHeader(lines, name, startUtc, finishUtc) {
  const result = [...lines];
  replaceExactlyOne(result, "EVENT_NAME,", `EVENT_NAME, "${name}"`);
  replaceExactlyOne(result, "EVENT_VERSION,", "EVENT_VERSION, \"fleet-soak-v1\"");
  replaceExactlyOne(result, "EVENT_START_DATE_TIME,", `EVENT_START_DATE_TIME, ${startUtc}`);
  replaceExactlyOne(result, "EVENT_FINISH_DATE_TIME,", `EVENT_FINISH_DATE_TIME, ${finishUtc}`);
  return result;
}

function renamedType(lines, fromType, toType, roleName, frequency) {
  const result = lines.map((line) => line.replace(new RegExp(`^TYPE${fromType}_`), `TYPE${toType}_`));
  replaceExactlyOne(result, `TYPE${toType}_ROLE_NAME,`, `TYPE${toType}_ROLE_NAME, "${roleName}"`);
  replaceExactlyOne(result, `TYPE${toType}_FREQ,`, `TYPE${toType}_FREQ, ${frequency}`);
  return result;
}

function roleBlocks(lines, expectedRoleCount) {
  const endIndex = lines.indexOf("EVENT_END");
  const starts = [];
  for (let role = 1; role <= expectedRoleCount; role += 1) {
    starts.push(lines.findIndex((line) => line.startsWith(`TYPE${role}_TX_COUNT,`)));
  }
  if (endIndex < 0 || starts.some((index) => index < 0) ||
      starts.some((index, position) => position > 0 && index <= starts[position - 1])) {
    fail("event template role blocks are not in the expected order");
  }
  return starts.map((start, index) => lines.slice(start, starts[index + 1] ?? endIndex));
}

function sprintEvent(template, name, startUtc, finishUtc) {
  const scheduled = scheduledHeader(template, name, startUtc, finishUtc);
  const blocks = roleBlocks(scheduled, 4);
  const header = scheduled.slice(0, scheduled.findIndex((line) => line.startsWith("TYPE1_TX_COUNT,")));
  // Normalize every generated event to the assignment order used by target .me files.
  return [
    ...header,
    ...renamedType(blocks[1], 2, 1, "Slow", 3520000),
    ...renamedType(blocks[0], 1, 2, "Fast", 3560000),
    ...renamedType(blocks[2], 3, 3, "Spectator", 3540000),
    ...renamedType(blocks[3], 4, 4, "Beacon", 3600000),
    "EVENT_END",
  ];
}

function classicEvent(template, name, startUtc, finishUtc) {
  const scheduled = scheduledHeader(template, name, startUtc, finishUtc);
  const eventTypeCountIndex = scheduled.findIndex((line) => line.startsWith("EVENT_NUMBER_OF_TX_TYPES,"));
  const blocks = roleBlocks(scheduled, 2);
  const type1Index = scheduled.findIndex((line) => line.startsWith("TYPE1_TX_COUNT,"));
  if (!(eventTypeCountIndex >= 0 && type1Index > eventTypeCountIndex)) fail("Classic template header is invalid");
  const header = scheduled.slice(0, type1Index);
  replaceExactlyOne(header, "EVENT_NUMBER_OF_TX_TYPES,", "EVENT_NUMBER_OF_TX_TYPES, 4");
  const foxRole = blocks[0];
  const finishRole = blocks[1];
  // Expand mature Classic role blocks into the same four-role monitoring layout as Sprint.
  const spectatorRole = renamedType(finishRole, 2, 3, "Spectator", 3540000);
  replaceExactlyOne(spectatorRole, "TYPE3_TX1_PATTERN,", 'TYPE3_TX1_PATTERN, "S "');
  return [
    ...header,
    ...renamedType(foxRole, 1, 1, "Classic Fox", 3520000),
    ...renamedType(foxRole, 1, 2, "Alternate Classic Fox", 3560000),
    ...spectatorRole,
    ...renamedType(finishRole, 2, 4, "Beacon", 3600000),
    "EVENT_END",
  ];
}

function transferLengthChecksum(lines) {
  return lines.reduce((sum, line) => sum + line.trim().length, 0);
}

function eventArtifact(filename, lines) {
  if (filename.length > maximumEventFilenameLength) fail(`event filename exceeds LittleFS limit: ${filename}`);
  if (lines.length > maximumEventLines) fail(`event exceeds ${maximumEventLines} lines: ${filename}`);
  const contents = `${lines.join("\n")}\n`;
  return {
    contents,
    filename,
    bytes: Buffer.byteLength(contents),
    sha256: createHash("sha256").update(contents).digest("hex"),
    transferLengthChecksum: transferLengthChecksum(lines),
  };
}

const options = parseArguments(process.argv.slice(2));
const startEpoch = strictUtcEpoch(options.start);
if (!options.allowShortLead && startEpoch < Date.now() + 30 * 60 * 1000) {
  fail("start must be at least 30 minutes in the future to allow upload, cloning, and assignment checks");
}

const compactStart = options.start.replace(/[-:]/g, "").replace("T", "-");
const outputRoot = resolve(options.output || join(repoRoot, "Software", "Huzzah", "tmp", "fleet-soak-events", compactStart));
if (existsSync(outputRoot)) fail(`output directory already exists: ${outputRoot}`);

const sprintTemplate = sourceLines(sprintTemplatePath);
const classicTemplate = sourceLines(classicTemplatePath);
const schedule = [];
const eventArtifacts = [];
let cursor = startEpoch;

for (let index = 0; index < eventCount; index += 1) {
  const sequence = index + 1;
  const kind = index % 2 === 0 ? "Sprint" : "Classic";
  const durationMinutes = durationsMinutes[index % durationsMinutes.length];
  const startUtc = formatUtc(cursor);
  const finishEpoch = cursor + durationMinutes * 60 * 1000;
  const finishUtc = formatUtc(finishEpoch);
  const filename = `FS${String(sequence).padStart(2, "0")}-${kind}.event`;
  const name = `Fleet Soak ${String(sequence).padStart(2, "0")} ${kind} ${durationMinutes}m`;
  const lines = kind === "Sprint"
    ? sprintEvent(sprintTemplate, name, startUtc, finishUtc)
    : classicEvent(classicTemplate, name, startUtc, finishUtc);
  const artifact = eventArtifact(filename, lines);
  const { contents, ...file } = artifact;
  eventArtifacts.push({ filename, contents });
  const gapAfterMinutes = index + 1 < eventCount ? gapsMinutes[index % gapsMinutes.length] : 0;
  schedule.push({
    sequence,
    kind,
    name,
    startUtc,
    finishUtc,
    durationMinutes,
    gapAfterMinutes,
    ...file,
  });
  cursor = finishEpoch + gapAfterMinutes * 60 * 1000;
}

const totalMinutes = Math.round((Date.parse(schedule.at(-1).finishUtc) - startEpoch) / 60000);
const manifest = {
  format: "flexfox-fleet-soak-events-v1",
  scheduleStartUtc: options.start,
  scheduleFinishUtc: schedule.at(-1).finishUtc,
  totalMinutes,
  eventCount,
  sprintEventCount: schedule.filter((event) => event.kind === "Sprint").length,
  classicEventCount: schedule.filter((event) => event.kind === "Classic").length,
  assignmentPlan: {
    units1Through5: "0:0 through 0:4",
    units6Through10: "1:0 through 1:4",
    spectator: "2:0",
    beacon: "3:0",
    roleFrequenciesHz: [3520000, 3560000, 3540000, 3600000],
    roleOrder: ["Slow or Classic Fox", "Fast or Alternate Classic Fox", "Spectator", "Beacon"],
  },
  events: schedule,
};
mkdirSync(outputRoot, { recursive: true });
for (const artifact of eventArtifacts) {
  writeFileSync(join(outputRoot, artifact.filename), artifact.contents);
}
writeFileSync(join(outputRoot, "fleet-soak-schedule.json"), `${JSON.stringify(manifest, null, 2)}\n`);
writeFileSync(
  join(outputRoot, "README.txt"),
  [
    "FlexFox80 fleet soak event bundle",
    `UTC schedule: ${options.start} through ${schedule.at(-1).finishUtc}`,
    `Duration: ${totalMinutes} minutes (${Math.floor(totalMinutes / 60)}h ${totalMinutes % 60}m)`,
    "Upload only the 12 .event files to the master unit.",
    "Use the same assignment in every file: 0:0-0:4 at 3.52 MHz, 1:0-1:4 at 3.56 MHz, 2:0 spectator at 3.54 MHz, or 3:0 beacon at 3.60 MHz.",
    "Verify the complete event sheet and every target assignment before starting the soak.",
    "",
  ].join("\n"),
);

process.stdout.write(`PASS: generated ${eventCount} events in ${outputRoot}\n`);
process.stdout.write(`Schedule: ${options.start} through ${schedule.at(-1).finishUtc} (${totalMinutes} minutes)\n`);
process.stdout.write("Assignments: role 0 is 3.52 MHz, role 1 is 3.56 MHz, role 2:0 is spectator, and role 3:0 is beacon.\n");
