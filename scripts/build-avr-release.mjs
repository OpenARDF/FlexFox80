#!/usr/bin/env node

import { createHash } from "node:crypto";
import { existsSync, mkdirSync, readFileSync, rmSync, writeFileSync } from "node:fs";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { spawnSync } from "node:child_process";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const projectRoot = join(repoRoot, "Software", "AVR128DA48", "FlexFox80");
const outputRoot = join(repoRoot, "Software", "AVR128DA48", "tmp", "avr-release");
const expectedCompilerVersion = "7.3.0";
const expectedDfpVersion = "1.9.103";
const allowVersionMismatch = process.env.AVR_ALLOW_VERSION_MISMATCH === "1";

function fail(message) {
  process.stderr.write(`AVR Release build: ${message}\n`);
  process.exit(2);
}

function requiredRoot(name) {
  const value = process.env[name];
  if (!value) {
    fail(`${name} is not set; see Docs/Software/BUILD_ENVIRONMENT.md`);
  }
  const root = resolve(value);
  if (!existsSync(root)) {
    fail(`${name} does not exist: ${root}`);
  }
  return root;
}

const toolchainRoot = requiredRoot("AVR_TOOLCHAIN_ROOT");
const dfpRoot = requiredRoot("AVR_DFP_ROOT");
const executableSuffix = process.platform === "win32" ? ".exe" : "";

function tool(name) {
  const candidate = join(toolchainRoot, "bin", `${name}${executableSuffix}`);
  if (!existsSync(candidate)) {
    fail(`required tool is missing: ${candidate}`);
  }
  return candidate;
}

const compiler = tool("avr-g++");
const objcopy = tool("avr-objcopy");
const objdump = tool("avr-objdump");
const size = tool("avr-size");
const dfpInclude = join(dfpRoot, "include");
const dfpDevice = join(dfpRoot, "gcc", "dev", "avr128da48");

for (const requiredPath of [dfpInclude, dfpDevice]) {
  if (!existsSync(requiredPath)) {
    fail(`AVR-Dx_DFP content is missing: ${requiredPath}`);
  }
}

const warnings = [];

function run(command, args, options = {}) {
  process.stdout.write(`+ ${basename(command)} ${args.join(" ")}\n`);
  const result = spawnSync(command, args, {
    cwd: projectRoot,
    encoding: "utf8",
    maxBuffer: 64 * 1024 * 1024,
    ...options,
  });

  if (result.error) {
    fail(`${basename(command)} could not run: ${result.error.message}`);
  }
  if (result.stdout && !options.quiet) process.stdout.write(result.stdout);
  if (result.stderr) {
    process.stderr.write(result.stderr);
    warnings.push(...result.stderr.split(/\r?\n/).filter((line) => /warning:/i.test(line)));
  }
  if (result.status !== 0 && !options.allowFailure) {
    fail(`${basename(command)} exited with status ${result.status}`);
  }
  return result;
}

const compilerVersion = run(compiler, ["-dumpversion"], { quiet: true }).stdout.trim();
const dfpVersion = basename(dfpRoot);
const versionProblems = [];
if (compilerVersion !== expectedCompilerVersion) {
  versionProblems.push(`compiler ${compilerVersion || "unknown"} (expected ${expectedCompilerVersion})`);
}
if (dfpVersion !== expectedDfpVersion) {
  versionProblems.push(`device pack ${dfpVersion} (expected ${expectedDfpVersion})`);
}
if (versionProblems.length && !allowVersionMismatch) {
  fail(`${versionProblems.join("; ")}. Set AVR_ALLOW_VERSION_MISMATCH=1 only for a non-baseline exploratory build.`);
}

rmSync(outputRoot, { recursive: true, force: true });
mkdirSync(outputRoot, { recursive: true });

const cppSources = [
  "atmel_start.cpp",
  "driver_isr.cpp",
  "main.cpp",
  "src/adc.cpp",
  "src/binio.cpp",
  "src/bod.cpp",
  "src/CircularStringBuff.cpp",
  "src/clkctrl.cpp",
  "src/cpuint.cpp",
  "src/dac0.cpp",
  "src/driver_init.cpp",
  "src/ds3231.cpp",
  "src/eeprommanager.cpp",
  "src/Goertzel.cpp",
  "src/huzzah.cpp",
  "src/i2c.cpp",
  "src/leds.cpp",
  "src/linkbus.cpp",
  "src/morse.cpp",
  "src/serialbus.cpp",
  "src/si5351.cpp",
  "src/slpctrl.cpp",
  "src/tcb.cpp",
  "src/transmitter.cpp",
  "src/usart_basic.cpp",
  "src/util.cpp",
];
const assemblySources = ["src/protected_io.S"];
const includeDirectories = ["Config", "include", "utils", "utils/assembler", ".", dfpInclude];
const deviceFlags = ["-mmcu=avr128da48", "-B", dfpDevice];
const objectFiles = [];

for (const source of cppSources) {
  const object = join(outputRoot, source.replace(/\.cpp$/, ".o"));
  mkdirSync(dirname(object), { recursive: true });
  objectFiles.push(object);
  run(compiler, [
    "-funsigned-char",
    "-funsigned-bitfields",
    "-DNDEBUG",
    ...includeDirectories.flatMap((directory) => ["-I", directory]),
    "-Os",
    "-fpack-struct",
    "-fshort-enums",
    "-ffunction-sections",
    "-fdata-sections",
    "-Wall",
    ...deviceFlags,
    "-c",
    "-fno-threadsafe-statics",
    "-o",
    object,
    source,
  ]);
}

for (const source of assemblySources) {
  const object = join(outputRoot, source.replace(/\.S$/, ".o"));
  mkdirSync(dirname(object), { recursive: true });
  objectFiles.push(object);
  run(compiler, [
    "-x",
    "assembler-with-cpp",
    "-c",
    ...deviceFlags,
    "-I",
    "utils",
    "-I",
    dfpInclude,
    "-o",
    object,
    source,
  ]);
}

const elf = join(outputRoot, "FlexFox80.elf");
const map = join(outputRoot, "FlexFox80.map");
const hex = join(outputRoot, "FlexFox80.hex");
const eep = join(outputRoot, "FlexFox80.eep");
const lss = join(outputRoot, "FlexFox80.lss");
const srec = join(outputRoot, "FlexFox80.srec");

run(compiler, [
  "-o",
  elf,
  ...objectFiles,
  `-Wl,-Map=${map}`,
  "-Wl,--start-group",
  "-Wl,-lm",
  "-Wl,--end-group",
  "-Wl,--gc-sections",
  ...deviceFlags,
]);
run(objcopy, ["-O", "ihex", "-R", ".eeprom", "-R", ".fuse", "-R", ".lock", "-R", ".signature", "-R", ".user_signatures", elf, hex]);
run(objcopy, ["-j", ".eeprom", "--set-section-flags=.eeprom=alloc,load", "--change-section-lma", ".eeprom=0", "--no-change-warnings", "-O", "ihex", elf, eep], { allowFailure: true });
const listing = run(objdump, ["-h", "-S", elf], { quiet: true });
writeFileSync(lss, listing.stdout);
run(objcopy, ["-O", "srec", "-R", ".eeprom", "-R", ".fuse", "-R", ".lock", "-R", ".signature", "-R", ".user_signatures", elf, srec]);
const sizeResult = run(size, [elf]);

function sha256(path) {
  return createHash("sha256").update(readFileSync(path)).digest("hex");
}

const artifacts = [elf, hex, eep, map, lss, srec]
  .filter((path) => existsSync(path))
  .map((path) => ({ file: basename(path), sha256: sha256(path) }));
const evidence = {
  status: versionProblems.length ? "exploratory-version-mismatch" : "reference-version-match",
  compilerVersion,
  expectedCompilerVersion,
  dfpVersion,
  expectedDfpVersion,
  sizeOutput: sizeResult.stdout.trim(),
  warningCount: warnings.length,
  warnings,
  artifacts,
};
writeFileSync(join(outputRoot, "build-evidence.json"), `${JSON.stringify(evidence, null, 2)}\n`);

process.stdout.write(`AVR Release build complete: ${outputRoot}\n`);
process.stdout.write(`Warnings: ${warnings.length}\n`);
for (const artifact of artifacts) {
  process.stdout.write(`${artifact.sha256}  ${artifact.file}\n`);
}
