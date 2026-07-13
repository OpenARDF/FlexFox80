#!/usr/bin/env node

const baseUrl = new URL(process.env.FLEXFOX_URL ?? "http://73.73.73.73/");
const authorized = process.env.FLEXFOX_CLONE_CONTROL_TEST === "1";
const dryRun = process.env.FLEXFOX_CLONE_CONTROL_DRY_RUN === "1";
const commands = {
  quiet: "$ESP,C;",
  edge: "$ESP,S;",
  resume: "$ESP,R;",
};

if (baseUrl.protocol !== "http:") {
  throw new Error("FLEXFOX_URL must use http:// because the deployed module does not serve TLS");
}

const websocketUrl = new URL(baseUrl);
websocketUrl.protocol = "ws:";
websocketUrl.port = "81";
websocketUrl.pathname = "/";
websocketUrl.search = "";
websocketUrl.hash = "";

if (dryRun) {
  console.log(`HTTP target: ${baseUrl.href}`);
  console.log(`WebSocket target: ${websocketUrl.href}`);
  console.log(`Quiet command: PASS,${commands.quiet}`);
  console.log(`One-shot edge command: PASS,${commands.edge}`);
  console.log(`Resume command: PASS,${commands.resume}`);
  console.log("The test does not write RTC, EEPROM, event, RF, or filesystem state.");
  process.exit(0);
}

if (!authorized) {
  throw new Error("set FLEXFOX_CLONE_CONTROL_TEST=1 only for an authorized dummy-loaded test unit");
}

let socket;
let heartbeat;
let quietRequested = false;
const syncMessages = [];

function sleep(milliseconds) {
  return new Promise((resolve) => setTimeout(resolve, milliseconds));
}

function sendPass(command) {
  socket.send(`PASS,${command}`);
}

function waitFor(predicate, description, timeoutMs) {
  const deadline = Date.now() + timeoutMs;
  return new Promise((resolve, reject) => {
    const poll = () => {
      if (predicate()) resolve();
      else if (Date.now() >= deadline) reject(new Error(`timed out waiting for ${description}`));
      else setTimeout(poll, 25);
    };
    poll();
  });
}

function closeSocket() {
  if (heartbeat) clearInterval(heartbeat);
  if (socket && socket.readyState < WebSocket.CLOSING) socket.close(1000, "test complete");
}

for (const signal of ["SIGINT", "SIGTERM"]) {
  process.once(signal, () => {
    if (socket?.readyState === WebSocket.OPEN) sendPass(commands.resume);
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
      if (message.toUpperCase().startsWith("SYNC,")) syncMessages.push({ message, receivedAt: Date.now() });
    });

    socket.addEventListener("error", () => reject(new Error("WebSocket error")));
    socket.addEventListener("close", (event) => {
      if (event.code !== 1000) reject(new Error(`WebSocket closed unexpectedly (${event.code})`));
    });
  });

  await waitFor(() => syncMessages.length >= 2, "two baseline clock reports", 7000);
  console.log("PASS baseline clock reports are active");

  sendPass(commands.quiet);
  quietRequested = true;
  await sleep(750);
  const quietBaseline = syncMessages.length;
  await sleep(3500);
  if (syncMessages.length !== quietBaseline) {
    throw new Error("ordinary clock reports continued after clone quiet command");
  }
  console.log("PASS clone quiet mode suppresses ordinary clock reports");

  sendPass(commands.edge);
  await waitFor(() => syncMessages.length === quietBaseline + 1, "one next-edge clock report", 3500);
  const oneShotCount = syncMessages.length;
  console.log("PASS clone sync command produces one next-edge clock report");

  await sleep(2500);
  if (syncMessages.length !== oneShotCount) {
    throw new Error("clone sync command produced more than one clock report while quiet");
  }
  console.log("PASS next-edge clock report is one-shot while quiet");

  sendPass(commands.resume);
  quietRequested = false;
  await waitFor(() => syncMessages.length > oneShotCount, "ordinary clock reports after resume", 4000);
  console.log("PASS normal clock reports resume after clone cleanup");
} finally {
  if (quietRequested && socket?.readyState === WebSocket.OPEN) {
    sendPass(commands.resume);
    await sleep(1000);
  }
  closeSocket();
}
