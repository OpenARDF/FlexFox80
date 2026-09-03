#!/usr/bin/env node

import { createHash } from "node:crypto";
import {
  copyFileSync,
  cpSync,
  existsSync,
  mkdirSync,
  readFileSync,
  readdirSync,
  rmSync,
  statSync,
  writeFileSync,
} from "node:fs";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { spawnSync } from "node:child_process";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const sketchRoot = join(repoRoot, "Software", "Huzzah", "ARDF_Transmitter");
const firmwareVersion = readFileSync(join(sketchRoot, "esp8266.h"), "utf8").match(
  /#define\s+WIFI_SW_VERSION\s+\("([^"]+)"\)/,
)?.[1];
if (!firmwareVersion) fail("could not read WIFI_SW_VERSION from the ESP firmware source");
const sharedAvrInclude = join(repoRoot, "Software", "AVR128DA48", "FlexFox80", "include");
const tempRoot = join(repoRoot, "Software", "Huzzah", "tmp");
const recoveryQualification = process.env.FLEXFOX_ESP_FILESYSTEM_RECOVERY_QUALIFICATION === "1";
const sketchOnlyDiagnostic = process.env.FLEXFOX_ESP_LITTLEFS_2_5_1_DIAGNOSTIC === "1";
if (recoveryQualification && sketchOnlyDiagnostic) {
  throw new Error("filesystem recovery and LittleFS diagnostic build modes are mutually exclusive");
}
const arduinoRoot = resolve(process.env.ESP_ARDUINO_DATA_ROOT || join(tempRoot, "arduino"));
const outputRoot = join(
  tempRoot,
  recoveryQualification
    ? "esp-build-filesystem-recovery"
    : sketchOnlyDiagnostic
      ? "esp-build-littlefs-2.5.1-diagnostic"
      : "esp-build",
);
const configPath = join(arduinoRoot, "arduino-cli.yaml");
const coreVersion = "2.7.4";
const webSocketsVersion = "2.3.6";
const littleFsToolVersion = "2.5.0-4-fe5bb56";
const littleFsSourceCoreVersion = "3.1.2";
const littleFsSourceArchiveName = `esp8266-${littleFsSourceCoreVersion}.zip`;
const littleFsSourceArchiveUrl = `https://github.com/esp8266/Arduino/releases/download/${littleFsSourceCoreVersion}/${littleFsSourceArchiveName}`;
const littleFsSourceArchiveSha256 = "b3f47686d7783c120c2f10bf82788f921c53db8642cc87012599abb6e335b182";
const packageUrl = "https://arduino.esp8266.com/stable/package_esp8266com_index.json";
const fqbn = "esp8266:esp8266:huzzah:baud=115200,xtal=80,eesz=4M1M,dbg=Disabled,lvl=None____,ip=lm2f,wipe=none";
const filesystemSize = 0x0fa000;

function fail(message) {
  process.stderr.write(`ESP8266 build: ${message}\n`);
  process.exit(2);
}

function findArduinoCli() {
  const candidates = [
    process.env.ARDUINO_CLI,
    "arduino-cli",
    "/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli",
  ].filter(Boolean);

  for (const candidate of candidates) {
    const probe = spawnSync(candidate, ["version"], { encoding: "utf8" });
    if (!probe.error && probe.status === 0) return candidate;
  }

  fail("arduino-cli was not found; install it, install Arduino IDE, or set ARDUINO_CLI");
}

const arduinoCli = findArduinoCli();
const warnings = [];

function run(command, args, options = {}) {
  process.stdout.write(`+ ${basename(command)} ${args.join(" ")}\n`);
  const result = spawnSync(command, args, {
    cwd: repoRoot,
    encoding: "utf8",
    maxBuffer: 64 * 1024 * 1024,
    ...options,
  });

  if (result.error) fail(`${basename(command)} could not run: ${result.error.message}`);
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

mkdirSync(arduinoRoot, { recursive: true });
const dataRoot = join(arduinoRoot, "data");
const downloadsRoot = join(arduinoRoot, "downloads");
const userRoot = join(arduinoRoot, "user");
for (const directory of [dataRoot, downloadsRoot, userRoot]) mkdirSync(directory, { recursive: true });

async function downloadFile(url, path) {
  const response = await fetch(url, { redirect: "follow" });
  if (!response.ok) fail(`download failed with HTTP ${response.status}: ${url}`);
  writeFileSync(path, Buffer.from(await response.arrayBuffer()));
}

async function prepareQualifiedLittleFsLibrary() {
  const archivePath = resolve(
    process.env.FLEXFOX_ESP_LITTLEFS_SOURCE_ARCHIVE ||
      process.env.FLEXFOX_ESP_DIAGNOSTIC_CORE_ARCHIVE ||
      join(downloadsRoot, littleFsSourceArchiveName),
  );
  if (!existsSync(archivePath)) {
    process.stdout.write(`Downloading ${littleFsSourceArchiveUrl}\n`);
    await downloadFile(littleFsSourceArchiveUrl, archivePath);
  }
  const archiveSha256 = createHash("sha256").update(readFileSync(archivePath)).digest("hex");
  if (archiveSha256 !== littleFsSourceArchiveSha256) {
    fail(`LittleFS source archive SHA-256 mismatch: ${archiveSha256}`);
  }

  const extractRoot = join(tempRoot, `esp8266-${littleFsSourceCoreVersion}-source`);
  const libraryRoot = join(tempRoot, "LittleFS-2.5.1-qualified");
  rmSync(extractRoot, { recursive: true, force: true });
  rmSync(libraryRoot, { recursive: true, force: true });
  mkdirSync(extractRoot, { recursive: true });
  run("unzip", ["-q", archivePath, "-d", extractRoot]);

  const installedLibraryRoot = join(
    dataRoot,
    "packages",
    "esp8266",
    "hardware",
    "esp8266",
    coreVersion,
    "libraries",
    "LittleFS",
  );
  if (!existsSync(installedLibraryRoot)) {
    fail(`ESP8266 core ${coreVersion} LittleFS wrapper is missing: ${installedLibraryRoot}`);
  }
  cpSync(installedLibraryRoot, libraryRoot, { recursive: true });

  const newerEngineRoot = join(
    extractRoot,
    `esp8266-${littleFsSourceCoreVersion}`,
    "libraries",
    "LittleFS",
    "lib",
    "littlefs",
  );
  for (const file of ["lfs.c", "lfs.h", "lfs_util.c", "lfs_util.h"]) {
    const source = join(newerEngineRoot, file);
    if (!existsSync(source)) fail(`LittleFS 2.5.1 source is missing from archive: ${source}`);
    copyFileSync(source, join(libraryRoot, "lib", "littlefs", file));
  }
  const qualifiedHeader = readFileSync(join(libraryRoot, "lib", "littlefs", "lfs.h"), "utf8");
  const qualifiedVersion = qualifiedHeader.match(
    /#define\s+LFS_VERSION\s+0x([0-9a-fA-F]+)/,
  )?.[1];
  if (qualifiedVersion !== "00020005") {
    fail(`expected LittleFS API version 2.5, found 0x${qualifiedVersion || "unknown"}`);
  }
  const qualifiedDiskVersion = qualifiedHeader.match(
    /#define\s+LFS_DISK_VERSION\s+0x([0-9a-fA-F]+)/,
  )?.[1];
  if (qualifiedDiskVersion !== "00020000") {
    fail(`expected unchanged LittleFS disk version 2.0, found 0x${qualifiedDiskVersion || "unknown"}`);
  }
  return { archivePath, archiveSha256, libraryRoot };
}

writeFileSync(
  configPath,
  [
    "board_manager:",
    "  additional_urls:",
    `    - ${packageUrl}`,
    "directories:",
    `  data: ${JSON.stringify(dataRoot)}`,
    `  downloads: ${JSON.stringify(downloadsRoot)}`,
    `  user: ${JSON.stringify(userRoot)}`,
    "logging:",
    "  level: info",
    "",
  ].join("\n"),
);

function cli(args, options = {}) {
  return run(arduinoCli, [...args, "--config-file", configPath], options);
}

let coreList = cli(["core", "list"], { quiet: true, allowFailure: true });
if (coreList.status !== 0 || !new RegExp(`esp8266:esp8266\\s+${coreVersion.replaceAll(".", "\\.")}(?:\\s|$)`).test(coreList.stdout)) {
  cli(["core", "install", `esp8266:esp8266@${coreVersion}`]);
  coreList = cli(["core", "list"], { quiet: true });
}

let libraryList = cli(["lib", "list"], { quiet: true, allowFailure: true });
if (libraryList.status !== 0 || !new RegExp(`WebSockets\\s+${webSocketsVersion.replaceAll(".", "\\.")}(?:\\s|$)`).test(libraryList.stdout)) {
  cli(["lib", "install", `WebSockets@${webSocketsVersion}`]);
  libraryList = cli(["lib", "list"], { quiet: true });
}

const qualifiedLittleFs = await prepareQualifiedLittleFsLibrary();
rmSync(outputRoot, { recursive: true, force: true });
mkdirSync(outputRoot, { recursive: true });
const compileArgs = [
  "compile",
  "--fqbn",
  fqbn,
  "--build-path",
  join(outputRoot, "work"),
  "--build-property",
  `compiler.cpp.extra_flags=-I${sharedAvrInclude} ` +
    `-DFLEXFOX_FILESYSTEM_RECOVERY_QUALIFICATION=${recoveryQualification ? 1 : 0}`,
];
compileArgs.push("--library", qualifiedLittleFs.libraryRoot);
compileArgs.push("--output-dir", outputRoot, sketchRoot);
const compileOutput = cli(compileArgs);
const compileCommandsPath = join(outputRoot, "work", "compile_commands.json");
if (!existsSync(compileCommandsPath)) {
  fail("build did not emit compile_commands.json");
}
const qualifiedLibraryPrefix = `${qualifiedLittleFs.libraryRoot}/`;
const compiledLittleFsSources = JSON.parse(readFileSync(compileCommandsPath, "utf8"))
  .map(({ file }) => file)
  .filter((file) => /\/LittleFS\.cpp$|\/lfs(?:_util)?\.c$/.test(file));
if (
  compiledLittleFsSources.length !== 3 ||
  compiledLittleFsSources.some((file) => !file.startsWith(qualifiedLibraryPrefix))
) {
  fail(`build selected unexpected LittleFS sources: ${compiledLittleFsSources.join(", ")}`);
}

function findFile(root, name) {
  if (!existsSync(root)) return null;
  for (const entry of readdirSync(root)) {
    const path = join(root, entry);
    if (statSync(path).isDirectory()) {
      const nested = findFile(path, name);
      if (nested) return nested;
    } else if (entry === name) {
      return path;
    }
  }
  return null;
}

const littleFsExecutable = findFile(
  join(dataRoot, "packages", "esp8266", "tools", "mklittlefs", littleFsToolVersion),
  process.platform === "win32" ? "mklittlefs.exe" : "mklittlefs",
);
if (!littleFsExecutable) {
  fail(`pinned mklittlefs ${littleFsToolVersion} was not installed with ESP8266 core ${coreVersion}`);
}

if (!sketchOnlyDiagnostic) {
  const littleFsImage = join(outputRoot, "ARDF_Transmitter.littlefs.bin");
  run(littleFsExecutable, [
    "-c",
    join(sketchRoot, "data"),
    "-b",
    "8192",
    "-p",
    "256",
    "-s",
    String(filesystemSize),
    littleFsImage,
  ]);
}

function sha256(path) {
  return createHash("sha256").update(readFileSync(path)).digest("hex");
}

const artifactNames = [
  "ARDF_Transmitter.ino.bin",
  "ARDF_Transmitter.ino.elf",
  "ARDF_Transmitter.ino.map",
];
if (!sketchOnlyDiagnostic) artifactNames.push("ARDF_Transmitter.littlefs.bin");
const artifacts = artifactNames.map((file) => {
  const path = join(outputRoot, file);
  if (!existsSync(path)) fail(`expected build artifact is missing: ${path}`);
  return { file, bytes: statSync(path).size, sha256: sha256(path) };
});
const cliVersion = run(arduinoCli, ["version"], { quiet: true }).stdout.trim();
const evidence = {
  status: recoveryQualification
    ? "filesystem-recovery-qualification-build"
    : sketchOnlyDiagnostic
      ? "clone-littlefs-2.5.1-diagnostic-build"
      : "hardware-compatible-normal-build",
  recoveryQualification,
  sketchOnlyDiagnostic,
  firmwareVersion,
  filesystemImageGenerated: !sketchOnlyDiagnostic,
  cliVersion,
  core: `esp8266:esp8266@${coreVersion}`,
  library: `WebSockets@${webSocketsVersion}`,
  littleFsEngine: "littlefs@2.5.1",
  littleFsDiskVersion: "2.0",
  compiledLittleFsSources,
  littleFsSourceArchive: {
    file: basename(qualifiedLittleFs.archivePath),
    sha256: qualifiedLittleFs.archiveSha256,
    url: littleFsSourceArchiveUrl,
  },
  littleFsToolPackage: `mklittlefs@${littleFsToolVersion}`,
  fqbn,
  boardProfile: {
    board: "Adafruit Feather HUZZAH ESP8266",
    cpuFrequencyMHz: 80,
    uploadSpeed: 115200,
    flash: "4MB",
    filesystemBytes: filesystemSize,
    erase: "Only Sketch",
    debugPort: "Disabled",
    debugLevel: "None",
    lwip: "v2 Lower Memory",
  },
  compileOutput: `${compileOutput.stdout || ""}${compileOutput.stderr || ""}`.trim(),
  filesystemDeterminism: sketchOnlyDiagnostic
    ? "No filesystem image is generated by the sketch-only diagnostic build."
    : "Image hashes can vary across invocations while the checked-in data inputs remain unchanged.",
  warningCount: warnings.length,
  warnings,
  artifacts,
};
writeFileSync(join(outputRoot, "build-evidence.json"), `${JSON.stringify(evidence, null, 2)}\n`);

process.stdout.write(`ESP8266 build complete: ${outputRoot}\n`);
