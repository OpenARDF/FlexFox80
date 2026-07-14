#!/usr/bin/env node

const baseUrl = new URL(process.env.FLEXFOX_URL ?? "http://73.73.73.73/");
const sampleTarget = Number.parseInt(process.env.FLEXFOX_CLOCK_SAMPLES ?? "12", 10);
const timeoutMs = Number.parseInt(
  process.env.FLEXFOX_CLOCK_TIMEOUT_MS ?? String(Math.max(30000, sampleTarget * 4000)),
  10,
);

if (baseUrl.protocol !== "http:") {
  throw new Error("FLEXFOX_URL must use http:// because the deployed module does not serve TLS");
}
if (!Number.isInteger(sampleTarget) || sampleTarget < 3 || sampleTarget > 3600) {
  throw new Error("FLEXFOX_CLOCK_SAMPLES must be an integer from 3 through 3600");
}
if (!Number.isInteger(timeoutMs) || timeoutMs < 10000 || timeoutMs > 86400000) {
  throw new Error("FLEXFOX_CLOCK_TIMEOUT_MS must be an integer from 10000 through 86400000");
}

const websocketUrl = new URL(baseUrl);
websocketUrl.protocol = "ws:";
websocketUrl.port = "81";
websocketUrl.pathname = "/";
websocketUrl.search = "";
websocketUrl.hash = "";

if (process.env.FLEXFOX_CLOCK_DRY_RUN === "1") {
  console.log(`HTTP target: ${baseUrl.href}`);
  console.log(`WebSocket target: ${websocketUrl.href}`);
  console.log(`Clock samples: ${sampleTarget}`);
  console.log(`Timeout: ${timeoutMs} ms`);
  console.log("WebSocket sends: !& heartbeat only");
  process.exit(0);
}

const samples = [];
let socket;
let heartbeat;
let finishTimer;

function closeSocket() {
  if (heartbeat) clearInterval(heartbeat);
  if (finishTimer) clearTimeout(finishTimer);
  if (socket && socket.readyState < WebSocket.CLOSING) socket.close(1000, "observation complete");
}

function median(values) {
  const sorted = [...values].sort((a, b) => a - b);
  const middle = Math.floor(sorted.length / 2);
  return sorted.length % 2 ? sorted[middle] : (sorted[middle - 1] + sorted[middle]) / 2;
}

for (const signal of ["SIGINT", "SIGTERM"]) {
  process.once(signal, () => {
    closeSocket();
    process.exit(0);
  });
}

try {
  const response = await fetch(baseUrl, { signal: AbortSignal.timeout(3000) });
  if (!response.ok) throw new Error(`HTTP probe returned ${response.status}`);
  console.log(`PASS HTTP ${response.status} ${baseUrl.href}`);

  await new Promise((resolve, reject) => {
    socket = new WebSocket(websocketUrl);
    finishTimer = setTimeout(
      () => reject(new Error(`timed out after ${samples.length}/${sampleTarget} clock samples`)),
      timeoutMs,
    );

    socket.addEventListener("open", () => {
      console.log(`PASS WebSocket connected ${websocketUrl.href}`);
      socket.send("!&");
      heartbeat = setInterval(() => {
        if (socket.readyState === WebSocket.OPEN) socket.send("!&");
      }, 5000);
    });

    socket.addEventListener("message", (event) => {
      const message = String(event.data);
      const match = /^SYNC,(\d+)$/.exec(message.trim());
      if (!match) return;

      const receivedMs = Date.now();
      const targetEpoch = Number.parseInt(match[1], 10);
      const offsetMs = receivedMs - targetEpoch * 1000;
      samples.push({ receivedMs, targetEpoch, offsetMs });
      console.log(
        `SAMPLE ${samples.length}/${sampleTarget} target=${targetEpoch} ` +
          `received=${new Date(receivedMs).toISOString()} offset_ms=${offsetMs}`,
      );

      if (samples.length >= sampleTarget) resolve();
    });

    socket.addEventListener("error", () => reject(new Error("WebSocket error")));
    socket.addEventListener("close", (event) => {
      if (samples.length < sampleTarget) {
        reject(new Error(`WebSocket closed after ${samples.length}/${sampleTarget} samples (${event.code})`));
      }
    });
  });

  const offsets = samples.map((sample) => sample.offsetMs);
  const minimum = Math.min(...offsets);
  const maximum = Math.max(...offsets);
  console.log(
    `RESULT samples=${samples.length} median_offset_ms=${median(offsets)} ` +
      `min_offset_ms=${minimum} max_offset_ms=${maximum} spread_ms=${maximum - minimum}`,
  );
  console.log(
    "NOTE offset includes WebSocket/USB-tunnel latency; compare medians between units using the same path.",
  );
} finally {
  closeSocket();
}
