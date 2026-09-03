#!/usr/bin/env node

import { appendFileSync, mkdirSync } from "node:fs";
import { join, resolve } from "node:path";

import {
  cloneMonitorDisconnectIsExpected,
  cloneMonitorLogFileName,
  cloneMonitorLogRecord,
  cloneMonitorMessageShouldBeCaptured,
} from "./lib/flexfox-clone-monitor-log.mjs";
import { probeTemperatureIsPlausible } from "./lib/flexfox-fleet-upgrade.mjs";

const baseUrl = new URL(process.env.FLEXFOX_URL ?? "http://73.73.73.73/");
const timeoutMs = Number.parseInt(process.env.FLEXFOX_PROBE_TIMEOUT_MS ?? "12000", 10);
const monitorMode = process.env.FLEXFOX_PROBE_MONITOR === "1";
const captureDirectory = String(process.env.FLEXFOX_PROBE_CAPTURE_DIR ?? "").trim();

if (baseUrl.protocol !== "http:") {
  throw new Error("FLEXFOX_URL must use http:// because the deployed module does not serve TLS");
}
if (!Number.isInteger(timeoutMs) || timeoutMs < 3000 || timeoutMs > 60000) {
  throw new Error("FLEXFOX_PROBE_TIMEOUT_MS must be an integer from 3000 through 60000");
}
if (captureDirectory && !monitorMode) {
  throw new Error("FLEXFOX_PROBE_CAPTURE_DIR requires continuous monitor mode");
}

const websocketUrlOverride = process.env.FLEXFOX_WEBSOCKET_URL;
const websocketUrl = new URL(websocketUrlOverride ?? baseUrl);
websocketUrl.protocol = "ws:";
if (!websocketUrlOverride) websocketUrl.port = "81";
websocketUrl.pathname = "/";
websocketUrl.search = "";
websocketUrl.hash = "";

const safeRequests = ["SSID", "MAC", "SW_VERSIONS", "MASTER"];
let capturePath = "";

function captureRecord(kind, details = {}) {
  if (!capturePath) return;
  appendFileSync(capturePath, `${cloneMonitorLogRecord(kind, details)}\n`, "utf8");
}

if (process.env.FLEXFOX_PROBE_DRY_RUN === "1") {
  console.log(`HTTP target: ${baseUrl.href}`);
  console.log(`WebSocket target: ${websocketUrl.href}`);
  console.log(`Safe requests: !&, ${safeRequests.join(", ")}`);
  console.log("The WebSocket connection also asks the AVR for temperature and battery state.");
  console.log(`Continuous monitor: ${monitorMode ? "enabled" : "disabled"}`);
  console.log(`Diagnostic capture: ${captureDirectory || "disabled"}`);
  process.exit(0);
}

if (captureDirectory) {
  const resolvedCaptureDirectory = resolve(process.cwd(), captureDirectory);
  mkdirSync(resolvedCaptureDirectory, { recursive: true });
  capturePath = join(resolvedCaptureDirectory, cloneMonitorLogFileName());
  captureRecord("monitor-start", {
    httpUrl: baseUrl.href,
    webSocketUrl: websocketUrl.href,
  });
  console.log(`CAPTURE Clone diagnostics: ${capturePath}`);
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

function validTemperatureObserved() {
  return messages.some((message) => {
    if (messageType(message) !== "TEMP") return false;
    const delimiter = message.indexOf(",");
    return delimiter >= 0 && probeTemperatureIsPlausible(message.slice(delimiter + 1));
  });
}

function closeSocket() {
  if (heartbeat) clearInterval(heartbeat);
  if (finishTimer) clearTimeout(finishTimer);
  if (socket && socket.readyState < WebSocket.CLOSING) socket.close(1000, "probe complete");
}

function reportLivePathIfComplete() {
  if (livePathReported) return;

  const observed = new Set(messages.map(messageType));
  if (["SSID", "MAC", "SW_VERSIONS", "MASTER", "BAT"].every((type) => observed.has(type)) &&
      validTemperatureObserved()) {
    livePathReported = true;
    console.log("PASS WiFi-to-AVR read-only path returned temperature and battery data");
    if (monitorMode) console.log("MONITOR Sending !& every 5 seconds; stop with Ctrl-C");
    else finishProbe?.();
  }
}

for (const signal of ["SIGINT", "SIGTERM"]) {
  process.once(signal, () => {
    captureRecord("monitor-stop", { signal });
    closeSocket();
    process.exit(0);
  });
}

try {
  const response = await fetch(baseUrl, { signal: AbortSignal.timeout(3000) });
  if (!response.ok) throw new Error(`HTTP probe returned ${response.status}`);
  console.log(`PASS HTTP ${response.status} ${baseUrl.href}`);

  await new Promise((resolve, reject) => {
    let settled = false;
    finishProbe = () => {
      if (settled) return;
      settled = true;
      resolve();
    };
    socket = new WebSocket(websocketUrl);

    const fail = (reason) => {
      if (settled) return;
      settled = true;
      reject(new Error(reason));
    };
    const finishOrFailOnDisconnect = (reason) => {
      if (settled) return;
      if (cloneMonitorDisconnectIsExpected(monitorMode, livePathReported)) {
        captureRecord("monitor-disconnect", { reason });
        console.log(`MONITOR DISCONNECTED: ${reason}`);
        finishProbe();
        return;
      }
      fail(reason);
    };

    const failTimer = setTimeout(() => fail("WebSocket connection timed out"), 4000);

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

      if (!monitorMode) finishTimer = setTimeout(finishProbe, timeoutMs);
    });

    socket.addEventListener("message", (event) => {
      const message = String(event.data);
      messages.push(message);
      console.log(`RECV ${message}`);
      if (cloneMonitorMessageShouldBeCaptured(message)) {
        captureRecord("message", { message });
      }
      reportLivePathIfComplete();
    });

    socket.addEventListener("error", () => finishOrFailOnDisconnect("WebSocket error"));
    socket.addEventListener("close", (event) => {
      if (event.code !== 1000) {
        finishOrFailOnDisconnect(`WebSocket closed unexpectedly (${event.code})`);
      }
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
  if (!validTemperatureObserved()) {
    throw new Error("AVR temperature is unavailable, malformed, or outside -20C through 120C");
  }

  reportLivePathIfComplete();
} finally {
  captureRecord("monitor-end");
  closeSocket();
}
