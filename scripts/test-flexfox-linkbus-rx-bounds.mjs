#!/usr/bin/env node

const baseUrl = new URL(process.env.FLEXFOX_URL ?? "http://73.73.73.73/");
const timeoutMs = Number.parseInt(process.env.FLEXFOX_LINKBUS_TEST_TIMEOUT_MS ?? "15000", 10);
const malformedFrames = ["$ZZZ,ABCDEFGHIJKLMNOPQRSTU;", "$ZZZ,A,B,C,D;", "$AZRX?"];
const recoveryQuery = "$TEM?";
const minimumUnansweredDelayMs = 6000;

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
  malformedFrames.forEach((frame) => console.log(`Malformed non-command frame: ${frame}`));
  console.log(`Read-only recovery query: ${recoveryQuery}`);
  process.exit(0);
}

let socket;
let heartbeat;
let temperatureCount = 0;
let batteryObserved = false;
const temperatureWaiters = [];

function closeSocket() {
  if (heartbeat) clearInterval(heartbeat);
  if (socket && socket.readyState < WebSocket.CLOSING) socket.close(1000, "test complete");
}

function waitFor(predicate, description) {
  return new Promise((resolve, reject) => {
    const deadline = Date.now() + timeoutMs;
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

function waitForNextTemperature(previousCount) {
  return new Promise((resolve, reject) => {
    const timer = setTimeout(
      () => reject(new Error("valid temperature query did not recover after malformed frame")),
      timeoutMs,
    );
    temperatureWaiters.push({
      previousCount,
      resolve: () => {
        clearTimeout(timer);
        resolve();
      },
    });
  });
}

async function requestFreshTemperature() {
  const previousCount = temperatureCount;
  const received = waitForNextTemperature(previousCount);
  socket.send(`PASS,${recoveryQuery}`);
  await received;
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
      if (message.toUpperCase().startsWith("TEMP,")) {
        temperatureCount++;
        for (let i = temperatureWaiters.length - 1; i >= 0; i--) {
          if (temperatureCount > temperatureWaiters[i].previousCount) {
            temperatureWaiters.splice(i, 1)[0].resolve();
          }
        }
      }
      if (message.toUpperCase().startsWith("BAT,")) batteryObserved = true;
    });

    socket.addEventListener("error", () => reject(new Error("WebSocket error")));
    socket.addEventListener("close", (event) => {
      if (event.code !== 1000) reject(new Error(`WebSocket closed unexpectedly (${event.code})`));
    });
  });

  await waitFor(
    () => temperatureCount > 0 && batteryObserved,
    "initial live AVR temperature and battery replies",
  );

  await requestFreshTemperature();
  console.log("PASS raw pass-through temperature query returns a fresh AVR reply");

  for (const frame of malformedFrames) {
    socket.send(`PASS,${frame}`);
    await new Promise((resolve) => setTimeout(resolve, 200));

    console.log(`WAIT ESP will retry the intentionally unanswered frame before sending ${recoveryQuery}`);
    const recoveryStarted = Date.now();
    await requestFreshTemperature();
    const recoveryDelay = Date.now() - recoveryStarted;
    if (recoveryDelay < minimumUnansweredDelayMs) {
      throw new Error(`${frame} was acknowledged unexpectedly after only ${recoveryDelay} ms`);
    }
    console.log(`PASS parser recovered after rejecting ${frame}`);
  }

  console.log("PASS Linkbus receive bounds and next-frame resynchronization");
} finally {
  closeSocket();
}
