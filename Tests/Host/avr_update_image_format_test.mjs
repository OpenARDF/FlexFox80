#!/usr/bin/env node

import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import {
  createFlexFoxAvrUpdateImage,
  FLEXFOX_AVR_APP_START,
  FLEXFOX_AVR_IMAGE_FORMAT_VERSION,
  FLEXFOX_AVR_PAGE_SIZE,
  FLEXFOX_AVR_TRAILER_MAGIC,
  inspectFlexFoxAvrUpdateImage,
} from "../../scripts/lib/flexfox-avr-update-image.mjs";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const payload = Buffer.alloc(3 * FLEXFOX_AVR_PAGE_SIZE, 0xff);
payload.set([0x0c, 0x94, 0x5b, 0x21], 0);
for(let index = 512; index < payload.length; index++) payload[index] = index & 0xff;
const image = createFlexFoxAvrUpdateImage(payload, "0.210");
const info = inspectFlexFoxAvrUpdateImage(image);

assert.equal(info.applicationVersion, "0.210");
assert.equal(info.applicationPayloadBytes, payload.length);
assert.equal(info.imageBytes, payload.length + FLEXFOX_AVR_PAGE_SIZE);
assert.equal(image.subarray(payload.length, payload.length + 8).toString("ascii"), FLEXFOX_AVR_TRAILER_MAGIC);
console.log("PASS protocol-2 image has a valid FlexFox product trailer");

for(const [name, offset] of [
  ["magic", payload.length],
  ["format", payload.length + 8],
  ["page-size", payload.length + 10],
  ["app-start", payload.length + 12],
  ["payload-size", payload.length + 16],
  ["payload-crc", payload.length + 20],
  ["payload-data", 700],
]) {
  const changed = Buffer.from(image);
  changed[offset] ^= 0x01;
  assert.throws(() => inspectFlexFoxAvrUpdateImage(changed), Error, `${name} mutation must be rejected`);
  console.log(`PASS ${name} mutation is rejected`);
}

assert.throws(() => createFlexFoxAvrUpdateImage(Buffer.alloc(513), "0.210"), /complete/);
assert.throws(() => createFlexFoxAvrUpdateImage(payload, ""), /version/);
console.log("PASS malformed payload geometry and version are rejected");

const contract = readFileSync(join(
  repoRoot,
  "Software", "AVR128DA48", "FlexFox80", "include", "avr_update_contract.h",
), "utf8");
assert.match(contract, new RegExp(`#define\\s+FLEXFOX_AVR_UPDATE_IMAGE_FORMAT_VERSION\\s+${FLEXFOX_AVR_IMAGE_FORMAT_VERSION}U`));
assert.match(contract, new RegExp(`#define\\s+FLEXFOX_AVR_UPDATE_TRAILER_MAGIC\\s+"${FLEXFOX_AVR_TRAILER_MAGIC}"`));
assert.match(contract, new RegExp(`#define\\s+FLEXFOX_AVR_APP_START_BYTES\\s+0x${FLEXFOX_AVR_APP_START.toString(16)}UL`, "i"));
assert.match(contract, new RegExp(`#define\\s+FLEXFOX_AVR_FLASH_PAGE_BYTES\\s+${FLEXFOX_AVR_PAGE_SIZE}U`));
console.log("PASS host packager and embedded update contract stay aligned");
