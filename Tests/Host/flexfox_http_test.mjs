#!/usr/bin/env node

import assert from "node:assert/strict";
import {
  crc32,
  digest,
  multipartFileBody,
  normalizeFlexFoxUrl,
} from "../../scripts/lib/flexfox-http.mjs";

const check = Buffer.from("123456789", "ascii");
assert.equal(crc32(check), 0xcbf43926);
assert.equal(
  digest("sha256", check),
  "15e2b0d3c33891ebb0f1ef609ec419420c20e320ce94c65fbc8c3312448eb225",
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

console.log("PASS FlexFox HTTP deployment helpers frame and hash exact payloads");
