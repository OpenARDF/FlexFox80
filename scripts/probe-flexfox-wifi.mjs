#!/usr/bin/env node

const baseUrl = new URL(process.env.FLEXFOX_URL ?? "http://73.73.73.73/");
const timeoutMs = Number.parseInt(process.env.FLEXFOX_PROBE_TIMEOUT_MS ?? "12000", 10);
const monitorMode = process.env.FLEXFOX_PROBE_MONITOR === "1";

if (baseUrl.protocol !== "http:") {
  throw new Error("FLEXFOX_URL must use http:// because the deployed module does not serve TLS");
}
if (!Number.isInteger(timeoutMs) || timeoutMs < 3000 || timeoutMs > 60000) {
  throw new Error("FLEXFOX_PROBE_TIMEOUT_MS must be an integer from 3000 through 60000");
}

const websocketUrlOverride = process.env.FLEXFOX_WEBSOCKET_URL;
const websocketUrl = new URL(websocketUrlOverride ?? baseUrl);
websocketUrl.protocol = "ws:";
if (!websocketUrlOverride) websocketUrl.port = "81";
websocketUrl.pathname = "/";
websocketUrl.search = "";
websocketUrl.hash = "";

const safeRequests = ["SSID", "MAC", "SW_VERSIONS", "MASTER"];

if (process.env.FLEXFOX_PROBE_DRY_RUN === "1") {
  console.log(`HTTP target: ${baseUrl.href}`);
  console.log(`WebSocket target: ${websocketUrl.href}`);
  console.log(`Safe requests: !&, ${safeRequests.join(", ")}`);
  console.log("The WebSocket connection also asks the AVR for temperature and battery state.");
  console.log(`Continuous monitor: ${monitorMode ? "enabled" : "disabled"}`);
  process.exit(0);
}

const messages = [];
let socket;
let heartbeat;
let finishTimer;
let finishProbe;
let livePathReported = false;

function messageType(message) {
  return message.split(",", 1)[0].toUpperCase();
}

function closeSocket() {
  if (heartbeat) clearInterval(heartbeat);
  if (finishTimer) clearTimeout(finishTimer);
  if (socket && socket.readyState < WebSocket.CLOSING) socket.close(1000, "probe complete");
}

function reportLivePathIfComplete() {
  if (livePathReported) return;

  const observed = new Set(messages.map(messageType));
  if (["SSID", "MAC", "SW_VERSIONS", "MASTER", "TEMP", "BAT"].every((type) => observed.has(type))) {
    livePathReported = true;
    console.log("PASS WiFi-to-AVR read-only path returned temperature and battery data");
    if (monitorMode) console.log("MONITOR Sending !& every 5 seconds; stop with Ctrl-C");
    else finishProbe?.();
  }
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
    finishProbe = resolve;
    socket = new WebSocket(websocketUrl);

    const failTimer = setTimeout(() => reject(new Error("WebSocket connection timed out")), 4000);

    socket.addEventListener("open", () => {
      clearTimeout(failTimer);
      console.log(`PASS WebSocket connected ${websocketUrl.href}`);

      socket.send("!&");
      safeRequests.forEach((request, index) => {
        setTimeout(() => {
          if (socket.readyState === WebSocket.OPEN) socket.send(request);
        }, 250 * (index + 1));
      });

      heartbeat = setInterval(() => {
        if (socket.readyState === WebSocket.OPEN) socket.send("!&");
      }, 5000);

      if (!monitorMode) finishTimer = setTimeout(resolve, timeoutMs);
    });

    socket.addEventListener("message", (event) => {
      const message = String(event.data);
      messages.push(message);
      console.log(`RECV ${message}`);
      reportLivePathIfComplete();
    });

    socket.addEventListener("error", () => reject(new Error("WebSocket error")));
    socket.addEventListener("close", (event) => {
      if (event.code !== 1000) reject(new Error(`WebSocket closed unexpectedly (${event.code})`));
    });
  });

  const observed = new Set(messages.map(messageType));
  const missingIdentity = ["SSID", "MAC", "SW_VERSIONS", "MASTER"].filter(
    (type) => !observed.has(type),
  );
  const missingAvr = ["TEMP", "BAT"].filter((type) => !observed.has(type));

  if (missingIdentity.length > 0) {
    throw new Error(`missing WiFi identity replies: ${missingIdentity.join(", ")}`);
  }
  if (missingAvr.length > 0) {
    throw new Error(`missing live AVR replies: ${missingAvr.join(", ")}`);
  }

  reportLivePathIfComplete();
} finally {
  closeSocket();
}
