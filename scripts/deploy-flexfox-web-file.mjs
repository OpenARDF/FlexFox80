#!/usr/bin/env node

import { readFileSync, statSync } from "node:fs";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import {
  crc32,
  digest,
  fetchWithTimeout,
  multipartFileBody,
  normalizeFlexFoxUrl,
} from "./lib/flexfox-http.mjs";
import { createBoundedFlexFoxHeartbeat } from "./lib/flexfox-heartbeat.mjs";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const localPath = resolve(
  process.env.FLEXFOX_WEB_FILE ??
    join(repoRoot, "Software", "Huzzah", "ARDF_Transmitter", "data", "events.html"),
);
const remoteName = process.env.FLEXFOX_REMOTE_NAME ?? basename(localPath);
const baseUrl = normalizeFlexFoxUrl(process.env.FLEXFOX_URL);
const uploadTimeoutMs = Number.parseInt(
  process.env.FLEXFOX_WEB_UPLOAD_TIMEOUT_MS ?? "120000",
  10,
);
const verificationTimeoutMs = Number.parseInt(
  process.env.FLEXFOX_WEB_VERIFY_TIMEOUT_MS ?? "60000",
  10,
);

if (process.env.FLEXFOX_WEB_CONFIRM !== "UPDATE FLEXFOX WEB FILE") {
  throw new Error(
    "Set FLEXFOX_WEB_CONFIRM='UPDATE FLEXFOX WEB FILE' to authorize the LittleFS file replacement",
  );
}
if (!Number.isInteger(uploadTimeoutMs) || uploadTimeoutMs < 30000 || uploadTimeoutMs > 300000) {
  throw new Error("FLEXFOX_WEB_UPLOAD_TIMEOUT_MS must be 30000 through 300000");
}
if (
  !Number.isInteger(verificationTimeoutMs) ||
  verificationTimeoutMs < 10000 ||
  verificationTimeoutMs > 110000
) {
  throw new Error("FLEXFOX_WEB_VERIFY_TIMEOUT_MS must be 10000 through 110000");
}
if (!statSync(localPath).isFile()) {
  throw new Error(`web asset is not a file: ${localPath}`);
}
if (!/^[A-Za-z0-9._-]+$/.test(remoteName)) {
  throw new Error(`unsafe remote filename: ${remoteName}`);
}

const content = readFileSync(localPath);
const expectedSha256 = digest("sha256", content);
const expectedCrc32 = crc32(content).toString(16).padStart(8, "0");
const uploadUrl = new URL("upload", baseUrl);
uploadUrl.searchParams.set("size", String(content.length));
uploadUrl.searchParams.set("crc32", expectedCrc32);
const multipart = multipartFileBody("name", remoteName, content);

console.log(`Target: ${baseUrl.href}`);
console.log(`Upload: ${localPath} -> /${remoteName} (${content.length} bytes)`);
console.log(`SHA-256: ${expectedSha256}`);
console.log(`CRC32: ${expectedCrc32}`);

const heartbeat = createBoundedFlexFoxHeartbeat(baseUrl, "web file deployment");
process.once("exit", () => heartbeat.stop());
await heartbeat.start();
console.log("PASS bounded AVR web-deployment heartbeat established");

try {
  const response = await fetchWithTimeout(uploadUrl, {
    method: "POST",
    headers: multipart.headers,
    body: multipart.body,
    redirect: "manual",
  }, uploadTimeoutMs);
  if (!response.ok) {
    throw new Error(`upload returned HTTP ${response.status}: ${(await response.text()).trim()}`);
  }
  heartbeat.poke();
} catch (error) {
  console.warn(`Upload response was interrupted or ambiguous: ${error.message}`);
  console.warn("Verifying the stored file before deciding whether the deployment succeeded.");
}

const verifyUrl = new URL(encodeURIComponent(remoteName), baseUrl);
verifyUrl.searchParams.set("cache", String(Date.now()));
const verifyResponse = await fetchWithTimeout(
  verifyUrl,
  { cache: "no-store" },
  verificationTimeoutMs,
);
if (!verifyResponse.ok) {
  throw new Error(`stored-file verification returned HTTP ${verifyResponse.status}`);
}
const stored = Buffer.from(await verifyResponse.arrayBuffer());
const storedSha256 = digest("sha256", stored);
if (stored.length !== content.length || storedSha256 !== expectedSha256) {
  throw new Error(
    `stored /${remoteName} mismatch: ${stored.length} bytes, SHA-256 ${storedSha256}`,
  );
}

heartbeat.stop();

console.log(`PASS /${remoteName} readback matches ${content.length} bytes and SHA-256 ${expectedSha256}`);
