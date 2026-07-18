#!/usr/bin/env node

import { createHash } from "node:crypto";
import { existsSync, mkdirSync, readFileSync, rmSync, writeFileSync } from "node:fs";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { spawnSync } from "node:child_process";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const sourceRoot = join(repoRoot, "Software", "AVR128DA48", "bootloader");
const outputRoot = join(repoRoot, "Software", "AVR128DA48", "tmp", "bootloader-release");
const expectedCompilerVersion = "7.3.0";
const expectedDfpVersion = "1.9.103";

function fail(message) {
  process.stderr.write(`AVR bootloader build: ${message}\n`);
  process.exit(2);
}

function requiredRoot(name) {
  const value = process.env[name];
  if (!value) fail(`${name} is not set; see Docs/Software/BUILD_ENVIRONMENT.md`);
  const root = resolve(value);
  if (!existsSync(root)) fail(`${name} does not exist: ${root}`);
  return root;
}

const toolchainRoot = requiredRoot("AVR_TOOLCHAIN_ROOT");
const dfpRoot = requiredRoot("AVR_DFP_ROOT");
const suffix = process.platform === "win32" ? ".exe" : "";
const tool = (name) => {
  const path = join(toolchainRoot, "bin", `${name}${suffix}`);
  if (!existsSync(path)) fail(`required tool is missing: ${path}`);
  return path;
};
const compiler = tool("avr-g++");
const objcopy = tool("avr-objcopy");
const sizeTool = tool("avr-size");
const dfpInclude = join(dfpRoot, "include");
const dfpDevice = join(dfpRoot, "gcc", "dev", "avr128da48");

function run(command, args, quiet = false) {
  process.stdout.write(`+ ${basename(command)} ${args.join(" ")}\n`);
  const result = spawnSync(command, args, { cwd: sourceRoot, encoding: "utf8", maxBuffer: 64 * 1024 * 1024 });
  if (result.error) fail(`${basename(command)} could not run: ${result.error.message}`);
  if (!quiet && result.stdout) process.stdout.write(result.stdout);
  if (result.stderr) process.stderr.write(result.stderr);
  if (result.status !== 0) fail(`${basename(command)} exited with status ${result.status}`);
  return result.stdout || "";
}

const compilerVersion = run(compiler, ["-dumpversion"], true).trim();
const dfpVersion = basename(dfpRoot);
if (process.env.AVR_ALLOW_VERSION_MISMATCH !== "1" &&
    (compilerVersion !== expectedCompilerVersion || dfpVersion !== expectedDfpVersion)) {
  fail(`requires compiler ${expectedCompilerVersion} and DFP ${expectedDfpVersion}; found ${compilerVersion} and ${dfpVersion}`);
}

rmSync(outputRoot, { recursive: true, force: true });
mkdirSync(outputRoot, { recursive: true });
const object = join(outputRoot, "main.o");
const protectedIoObject = join(outputRoot, "protected_io.o");
const elf = join(outputRoot, "FlexFox80Bootloader.elf");
const hex = join(outputRoot, "FlexFox80Bootloader.hex");
const binary = join(outputRoot, "FlexFox80Bootloader.bin");
const map = join(outputRoot, "FlexFox80Bootloader.map");

const deviceFlags = ["-mmcu=avr128da48", "-B", dfpDevice];
run(compiler, [
  "-DF_CPU=24000000UL", "-Os", "-ffunction-sections", "-fdata-sections", "-fno-exceptions",
  "-fno-threadsafe-statics", "-Wall", "-Wextra", "-I", "include", "-I", "../FlexFox80/include", "-I", "../FlexFox80/utils", "-I", dfpInclude,
  ...deviceFlags, "-c", "-o", object, "src/main.cpp",
]);
run(compiler, [
  "-x", "assembler-with-cpp", "-c", ...deviceFlags,
  "-I", "../FlexFox80/utils", "-I", dfpInclude,
  "-o", protectedIoObject, "../FlexFox80/src/protected_io.S",
]);
run(compiler, [
  "-o", elf, object, protectedIoObject, `-Wl,-Map=${map}`, "-Wl,--gc-sections", "-Wl,--section-start=.text=0x0", ...deviceFlags,
]);
run(objcopy, ["-O", "ihex", "-R", ".eeprom", "-R", ".fuse", "-R", ".lock", "-R", ".signature", "-R", ".user_signatures", elf, hex]);
run(objcopy, ["-O", "binary", "-R", ".eeprom", "-R", ".fuse", "-R", ".lock", "-R", ".signature", "-R", ".user_signatures", elf, binary]);
const sizeOutput = run(sizeTool, [elf]).trim();
const bytes = readFileSync(binary);
if (bytes.length > 0x4000) fail(`bootloader is ${bytes.length} bytes and overlaps application start 0x4000`);

const artifacts = [elf, hex, binary, map].map((path) => ({
  file: basename(path),
  sha256: createHash("sha256").update(readFileSync(path)).digest("hex"),
}));
writeFileSync(join(outputRoot, "build-evidence.json"), `${JSON.stringify({
  compilerVersion, expectedCompilerVersion, dfpVersion, expectedDfpVersion,
  bootloaderBytes: bytes.length, maximumBootloaderBytes: 0x4000,
  applicationStart: 0x4000, bootSizeFuse: 0x20, codeSizeFuse: 0,
  sizeOutput, artifacts,
}, null, 2)}\n`);

process.stdout.write(`AVR bootloader build complete: ${outputRoot}\n`);
process.stdout.write(`Bootloader occupancy: ${bytes.length}/16384 bytes\n`);
