#!/usr/bin/env node

import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const eepromManagerPath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "src",
  "eeprommanager.cpp",
);
const source = readFileSync(eepromManagerPath, "utf8");
const declaration = source.match(/extern\s+volatile\s+Fox_t\s+g_fox\s*\[\s*([^\]]+?)\s*\]\s*;/);

if (!declaration) {
  process.stderr.write("Firmware contract check failed: g_fox extern declaration was not found\n");
  process.exit(1);
}

const declaredExtent = declaration[1].replace(/\s+/g, "");
if (declaredExtent !== "EVENT_NUMBER_OF_EVENTS") {
  process.stderr.write(
    `Firmware contract check failed: g_fox extent is ${declaredExtent}; expected EVENT_NUMBER_OF_EVENTS\n`,
  );
  process.exit(1);
}

process.stdout.write("PASS g_fox declaration covers every Event_t value\n");
