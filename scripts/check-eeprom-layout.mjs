#!/usr/bin/env node

import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

process.on("uncaughtException", (error) => {
  process.stderr.write(`${error.message}\n`);
  process.exit(1);
});

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const firmwareRoot = join(repoRoot, "Software", "AVR128DA48", "FlexFox80");
const header = readFileSync(join(firmwareRoot, "include", "eeprommanager.h"), "utf8");
const definitions = readFileSync(join(firmwareRoot, "defs.h"), "utf8");

// AVR-GCC 7.3.0 target widths used by the deployed EEPROM schema. AVR data
// alignment is one byte; the checked-in reference map records ee_vars as 0x112
// bytes, matching these widths and the current declaration.
const typeWidths = new Map([
  ["bool", 1],
  ["char", 1],
  ["uint8_t", 1],
  ["int16_t", 2],
  ["uint16_t", 2],
  ["Event_t", 2],
  ["Fox_t", 2],
  ["float", 4],
  ["Frequency_Hz", 4],
  ["time_t", 4],
  ["uint32_t", 4],
]);

const constants = new Map([["GUARDSIZE", 4]]);
for (const name of [
  "MAX_PATTERN_TEXT_LENGTH",
  "MAX_UNLOCK_CODE_LENGTH",
  "UNLOCK_CODE_SIZE",
]) {
  const match = definitions.match(
    new RegExp(`#define\\s+${name}\\s+(?:\\(uint8_t\\)\\s*)?(\\d+)\\b`),
  );
  if (!match) {
    throw new Error(`EEPROM layout check failed: ${name} was not found`);
  }
  constants.set(name, Number(match[1]));
}

function stripComments(text) {
  return text.replace(/\/\*[\s\S]*?\*\//g, "").replace(/\/\/.*$/gm, "");
}

function resolveTerm(term, knownOffsets = new Map()) {
  const value = term.trim();
  const sizeOf = value.match(/^sizeof\s*\(\s*([A-Za-z_]\w*)\s*\)$/);
  if (sizeOf) {
    const width = typeWidths.get(sizeOf[1]);
    if (width === undefined) {
      throw new Error(`EEPROM layout check failed: unknown sizeof type ${sizeOf[1]}`);
    }
    return width;
  }
  if (/^\d+$/.test(value)) {
    return Number(value);
  }
  if (constants.has(value)) {
    return constants.get(value);
  }
  if (knownOffsets.has(value)) {
    return knownOffsets.get(value);
  }
  throw new Error(`EEPROM layout check failed: unknown expression term ${value}`);
}

function resolveSum(expression, knownOffsets = new Map()) {
  const terms = expression
    .split("+")
    .map((term) => term.trim())
    .filter(Boolean);
  return terms.reduce((sum, term) => sum + resolveTerm(term, knownOffsets), 0);
}

const structMatch = header.match(/struct\s+EE_prom\s*\{([\s\S]*?)\};/);
if (!structMatch) {
  throw new Error("EEPROM layout check failed: EE_prom declaration was not found");
}

const fields = stripComments(structMatch[1])
  .split(";")
  .map((entry) => entry.trim())
  .filter(Boolean)
  .map((entry) => {
    const match = entry.match(
      /^([A-Za-z_]\w*)\s+([A-Za-z_]\w*)(?:\s*\[\s*([^\]]+)\s*\])?$/,
    );
    if (!match) {
      throw new Error(`EEPROM layout check failed: unsupported field declaration ${entry}`);
    }
    const [, type, name, lengthExpression] = match;
    const width = typeWidths.get(type);
    if (width === undefined) {
      throw new Error(`EEPROM layout check failed: unknown field type ${type}`);
    }
    const length = lengthExpression ? resolveSum(lengthExpression) : 1;
    return { type, name, offset: 0, size: width * length };
  });

let layoutSize = 0;
for (const field of fields) {
  field.offset = layoutSize;
  layoutSize += field.size;
}

const enumMatch = header.match(/typedef\s+enum\s*\{([\s\S]*?)\}\s*EE_var_t\s*;/);
if (!enumMatch) {
  throw new Error("EEPROM layout check failed: EE_var_t declaration was not found");
}

const enumOffsets = new Map();
const enumExpressions = new Map();
const enumEntries = stripComments(enumMatch[1])
  .split(",")
  .map((entry) => entry.trim())
  .filter(Boolean);

for (const entry of enumEntries) {
  const match = entry.match(/^([A-Za-z_]\w*)\s*=\s*(.+)$/);
  if (!match) {
    throw new Error(`EEPROM layout check failed: unsupported enum entry ${entry}`);
  }
  enumExpressions.set(match[1], match[2].replace(/\s+/g, ""));
  enumOffsets.set(match[1], resolveSum(match[2], enumOffsets));
}

const enumNameExceptions = new Map([
  ["ID_period_seconds", "ID_Period_Seconds"],
  ["frequency_beacon", "Frequency_Beacon"],
  ["frequency_high", "Frequency_Hi"],
  ["frequency_low", "Frequency_Low"],
  ["frequency_med", "Frequency_Med"],
  ["foxoring_pattern_codespeed", "Foxoring_Pattern_Code_Speed"],
  ["i2c_failure_count", "I2C_failure_count"],
  ["intra_cycle_delay_time", "Intra_Cycle_Delay_Seconds"],
  ["off_air_seconds", "Off_Air_Seconds"],
  ["on_air_seconds", "On_Air_Seconds"],
  ["pattern_codespeed", "Pattern_Code_Speed"],
  ["rf_power", "RF_Power"],
  ["rtty_offset", "RTTY_offset"],
]);

const failures = [];
const fixedWidthEnumFields = [
  ["fox_setting_none", "Guard4_8", "Fox_setting_none"],
  ["fox_setting_classic", "Guard4_9", "Fox_setting_classic"],
  ["fox_setting_sprint", "Guard4_10", "Fox_setting_sprint"],
  ["fox_setting_foxoring", "Guard4_11", "Fox_setting_foxoring"],
  ["fox_setting_blind", "Guard4_12", "Fox_setting_blind"],
  ["event_setting", "Guard4_23", "Event_setting"],
];

for (const [fieldName, followingGuard, enumName] of fixedWidthEnumFields) {
  const field = fields.find((candidate) => candidate.name === fieldName);
  if (!field || field.type !== "uint16_t") {
    failures.push(`${fieldName} is not declared with the deployed uint16_t width`);
  }
  const expectedExpression = `${enumName}+sizeof(uint16_t)`;
  if (enumExpressions.get(followingGuard) !== expectedExpression) {
    failures.push(`${followingGuard} does not advance by the fixed uint16_t width`);
  }
}

if (fields.length !== enumEntries.length) {
  failures.push(
    `EE_prom has ${fields.length} fields but EE_var_t has ${enumEntries.length} offsets`,
  );
}

for (const field of fields) {
  const enumName =
    enumNameExceptions.get(field.name) ??
    `${field.name[0].toUpperCase()}${field.name.slice(1)}`;
  const enumOffset = enumOffsets.get(enumName);
  if (enumOffset === undefined) {
    failures.push(`${field.name} has no matching ${enumName} enum offset`);
  } else if (enumOffset !== field.offset) {
    failures.push(
      `${enumName} is ${enumOffset}; ${field.name} is at target offset ${field.offset}`,
    );
  }
}

if (layoutSize !== 0x112) {
  failures.push(`target layout size is ${layoutSize}; expected deployed size 274 (0x112)`);
}

if (failures.length > 0) {
  for (const failure of failures) {
    process.stderr.write(`EEPROM layout check failed: ${failure}\n`);
  }
  process.exit(1);
}

if (process.argv.includes("--show-offsets")) {
  for (const field of fields) {
    process.stdout.write(`${field.offset}\t${field.size}\t${field.name}\n`);
  }
}

process.stdout.write(
  `PASS all ${fields.length} EEPROM field offsets match the 274-byte AVR layout\n`,
);
