#!/usr/bin/env node

import assert from "node:assert/strict";

import { migrateLegacy268To274 } from "../../scripts/migrate-eeprom-enum-layout.mjs";

function legacyImage() {
  const image = Buffer.alloc(512, 0xff);

  image.writeUInt16LE(0x0108, 0);
  image.writeUInt32LE(1_783_770_660, 6);
  image.writeUInt32LE(1_783_867_860, 14);
  image.writeUInt8(0, 114);
  image.writeUInt8(1, 119);
  image.writeUInt8(7, 124);
  image.writeUInt8(17, 129);
  image.writeUInt8(1, 134);
  image.writeUInt8(0, 139);
  image.writeUInt32LE(170, 144);
  image.writeUInt16LE(3_000, 152);
  image.writeUInt8(20, 158);
  image.writeUInt8(8, 163);
  image.writeUInt8(8, 168);
  image.writeUInt16LE(0, 173);
  image.writeUInt16LE(60, 179);
  image.writeUInt16LE(300, 185);
  image.writeUInt16LE(0, 191);
  image.writeUInt8(3, 197);
  image.writeUInt32LE(3_600_000, 202);
  image.writeUInt32LE(3_530_000, 210);
  image.writeUInt32LE(3_550_000, 218);
  image.writeUInt32LE(3_570_000, 226);
  image.writeUInt32LE(3_999_999, 234);
  image.writeUInt8(0, 242);
  image.writeFloatLE(3.8, 247);
  image.writeUInt16LE(0x7fff, 255);
  image.writeUInt8(1, 261);
  image.writeUInt16LE(0, 266);

  // Values outside the historical 268-byte schema must survive untouched.
  for (let offset = 274; offset < image.length; offset += 1) {
    image[offset] = offset & 0xff;
  }

  return image;
}

const legacy = legacyImage();
const migrated = migrateLegacy268To274(legacy);

assert.equal(migrated.length, 512);
assert.deepEqual(migrated.subarray(0, 114), legacy.subarray(0, 114));

for (const [legacyOffset, correctedOffset] of [
  [114, 114],
  [119, 120],
  [124, 126],
  [129, 132],
  [134, 138],
  [197, 202],
]) {
  assert.equal(migrated[correctedOffset], legacy[legacyOffset]);
  assert.equal(migrated[correctedOffset + 1], 0);
}

assert.equal(migrated.readUInt32LE(208), 3_600_000);
assert.equal(migrated.readUInt32LE(216), 3_530_000);
assert.equal(migrated.readUInt32LE(224), 3_550_000);
assert.equal(migrated.readUInt32LE(232), 3_570_000);
assert.equal(migrated.readUInt32LE(240), 3_999_999);
assert.equal(migrated.readUInt8(248), 0);
assert.ok(Math.abs(migrated.readFloatLE(253) - 3.8) < 0.0001);
assert.equal(migrated.readUInt16LE(261), 0x7fff);
assert.equal(migrated.readUInt8(267), 1);
assert.equal(migrated.readUInt16LE(272), 0);
assert.deepEqual(migrated.subarray(274), legacy.subarray(274));

assert.throws(() => migrateLegacy268To274(Buffer.alloc(511)), /exactly 512 bytes/);

const invalidEvent = legacyImage();
invalidEvent.writeUInt8(99, 197);
assert.throws(() => migrateLegacy268To274(invalidEvent), /legacy event value/);

console.log("PASS legacy 268-byte EEPROM layout migrates to the corrected 274-byte layout");
console.log("PASS migration preserves the unused EEPROM tail");
console.log("PASS migration rejects wrong-sized and implausible input");
