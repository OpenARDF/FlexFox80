#!/usr/bin/env node

import { createHash } from "node:crypto";
import { existsSync, mkdirSync, readFileSync, rmSync, writeFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import {
  createFlexFoxAvrUpdateImage,
  FLEXFOX_AVR_APP_START,
  FLEXFOX_AVR_FLASH_SIZE,
  FLEXFOX_AVR_IMAGE_FORMAT,
  FLEXFOX_AVR_IMAGE_FORMAT_VERSION,
  FLEXFOX_AVR_PAGE_SIZE,
  flexFoxAvrCrc32,
} from "./lib/flexfox-avr-update-image.mjs";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const tmpRoot = join(repoRoot, "Software", "AVR128DA48", "tmp");
const bootHex = join(tmpRoot, "bootloader-release", "FlexFox80Bootloader.hex");
const bootBuildEvidencePath = join(tmpRoot, "bootloader-release", "build-evidence.json");
const appHex = join(tmpRoot, "avr-release-relocated", "FlexFox80.hex");
const outputRoot = join(tmpRoot, "avr-boot-chain");
const appStart = FLEXFOX_AVR_APP_START;
const pageSize = FLEXFOX_AVR_PAGE_SIZE;
const flashSize = FLEXFOX_AVR_FLASH_SIZE;
const avrDefinitions = readFileSync(join(repoRoot, "Software", "AVR128DA48", "FlexFox80", "defs.h"), "utf8");
const espDefinitions = readFileSync(join(repoRoot, "Software", "Huzzah", "ARDF_Transmitter", "esp8266.h"), "utf8");
const bootloaderConfig = readFileSync(join(repoRoot, "Software", "AVR128DA48", "bootloader", "include", "bootloader_config.h"), "utf8");
const applicationVersion = avrDefinitions.match(/#define\s+SW_REVISION\s+"([^"]+)"/)?.[1];
const minimumEspVersion = espDefinitions.match(/#define\s+WIFI_SW_VERSION\s+\("([^"]+)"\)/)?.[1];
const bootloaderVersion = bootloaderConfig.match(/#define\s+FLEXFOX_BOOTLOADER_VERSION\s+"([^"]+)"/)?.[1];

function fail(message) {
  process.stderr.write(`AVR boot-chain package: ${message}\n`);
  process.exit(2);
}

function parseHex(path) {
  const memory = new Map();
  let base = 0;
  for (const [index, raw] of readFileSync(path, "utf8").split(/\r?\n/).entries()) {
    if (!raw) continue;
    if (!raw.startsWith(":")) fail(`${path}:${index + 1}: invalid Intel HEX record`);
    const data = Buffer.from(raw.slice(1), "hex");
    const length = data[0];
    if (data.length !== length + 5) fail(`${path}:${index + 1}: bad record length`);
    let sum = 0;
    for (const byte of data) sum = (sum + byte) & 0xff;
    if (sum !== 0) fail(`${path}:${index + 1}: bad checksum`);
    const offset = (data[1] << 8) | data[2];
    const type = data[3];
    if (type === 0) {
      for (let i = 0; i < length; i++) {
        const address = base + offset + i;
        const existing = memory.get(address);
        if (existing !== undefined && existing !== data[4 + i]) fail(`overlap differs at 0x${address.toString(16)}`);
        memory.set(address, data[4 + i]);
      }
    } else if (type === 4) {
      base = ((data[4] << 8) | data[5]) << 16;
    } else if (type === 1) {
      break;
    }
  }
  return memory;
}

function record(address, type, payload) {
  const bytes = [payload.length, (address >> 8) & 0xff, address & 0xff, type, ...payload];
  const sum = bytes.reduce((value, byte) => (value + byte) & 0xff, 0);
  bytes.push((-sum) & 0xff);
  return `:${Buffer.from(bytes).toString("hex").toUpperCase()}`;
}

function encodeHex(memory) {
  const addresses = [...memory.keys()].sort((a, b) => a - b);
  const lines = [];
  let upper = -1;
  for (let i = 0; i < addresses.length;) {
    const start = addresses[i];
    const nextUpper = start >>> 16;
    if (nextUpper !== upper) {
      upper = nextUpper;
      lines.push(record(0, 4, [(upper >> 8) & 0xff, upper & 0xff]));
    }
    const payload = [];
    let address = start;
    while (i < addresses.length && addresses[i] === address && (address >>> 16) === upper && payload.length < 16) {
      payload.push(memory.get(address));
      i++; address++;
    }
    lines.push(record(start & 0xffff, 0, payload));
  }
  lines.push(record(0, 1, []));
  return `${lines.join("\n")}\n`;
}

for (const path of [bootHex, bootBuildEvidencePath, appHex]) {
  if (!existsSync(path)) fail(`missing ${path}; run just avr-boot-chain-build`);
}
if (!applicationVersion || !minimumEspVersion || !bootloaderVersion) fail("unable to read release versions from firmware sources");
const bootBuildEvidence = JSON.parse(readFileSync(bootBuildEvidencePath, "utf8"));
if (![9600, 19200, 38400, 57600, 115200].includes(bootBuildEvidence.bootloaderBaud) ||
    bootBuildEvidence.applicationStart !== appStart) {
  fail("bootloader build evidence has unsupported UART or application geometry");
}
const boot = parseHex(bootHex);
const app = parseHex(appHex);
if ([...boot.keys()].some((address) => address >= appStart)) fail("bootloader overlaps application section");
if ([...app.keys()].some((address) => address < appStart)) fail("application is not relocated to 0x4000");
if ([...app.keys()].some((address) => address >= flashSize)) fail("application exceeds AVR flash");

const combined = new Map(boot);
for (const [address, value] of app) {
  if (combined.has(address)) fail(`bootloader/application overlap at 0x${address.toString(16)}`);
  combined.set(address, value);
}

const maximumAppAddress = Math.max(...app.keys());
const appLength = Math.ceil((maximumAppAddress - appStart + 1) / pageSize) * pageSize;
const appPayload = Buffer.alloc(appLength, 0xff);
for (const [address, value] of app) appPayload[address - appStart] = value;
const payloadCrc32 = flexFoxAvrCrc32(appPayload);
const appBinary = createFlexFoxAvrUpdateImage(appPayload, applicationVersion);
const trailer = appBinary.subarray(appPayload.length);
const crc32 = flexFoxAvrCrc32(appBinary);

for (let index = 0; index < trailer.length; index++) {
  combined.set(appStart + appPayload.length + index, trailer[index]);
}

rmSync(outputRoot, { recursive: true, force: true });
mkdirSync(outputRoot, { recursive: true });
const updateName = `FlexFox80-AVR-Update-${applicationVersion}.bin`;
const initialName = `FlexFox80-AVR-First-Install-${applicationVersion}.hex`;
writeFileSync(join(outputRoot, updateName), appBinary);
writeFileSync(join(outputRoot, initialName), encodeHex(combined));
const manifest = {
  format: FLEXFOX_AVR_IMAGE_FORMAT, product: "FlexFox80", applicationVersion,
  bootloaderVersion, protocolVersion: 2, bootloaderBaud: bootBuildEvidence.bootloaderBaud, target: "avr128da48",
  minimumEspVersion,
  applicationStart: appStart, pageSize, flashSize, imageFormatVersion: FLEXFOX_AVR_IMAGE_FORMAT_VERSION,
  applicationPayloadBytes: appPayload.length, applicationPayloadCrc32: `0x${payloadCrc32.toString(16).padStart(8, "0")}`,
  trailerBytes: trailer.length, imageBytes: appBinary.length,
  imageCrc32: `0x${crc32.toString(16).padStart(8, "0")}`,
  imageSha256: createHash("sha256").update(appBinary).digest("hex"),
  updateFile: updateName, initialInstallFile: initialName,
  requiredFuses: { codesize: "0x00", bootsize: "0x20" },
  safety: { resetVectorPageWrittenLast: true, ssidSuffixAuthorizationRequired: true },
};
writeFileSync(join(outputRoot, "FlexFox80-AVR-Release-Info.json"), `${JSON.stringify(manifest, null, 2)}\n`);
process.stdout.write(`AVR boot-chain package complete: ${outputRoot}\n`);
process.stdout.write(`${appBinary.length} update bytes, CRC32 ${manifest.imageCrc32}\n`);
