#!/usr/bin/env node

import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { randomBytes } from "node:crypto";
import { crc32, fetchWithTimeout, normalizeFlexFoxUrl } from "./lib/flexfox-http.mjs";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const expectedSsid = process.env.FLEXFOX_EXPECTED_DEVICE_SSID;
const durationMs = Number.parseInt(process.env.FLEXFOX_WIFI_LEASE_TEST_DURATION_MS ?? "135000", 10);
const baseUrl = normalizeFlexFoxUrl(process.env.FLEXFOX_URL);
const scratchName = "lease.chk";

if (process.env.FLEXFOX_WIFI_LEASE_TEST_CONFIRM !== "TEST UPDATE WIFI LEASE") {
  throw new Error("Set FLEXFOX_WIFI_LEASE_TEST_CONFIRM='TEST UPDATE WIFI LEASE' to authorize the scratch-file test");
}
if (!expectedSsid || !/^Tx_[0-9A-F]{8}$/.test(expectedSsid)) {
  throw new Error("Set FLEXFOX_EXPECTED_DEVICE_SSID to the exact MAC-derived device SSID");
}
if (!Number.isInteger(durationMs) || durationMs < 125000 || durationMs > 180000) {
  throw new Error("FLEXFOX_WIFI_LEASE_TEST_DURATION_MS must be 125000 through 180000");
}

const espHeader = readFileSync(join(repoRoot, "Software/Huzzah/ARDF_Transmitter/esp8266.h"), "utf8");
const avrHeader = readFileSync(join(repoRoot, "Software/AVR128DA48/FlexFox80/defs.h"), "utf8");
const expectedEsp = espHeader.match(/WIFI_SW_VERSION\s+\("([^"]+)"\)/)?.[1];
const expectedAvr = avrHeader.match(/SW_REVISION\s+"([^"]+)"/)?.[1];

async function queryIdentity() {
  const websocketUrl = new URL(baseUrl);
  websocketUrl.protocol = "ws:";
  websocketUrl.port = "81";
  return new Promise((resolvePromise, rejectPromise) => {
    const replies = new Map();
    const socket = new WebSocket(websocketUrl);
    const timer = setTimeout(() => rejectPromise(new Error("identity query timed out")), 7000);
    const finish = () => {
      if (!replies.has("SSID") || !replies.has("SW_VERSIONS")) return;
      clearTimeout(timer);
      socket.close(1000, "identity verified");
      resolvePromise(replies);
    };
    socket.addEventListener("open", () => {
      socket.send("SSID");
      socket.send("SW_VERSIONS");
    });
    socket.addEventListener("message", (event) => {
      const message = String(event.data);
      const comma = message.indexOf(",");
      if (comma > 0) replies.set(message.slice(0, comma), message.slice(comma + 1));
      finish();
    });
    socket.addEventListener("error", () => rejectPromise(new Error("identity query failed")));
  });
}

const statusResponse = await fetchWithTimeout(new URL("firmware/status", baseUrl), { cache: "no-store" }, 5000);
const status = await statusResponse.json();
if (status.deviceSsid !== expectedSsid || status.version !== expectedEsp ||
    status.filesystemMounted !== true || status.recoveryMode !== false) {
  throw new Error(`unexpected target status: ${JSON.stringify(status)}`);
}
const identity = await queryIdentity();
if (identity.get("SSID") !== expectedSsid || identity.get("SW_VERSIONS") !== `${expectedEsp},${expectedAvr}`) {
  throw new Error(`unexpected live identity: ${JSON.stringify(Object.fromEntries(identity))}`);
}

const fileBytes = randomBytes(72 * 1024);
const boundary = `----FlexFoxLease-${randomBytes(8).toString("hex")}`;
const before = Buffer.from(
  `--${boundary}\r\nContent-Disposition: form-data; name="name"; filename="${scratchName}"\r\n` +
  "Content-Type: application/octet-stream\r\n\r\n",
);
const after = Buffer.from(`\r\n--${boundary}--\r\n`);
const chunks = 135;
const intervalMs = Math.ceil(durationMs / chunks);
let offset = 0;
let sentChunks = 0;
const body = new ReadableStream({
  start(controller) {
    controller.enqueue(before);
    const timer = setInterval(() => {
      const remainingChunks = chunks - sentChunks;
      const count = remainingChunks <= 1 ? fileBytes.length - offset :
        Math.ceil((fileBytes.length - offset) / remainingChunks);
      controller.enqueue(fileBytes.subarray(offset, offset + count));
      offset += count;
      sentChunks += 1;
      if (offset >= fileBytes.length) {
        clearInterval(timer);
        controller.enqueue(after);
        controller.close();
      }
    }, intervalMs);
  },
});

const uploadUrl = new URL("upload", baseUrl);
uploadUrl.searchParams.set("size", String(fileBytes.length));
uploadUrl.searchParams.set("crc32", crc32(fileBytes).toString(16).padStart(8, "0"));
const started = Date.now();
const response = await fetchWithTimeout(uploadUrl, {
  method: "POST",
  headers: {
    "Content-Length": String(before.length + fileBytes.length + after.length),
    "Content-Type": `multipart/form-data; boundary=${boundary}`,
  },
  body,
  duplex: "half",
}, durationMs + 30000);
const elapsed = Date.now() - started;
if (!response.ok) throw new Error(`slow upload failed with HTTP ${response.status}: ${await response.text()}`);
if (elapsed < 120000) throw new Error(`upload completed too quickly to cross the old timeout: ${elapsed} ms`);

const downloadResponse = await fetchWithTimeout(new URL(scratchName, baseUrl), {}, 10000);
const downloaded = Buffer.from(await downloadResponse.arrayBuffer());
if (!downloaded.equals(fileBytes)) throw new Error("scratch-file readback did not match the slow upload");
await fetchWithTimeout(new URL("delete", baseUrl), {
  method: "POST",
  headers: { "Content-Type": "application/x-www-form-urlencoded" },
  body: new URLSearchParams({ name: scratchName }),
}, 10000);
const missing = await fetchWithTimeout(new URL(scratchName, baseUrl), { cache: "no-store" }, 5000);
if (missing.status !== 404) throw new Error("scratch file was not removed after the test");

console.log(`PASS ${expectedSsid} remained available for the ${elapsed} ms maintenance upload`);
console.log("PASS exact scratch-file readback and cleanup completed");
