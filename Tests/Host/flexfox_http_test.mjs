#!/usr/bin/env node

import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import {
  crc32,
  digest,
  esp8266SketchMd5Candidates,
  multipartFileBody,
  normalizeFlexFoxUrl,
} from "../../scripts/lib/flexfox-http.mjs";

const check = Buffer.from("123456789", "ascii");
assert.equal(crc32(check), 0xcbf43926);
assert.equal(
  digest("sha256", check),
  "15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225",
);

const sketch = Buffer.from([0xe9, 1, 0, 0x40, 0x5c, 0xf4, 0x10, 0x40]);
const originalSketch = Buffer.from(sketch);
const sketchMd5Candidates = esp8266SketchMd5Candidates(sketch);
assert.deepEqual(sketchMd5Candidates.map(({ flashMode }) => flashMode), [0, 1, 2, 3]);
for (const { flashMode, md5 } of sketchMd5Candidates) {
  const installedSketch = Buffer.from(sketch);
  installedSketch[2] = flashMode;
  assert.equal(md5, digest("md5", installedSketch));
}
assert.deepEqual(sketch, originalSketch, "candidate calculation must not modify the upload image");
assert.throws(
  () => esp8266SketchMd5Candidates(Buffer.from([0, 1, 2])),
  /uncompressed sketch image/,
);

const multipart = multipartFileBody("firmware", "firmware.bin", check);
assert.match(multipart.headers["Content-Type"], /^multipart\/form-data; boundary=/);
assert.equal(Number(multipart.headers["Content-Length"]), multipart.body.length);
assert.ok(multipart.body.includes(Buffer.from('name="firmware"; filename="firmware.bin"')));
assert.ok(multipart.body.includes(check));
assert.throws(() => multipartFileBody("name", "../events.html", check), /unsafe remote filename/);

assert.equal(normalizeFlexFoxUrl().href, "http://73.73.73.73/");
assert.equal(normalizeFlexFoxUrl("http://73.73.73.73/events.html?x=1").href, "http://73.73.73.73/");
assert.throws(() => normalizeFlexFoxUrl("https://73.73.73.73/"), /must use http/);

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const updater = readFileSync(join(repoRoot, "scripts", "update-flexfox-esp-over-wifi.mjs"), "utf8");
assert.match(updater, /FLEXFOX_EXPECTED_PREUPDATE_DEVICE_SSID/);
assert.match(updater, /FLEXFOX_RECONNECT_SSID/);
assert.match(updater, /beforeDeviceSsid !== expectedPreUpdateDeviceSsid/);
assert.match(updater, /after\.deviceSsid !== expectedDeviceSsid/);

console.log("PASS FlexFox HTTP deployment helpers frame uploads and verify normalized sketch hashes");
