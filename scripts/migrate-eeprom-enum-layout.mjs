#!/usr/bin/env node

import { createHash } from "node:crypto";
import { existsSync, readFileSync, writeFileSync } from "node:fs";
import { resolve } from "node:path";
import { fileURLToPath } from "node:url";

const EEPROM_BYTES = 512;
const INITIALIZED_FLAG = 0x0108;
const widenedFields = new Set([
  "fox_setting_none",
  "fox_setting_classic",
  "fox_setting_sprint",
  "fox_setting_foxoring",
  "fox_setting_blind",
  "event_setting",
]);

const legacyFields = [
  ["eeprom_initialization_flag", 2],
  ["guard4_1", 4],
  ["event_start_epoch", 4],
  ["guard4_2", 4],
  ["event_finish_epoch", 4],
  ["guard4_3", 4],
  ["pattern_text", 22],
  ["guard4_4", 4],
  ["foxoring_pattern_text", 22],
  ["guard4_5", 4],
  ["stationID_text", 22],
  ["guard4_6", 4],
  ["unlockCode", 10],
  ["guard4_7", 4],
  ["fox_setting_none", 1],
  ["guard4_8", 4],
  ["fox_setting_classic", 1],
  ["guard4_9", 4],
  ["fox_setting_sprint", 1],
  ["guard4_10", 4],
  ["fox_setting_foxoring", 1],
  ["guard4_11", 4],
  ["fox_setting_blind", 1],
  ["guard4_12", 4],
  ["utc_offset", 1],
  ["guard4_13", 4],
  ["rtty_offset", 4],
  ["guard4_14", 4],
  ["rf_power", 2],
  ["guard4_15", 4],
  ["id_codespeed", 1],
  ["guard4_16", 4],
  ["pattern_codespeed", 1],
  ["guard4_17", 4],
  ["foxoring_pattern_codespeed", 1],
  ["guard4_18", 4],
  ["off_air_seconds", 2],
  ["guard4_19", 4],
  ["on_air_seconds", 2],
  ["guard4_20", 4],
  ["ID_period_seconds", 2],
  ["guard4_21", 4],
  ["intra_cycle_delay_time", 2],
  ["guard4_22", 4],
  ["event_setting", 1],
  ["guard4_23", 4],
  ["frequency", 4],
  ["guard4_24", 4],
  ["frequency_low", 4],
  ["guard4_25", 4],
  ["frequency_med", 4],
  ["guard4_26", 4],
  ["frequency_high", 4],
  ["guard4_27", 4],
  ["frequency_beacon", 4],
  ["guard4_28", 4],
  ["master_setting", 1],
  ["guard4_29", 4],
  ["voltage_threshold", 4],
  ["guard4_30", 4],
  ["clock_calibration", 2],
  ["guard4_31", 4],
  ["days_to_run", 1],
  ["guard4_32", 4],
  ["i2c_failure_count", 2],
];

function withOffsets(fields, widenEnums = false) {
  let offset = 0;
  return fields.map(([name, legacySize]) => {
    const size = widenEnums && widenedFields.has(name) ? 2 : legacySize;
    const field = { name, offset, size, legacySize };
    offset += size;
    return field;
  });
}

const legacyLayout = withOffsets(legacyFields);
const correctedLayout = withOffsets(legacyFields, true);
const legacySize = legacyLayout.at(-1).offset + legacyLayout.at(-1).size;
const correctedSize = correctedLayout.at(-1).offset + correctedLayout.at(-1).size;

if (legacySize !== 268 || correctedSize !== 274) {
  throw new Error(`internal EEPROM layout mismatch: legacy=${legacySize}, corrected=${correctedSize}`);
}

function field(layout, name) {
  const result = layout.find((candidate) => candidate.name === name);
  if (!result) throw new Error(`internal EEPROM field is missing: ${name}`);
  return result;
}

function unsigned(image, layout, name) {
  const { offset, size } = field(layout, name);
  if (size === 1) return image.readUInt8(offset);
  if (size === 2) return image.readUInt16LE(offset);
  if (size === 4) return image.readUInt32LE(offset);
  throw new Error(`unsupported integer width for ${name}: ${size}`);
}

function validateLegacy(image) {
  if (!Buffer.isBuffer(image) || image.length !== EEPROM_BYTES) {
    throw new Error(`EEPROM input must contain exactly ${EEPROM_BYTES} bytes`);
  }
  if (unsigned(image, legacyLayout, "eeprom_initialization_flag") !== INITIALIZED_FLAG) {
    throw new Error("legacy EEPROM initialization flag is not 0x0108");
  }

  for (const name of [
    "fox_setting_none",
    "fox_setting_classic",
    "fox_setting_sprint",
    "fox_setting_foxoring",
    "fox_setting_blind",
  ]) {
    const value = unsigned(image, legacyLayout, name);
    if (value > 31) throw new Error(`legacy ${name} value is implausible: ${value}`);
  }

  const event = unsigned(image, legacyLayout, "event_setting");
  if (event > 4) throw new Error(`legacy event value is implausible: ${event}`);

  for (const name of [
    "frequency",
    "frequency_low",
    "frequency_med",
    "frequency_high",
    "frequency_beacon",
  ]) {
    const value = unsigned(image, legacyLayout, name);
    if (value < 3_500_000 || value > 4_000_000) {
      throw new Error(`legacy ${name} is outside the FlexFox 80 m band: ${value}`);
    }
  }

  const master = unsigned(image, legacyLayout, "master_setting");
  if (master > 1) throw new Error(`legacy master setting is implausible: ${master}`);

  const voltageField = field(legacyLayout, "voltage_threshold");
  const voltage = image.readFloatLE(voltageField.offset);
  if (!Number.isFinite(voltage) || voltage < 0.1 || voltage > 15) {
    throw new Error(`legacy voltage threshold is implausible: ${voltage}`);
  }
}

export function migrateLegacy268To274(input) {
  validateLegacy(input);
  const output = Buffer.from(input);

  for (const legacyField of legacyLayout) {
    const correctedField = field(correctedLayout, legacyField.name);
    input.copy(
      output,
      correctedField.offset,
      legacyField.offset,
      legacyField.offset + legacyField.size,
    );
    if (widenedFields.has(legacyField.name)) {
      output[correctedField.offset + 1] = 0;
    }
  }

  return output;
}

function sha256(buffer) {
  return createHash("sha256").update(buffer).digest("hex");
}

function usage() {
  return (
    "Usage: node scripts/migrate-eeprom-enum-layout.mjs " +
    "--from legacy-268 --input <512-byte.bin> --output <new.bin>"
  );
}

function argument(name) {
  const index = process.argv.indexOf(name);
  if (index < 0 || index + 1 >= process.argv.length) return undefined;
  return process.argv[index + 1];
}

function main() {
  const sourceLayout = argument("--from");
  const inputArgument = argument("--input");
  const outputArgument = argument("--output");
  if (sourceLayout !== "legacy-268" || !inputArgument || !outputArgument) {
    throw new Error(usage());
  }

  const inputPath = resolve(inputArgument);
  const outputPath = resolve(outputArgument);
  if (inputPath === outputPath) throw new Error("refusing to overwrite the source EEPROM image");
  if (existsSync(outputPath)) throw new Error(`refusing to overwrite existing output: ${outputPath}`);

  const input = readFileSync(inputPath);
  const output = migrateLegacy268To274(input);
  writeFileSync(outputPath, output, { flag: "wx" });

  console.log(`Legacy schema bytes: ${legacySize}`);
  console.log(`Corrected schema bytes: ${correctedSize}`);
  console.log(`Input SHA-256: ${sha256(input)}`);
  console.log(`Output SHA-256: ${sha256(output)}`);
  console.log(`Wrote migrated EEPROM image: ${outputPath}`);
}

const invokedPath = process.argv[1] ? resolve(process.argv[1]) : undefined;
if (invokedPath === fileURLToPath(import.meta.url)) {
  try {
    main();
  } catch (error) {
    console.error(error.message);
    process.exitCode = 1;
  }
}
