#!/usr/bin/env node

import assert from "node:assert/strict";
import { existsSync, readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "../..");
const optionalPagePath = join(repoRoot, "Software/Huzzah/optional/fleet-soak.html");
const baselinePagePath = join(repoRoot, "Software/Huzzah/ARDF_Transmitter/data/fleet-soak.html");
const source = readFileSync(optionalPagePath, "utf8");

assert.equal(existsSync(baselinePagePath), false, "Fleet Soak must remain absent from the baseline LittleFS image");
assert.match(source, /const reservedNames = Array\.from\(\{length: 12\}/);
assert.match(source, /FLEET_SOAK_MODE,\$\{mode\}/);
assert.match(source, /FLEET_SOAK_ASSIGN,\$\{ssid\},\$\{assignment\}/);
assert.match(source, /\/fleet-soak\/activate/);
assert.match(source, /\/fleet-soak\/cleanup/);
assert.doesNotMatch(source, /CLONE_PRUNE_EVENTS/);
assert.doesNotMatch(source, /\/delete(?:\.html)?/);

const script = source.match(/<script>([\s\S]*?)<\/script>/)?.[1];
assert.ok(script, "Fleet Soak page must contain an inline script");
new Function(script);

console.log("PASS optional Fleet Soak page boundaries and JavaScript syntax");
