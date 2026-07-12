#!/usr/bin/env node

const baseUrl = new URL(process.env.FLEXFOX_URL ?? "http://73.73.73.73/");
const timeoutMs = Number.parseInt(process.env.FLEXFOX_LINKBUS_TEST_TIMEOUT_MS ?? "15000", 10);
const unansweredFrames = ["$ZZZ,ABCDEFGHIJKLMNOPQRSTU;", "$ZZZ,A,B,C,D;", "$AZRX?"];
const aliasProbe = "$RXW?";
const recoveryQueries = ["$TEM?", "$BAT?"];
const minimumUnansweredDelayMs = 6000;
const aliasObservationMs = 750;

if (baseUrl.protocol !== "http:") {
  throw new Error("FLEXFOX_URL must use http:// because the deployed module does not serve TLS");
}
if (!Number.isInteger(timeoutMs) || timeoutMs < 3000 || timeoutMs > 30000) {
  throw new Error("FLEXFOX_LINKBUS_TEST_TIMEOUT_MS must be an integer from 3000 through 30000");
}

const websocketUrl = new URL(baseUrl);
websocketUrl.protocol = "ws:";
websocketUrl.port = "81";
websocketUrl.pathname = "/";
websocketUrl.search = "";
websocketUrl.hash = "";

if (process.env.FLEXFOX_LINKBUS_TEST_DRY_RUN === "1") {
  console.log(`HTTP target: ${baseUrl.href}`);
  console.log(`WebSocket target: ${websocketUrl.href}`);
  unansweredFrames.forEach((frame) => console.log(`Unanswered non-command frame: ${frame}`));
  console.log(`Read-only collision-alias probe: ${aliasProbe}`);
  recoveryQueries.forEach((query) => console.log(`Read-only recovery query: ${query}`));
  process.exit(0);
}

let socket;
let heartbeat;
let temperatureCount = 0;
let batteryCount = 0;

function closeSocket() {
  if (heartbeat) clearInterval(heartbeat);
  if (socket && socket.readyState < WebSocket.CLOSING) socket.close(1000, "test complete");
}

function waitFor(predicate, description, deadline = Date.now() + timeoutMs) {
  return new Promise((resolve, reject) => {
    const poll = () => {
      if (predicate()) {
        resolve();
      } else if (Date.now() >= deadline) {
        reject(new Error(`timed out waiting for ${description}`));
      } else {
        setTimeout(poll, 25);
      }
    };
    poll();
  });
}

function sendQuery(query) {
  socket.send(`PASS,${query}`);
}

function sendRecoveryQueries() {
  for (const query of recoveryQueries) sendQuery(query);
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
    const failTimer = setTimeout(() => reject(new Error("WebSocket connection timed out")), 4000);

    socket.addEventListener("open", () => {
      clearTimeout(failTimer);
      console.log(`PASS WebSocket connected ${websocketUrl.href}`);
      socket.send("!&");
      heartbeat = setInterval(() => {
        if (socket.readyState === WebSocket.OPEN) socket.send("!&");
      }, 5000);
      resolve();
    });

    socket.addEventListener("message", (event) => {
      const message = String(event.data);
      console.log(`RECV ${message}`);
      if (message.toUpperCase().startsWith("TEMP,")) temperatureCount++;
      if (message.toUpperCase().startsWith("BAT,")) batteryCount++;
    });

    socket.addEventListener("error", () => reject(new Error("WebSocket error")));
    socket.addEventListener("close", (event) => {
      if (event.code !== 1000) reject(new Error(`WebSocket closed unexpectedly (${event.code})`));
    });
  });

  await waitFor(
    () => temperatureCount > 0 && batteryCount > 0,
    "initial live AVR temperature and battery replies",
  );

  const initialTemperatureCount = temperatureCount;
  const initialBatteryCount = batteryCount;
  sendRecoveryQueries();
  await waitFor(
    () => temperatureCount > initialTemperatureCount && batteryCount > initialBatteryCount,
    "fresh raw pass-through temperature and battery replies",
  );
  console.log("PASS raw pass-through recovery queries return fresh AVR replies");

  for (const frame of unansweredFrames) {
    const recoveryStarted = Date.now();
    socket.send(`PASS,${frame}`);
    await new Promise((resolve) => setTimeout(resolve, 200));
    sendRecoveryQueries();

    console.log("WAIT ESP will retry the intentionally unanswered frame before sending recovery queries");
    await new Promise((resolve) => setTimeout(resolve, minimumUnansweredDelayMs));

    // Periodic TEMP or BAT telemetry can arrive independently of these queries.
    // Only a fresh pair after the rejection window proves the queued reads ran.
    const lateTemperatureCount = temperatureCount;
    const lateBatteryCount = batteryCount;
    await waitFor(
      () => temperatureCount > lateTemperatureCount && batteryCount > lateBatteryCount,
      `late temperature and battery recovery replies after ${frame}`,
      recoveryStarted + timeoutMs,
    );
    console.log(`PASS parser recovered after rejecting ${frame}`);
  }

  const aliasProbeTemperatureStart = temperatureCount;
  for (let attempt = 1; attempt <= 2; attempt++) {
    socket.send(`PASS,${aliasProbe}`);
    await new Promise((resolve) => setTimeout(resolve, aliasObservationMs));
    console.log(`PASS completed collision-alias observation ${attempt}/2`);
  }
  if (temperatureCount - aliasProbeTemperatureStart >= 2) {
    throw new Error(`${aliasProbe} still behaves like its legacy TEM alias`);
  }

  const postAliasBatteryCount = batteryCount;
  sendQuery(recoveryQueries[1]);
  await waitFor(
    () => batteryCount > postAliasBatteryCount,
    `battery recovery reply after ${aliasProbe}`,
  );
  console.log(`PASS collision-free parser rejects ${aliasProbe} without a TEM response`);

  console.log("PASS Linkbus receive bounds and next-frame resynchronization");
} finally {
  closeSocket();
}
