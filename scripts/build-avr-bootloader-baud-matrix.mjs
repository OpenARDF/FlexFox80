#!/usr/bin/env node

import { createHash } from "node:crypto";
import { cpSync, mkdirSync, readFileSync, rmSync, writeFileSync } from "node:fs";
import { spawnSync } from "node:child_process";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const scriptsRoot = join(repoRoot, "scripts");
const tmpRoot = join(repoRoot, "Software", "AVR128DA48", "tmp");
const matrixRoot = join(tmpRoot, "bootloader-baud-matrix");
const bauds = [9600, 19200, 38400, 57600, 115200];

function fail(message) {
  process.stderr.write(`AVR bootloader baud matrix: ${message}\n`);
  process.exit(2);
}

function run(script, extraEnvironment = {}) {
  const result = spawnSync(process.execPath, [join(scriptsRoot, script)], {
    cwd: repoRoot,
    env: { ...process.env, ...extraEnvironment },
    encoding: "utf8",
    maxBuffer: 64 * 1024 * 1024,
  });
  if(result.stdout) process.stdout.write(result.stdout);
  if(result.stderr) process.stderr.write(result.stderr);
  if(result.error) fail(`${script} could not run: ${result.error.message}`);
  if(result.status !== 0) fail(`${script} exited with status ${result.status}`);
}

function sha256(path) {
  return createHash("sha256").update(readFileSync(path)).digest("hex");
}

rmSync(matrixRoot, { recursive: true, force: true });
mkdirSync(matrixRoot, { recursive: true });
const results = [];

for(const baud of bauds) {
  process.stdout.write(`\nBuilding protocol-2 boot chain at ${baud} baud...\n`);
  run("build-avr-bootloader.mjs", { FLEXFOX_BOOT_TEST_BAUD: String(baud) });
  run("package-avr-boot-chain.mjs");
  const destination = join(matrixRoot, String(baud));
  mkdirSync(destination, { recursive: true });
  cpSync(join(tmpRoot, "bootloader-release"), join(destination, "bootloader-release"), { recursive: true });
  cpSync(join(tmpRoot, "avr-boot-chain"), join(destination, "avr-boot-chain"), { recursive: true });
  const manifestPath = join(destination, "avr-boot-chain", "FlexFox80-AVR-Release-Info.json");
  const manifest = JSON.parse(readFileSync(manifestPath, "utf8"));
  if(manifest.bootloaderBaud !== baud) fail(`packaged ${manifest.bootloaderBaud} baud while building ${baud}`);
  results.push({
    baud,
    bootloaderSha256: sha256(join(destination, "bootloader-release", "FlexFox80Bootloader.bin")),
    initialInstallSha256: sha256(join(destination, "avr-boot-chain", manifest.initialInstallFile)),
    updateImageSha256: sha256(join(destination, "avr-boot-chain", manifest.updateFile)),
  });
}

/* Leave the normal build output on the release default so a matrix run cannot
 * accidentally turn a later fleet package into a qualification-only baud. */
run("build-avr-bootloader.mjs", { FLEXFOX_BOOT_TEST_BAUD: "38400" });
run("package-avr-boot-chain.mjs");
writeFileSync(join(matrixRoot, "matrix-evidence.json"), `${JSON.stringify({
  protocolVersion: 2,
  applicationStart: "0x4000",
  pageSize: 512,
  bauds: results,
}, null, 2)}\n`);

process.stdout.write(`\nAVR bootloader baud matrix complete: ${matrixRoot}\n`);
process.stdout.write(`${bauds.map((baud) => basename(String(baud))).join(", ")} baud; default artifacts restored to 38400 baud.\n`);
