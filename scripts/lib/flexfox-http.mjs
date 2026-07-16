import { createHash, randomBytes } from "node:crypto";

export function crc32(bytes) {
  let crc = 0xffffffff;
  for (const byte of bytes) {
    crc ^= byte;
    for (let bit = 0; bit < 8; bit += 1) {
      crc = (crc >>> 1) ^ ((crc & 1) ? 0xedb88320 : 0);
    }
  }
  return (crc ^ 0xffffffff) >>> 0;
}

export function digest(algorithm, bytes) {
  return createHash(algorithm).update(bytes).digest("hex");
}

export function sleep(milliseconds) {
  return new Promise((resolvePromise) => setTimeout(resolvePromise, milliseconds));
}

export async function fetchWithTimeout(url, options = {}, timeoutMs = 5000) {
  return fetch(url, {
    ...options,
    signal: AbortSignal.timeout(timeoutMs),
  });
}

export function multipartFileBody(fieldName, filename, bytes) {
  if (!/^[A-Za-z0-9._-]+$/.test(filename)) {
    throw new Error(`unsafe remote filename: ${filename}`);
  }
  const boundary = `----FlexFox80-${randomBytes(12).toString("hex")}`;
  const before = Buffer.from(
    `--${boundary}\r\n` +
    `Content-Disposition: form-data; name="${fieldName}"; filename="${filename}"\r\n` +
    "Content-Type: application/octet-stream\r\n\r\n",
    "utf8",
  );
  const after = Buffer.from(`\r\n--${boundary}--\r\n`, "utf8");
  const body = Buffer.concat([before, bytes, after]);
  return {
    body,
    headers: {
      "Content-Length": String(body.length),
      "Content-Type": `multipart/form-data; boundary=${boundary}`,
    },
  };
}

export function normalizeFlexFoxUrl(value) {
  const url = new URL(value ?? "http://73.73.73.73/");
  if (url.protocol !== "http:") {
    throw new Error("FlexFox device URLs must use http://");
  }
  url.pathname = "/";
  url.search = "";
  url.hash = "";
  return url;
}
