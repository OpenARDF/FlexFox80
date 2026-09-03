#!/usr/bin/env node

import { existsSync, mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { spawnSync } from "node:child_process";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const oldRoot = join(
  repoRoot,
  "Software/Huzzah/tmp/arduino/data/packages/esp8266/hardware/esp8266/2.7.4/libraries/LittleFS/lib/littlefs",
);
const newRoot = join(repoRoot, "Software/Huzzah/tmp/LittleFS-2.5.1-qualified/lib/littlefs");
const source = join(repoRoot, "Tests/Host/littlefs_rename_characterization.c");
const outputRoot = join(repoRoot, "Software/Huzzah/tmp/esp-build-littlefs-2.5.1-diagnostic");
const compiler = process.env.CC || "cc";
const rounds = 3000;

function fail(message) {
  process.stderr.write(`LittleFS rename characterization: ${message}\n`);
  process.exit(2);
}

function run(command, args) {
  const result = spawnSync(command, args, {
    cwd: repoRoot,
    encoding: "utf8",
    maxBuffer: 16 * 1024 * 1024,
  });
  if (result.error) fail(`${command} could not run: ${result.error.message}`);
  if (result.status !== 0) {
    if (result.stdout) process.stdout.write(result.stdout);
    if (result.stderr) process.stderr.write(result.stderr);
    fail(`${command} exited with status ${result.status}`);
  }
  return result.stdout.trim();
}

for (const path of [source, join(oldRoot, "lfs.c"), join(newRoot, "lfs.c")]) {
  if (!existsSync(path)) {
    fail(`required source is missing: ${path}; run just esp-clone-littlefs-diagnostic-build first`);
  }
}
mkdirSync(outputRoot, { recursive: true });

function compile(label, root) {
  const executable = join(outputRoot, `littlefs-${label}-rename-characterization`);
  run(compiler, [
    "-O2",
    "-std=c11",
    "-Wall",
    "-Wextra",
    "-Werror",
    "-Wno-missing-field-initializers",
    "-pedantic",
    "-DLFS_NAME_MAX=32",
    "-DLFS_NO_DEBUG",
    "-I",
    root,
    source,
    join(root, "lfs.c"),
    join(root, "lfs_util.c"),
    "-o",
    executable,
  ]);
  return executable;
}

function characterize(version, executable) {
  const output = run(executable, [String(rounds)]);
  const match = output.match(
    /^rounds=(\d+) total_reads=(\d+) max_round=(\d+) max_reads=(\d+) max_progs=(\d+) max_erases=(\d+)$/,
  );
  if (!match) fail(`unexpected ${version} output: ${output}`);
  return {
    version,
    rounds: Number(match[1]),
    totalReads: Number(match[2]),
    maxRound: Number(match[3]),
    maxRenameReads: Number(match[4]),
    maxRenamePrograms: Number(match[5]),
    maxRenameErases: Number(match[6]),
  };
}

const oldExecutable = compile("2.2", oldRoot);
const newExecutable = compile("2.5.1", newRoot);
const oldResult = characterize("2.2", oldExecutable);
const newResult = characterize("2.5.1", newExecutable);
const compatibilityImage = join(outputRoot, "littlefs-cross-version-compatibility.bin");
if (run(oldExecutable, ["compat-create", compatibilityImage]) !== "compat-create=pass") {
  fail("LittleFS 2.2 did not create the compatibility image");
}
if (run(newExecutable, ["compat-replace", compatibilityImage]) !== "compat-replace=pass") {
  fail("LittleFS 2.5.1 did not replace the compatibility image target");
}
if (run(oldExecutable, ["compat-verify", compatibilityImage]) !== "compat-verify=pass") {
  fail("LittleFS 2.2 could not read the image after the 2.5.1 replacement");
}
const comparison = {
  geometry: {
    filesystemBytes: 1024000,
    blockBytes: 8192,
    blockCount: 125,
    ioBytes: 64,
    blockCycles: 16,
    initialRootFiles: 40,
    replacementFiles: 9,
    replacementBytes: 1200,
  },
  results: [oldResult, newResult],
  totalReadReduction: oldResult.totalReads / newResult.totalReads,
  worstRenameReadReduction: oldResult.maxRenameReads / newResult.maxRenameReads,
  crossVersionCompatibility: {
    createWith: "2.2",
    replaceWith: "2.5.1",
    verifyWith: "2.2",
    passed: true,
  },
};

if (
  comparison.totalReadReduction < 3 ||
  comparison.worstRenameReadReduction < 10
) {
  fail(`expected LittleFS 2.5.1 rename-read reduction was not observed: ${JSON.stringify(comparison)}`);
}

writeFileSync(
  join(outputRoot, "rename-characterization.json"),
  `${JSON.stringify(comparison, null, 2)}\n`,
);
process.stdout.write(
  `PASS LittleFS 2.5.1 reduced total rename reads ${comparison.totalReadReduction.toFixed(2)}x ` +
    `and worst-case rename reads ${comparison.worstRenameReadReduction.toFixed(2)}x; ` +
    "2.2 rollback readback passed\n",
);
