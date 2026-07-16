#!/usr/bin/env node

import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import vm from "node:vm";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const pagePath = join(
  repoRoot,
  "Software",
  "Huzzah",
  "ARDF_Transmitter",
  "FirmwareUpdatePage.h",
);
const source = readFileSync(pagePath, "utf8");
const pageMatch = source.match(
  /static const char FIRMWARE_UPDATE_PAGE_HTML\[\] PROGMEM = R"FIRMWAREPAGE\(([\s\S]*?)\)FIRMWAREPAGE";/,
);

assert.ok(pageMatch, "firmware update page must remain a testable flash-resident raw string");
const html = pageMatch[1];
assert.match(html, /accept="\.bin,application\/octet-stream"/);
assert.doesNotMatch(html, /filesystem/i, "update page must not offer filesystem images");
assert.match(html, /LittleFS are not modified/);

const scripts = [...html.matchAll(/<script(?:\s[^>]*)?>([\s\S]*?)<\/script>/gi)];
assert.equal(scripts.length, 1);
new vm.Script(scripts[0][1], { filename: `${pagePath}:FIRMWARE_UPDATE_PAGE_HTML` });

const firmwareBytes = new Uint8Array(4096);
firmwareBytes[0] = 0xe9;
for (let index = 1; index < firmwareBytes.length; index += 1) {
  firmwareBytes[index] = index & 0xff;
}

function crc32(bytes) {
  let crc = 0xffffffff;
  for (const byte of bytes) {
    crc ^= byte;
    for (let bit = 0; bit < 8; bit += 1) {
      crc = (crc >>> 1) ^ ((crc & 1) ? 0xedb88320 : 0);
    }
  }
  return (crc ^ 0xffffffff) >>> 0;
}

const listeners = new Map();
let submitted = 0;
const elements = {
  firmwareForm: {
    action: "",
    addEventListener(type, listener) {
      listeners.set(`form:${type}`, listener);
    },
    submit() {
      submitted += 1;
    },
  },
  firmwareFile: {
    files: [{
      name: "ARDF_Transmitter.ino.bin",
      arrayBuffer: async () => firmwareBytes.buffer,
    }],
  },
  updateButton: { disabled: false },
  status: { textContent: "" },
  message: { textContent: "" },
};

class FakeWebSocket {
  constructor(url) {
    this.url = url;
    this.readyState = 0;
  }
  close() {}
  send() {}
}

const context = vm.createContext({
  FileReader: class {},
  Uint8Array,
  WebSocket: FakeWebSocket,
  clearInterval() {},
  console: { log() {} },
  document: {
    getElementById(id) {
      return elements[id];
    },
  },
  fetch: async () => ({
    ok: true,
    json: async () => ({ version: "2.2", cloneActive: false }),
  }),
  location: { hostname: "flexfox.test" },
  setInterval() { return 1; },
  window: {
    addEventListener(type, listener) {
      listeners.set(`window:${type}`, listener);
    },
    confirm() { return true; },
  },
});

vm.runInContext(scripts[0][1], context, {
  filename: `${pagePath}:FIRMWARE_UPDATE_PAGE_HTML`,
});
await new Promise((resolvePromise) => setImmediate(resolvePromise));
assert.equal(elements.status.textContent, "Running WiFi firmware 2.2 - ready for a sketch-only update");

listeners.get("form:submit")({ preventDefault() {} });
await new Promise((resolvePromise) => setImmediate(resolvePromise));
await new Promise((resolvePromise) => setImmediate(resolvePromise));

const expectedCrc = crc32(firmwareBytes).toString(16).padStart(8, "0");
assert.equal(submitted, 1);
assert.equal(
  elements.firmwareForm.action,
  `/firmware?confirm=UPDATE&size=4096&crc32=${expectedCrc}`,
);
assert.equal(elements.message.textContent, "Uploading. Keep power connected...");
assert.ok(listeners.has("window:pagehide"));

console.log("PASS firmware update page validates size, image magic, and CRC32 before upload");
