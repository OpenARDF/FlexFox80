#!/usr/bin/env node

const baseUrl = new URL(process.env.FLEXFOX_URL ?? "http://73.73.73.73/");
const authorized = process.env.FLEXFOX_ROLE_ASSIGNMENT_TEST === "1";
const dryRun = process.env.FLEXFOX_ROLE_ASSIGNMENT_DRY_RUN === "1";
const expectedEvent = process.env.FLEXFOX_ROLE_ASSIGNMENT_EXPECT_EVENT ?? "";
const expectedRole = process.env.FLEXFOX_ROLE_ASSIGNMENT_EXPECT_ROLE ?? "";

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
  console.log(`FlexFox target: ${baseUrl.href}`);
  console.log(`WebSocket target: ${websocketUrl.href}`);
  console.log(`Expected event: ${expectedEvent || "required for live test"}`);
  console.log(`Restoration role: ${expectedRole || "required for live test"}`);
  console.log("Allowed sends: !&, SSID, EVENT_NAME, TX_ROLE, FREQ, and POWER");
  console.log("Forbidden sends: EXECUTE, PASS, SYNC, CLEAR, and direct AVR commands");
  process.exit(0);
}

if (!authorized) {
  throw new Error(
    "set FLEXFOX_ROLE_ASSIGNMENT_TEST=1 only for an authorized dummy-loaded test unit",
  );
}
if (!expectedEvent || !expectedRole) {
  throw new Error(
    "set FLEXFOX_ROLE_ASSIGNMENT_EXPECT_EVENT and FLEXFOX_ROLE_ASSIGNMENT_EXPECT_ROLE before the live test",
  );
}
if (/[\r\n,]/.test(expectedEvent)) {
  throw new Error("FLEXFOX_ROLE_ASSIGNMENT_EXPECT_EVENT must not contain commas or line breaks");
}
if (!/^\d+:\d+$/.test(expectedRole)) {
  throw new Error("FLEXFOX_ROLE_ASSIGNMENT_EXPECT_ROLE must use the numeric role:slot form");
}

let socket;
let heartbeat;
let restoreInProgress = false;
let baselineCaptured = false;
const messages = [];

function sleep(milliseconds) {
  return new Promise((resolve) => setTimeout(resolve, milliseconds));
}

function messageParts(message) {
  return message.split(",");
}

function waitForMessage(predicate, description, startIndex = 0, timeoutMs = 30000) {
  const deadline = Date.now() + timeoutMs;
  return new Promise((resolve, reject) => {
    const poll = () => {
      const match = messages.slice(startIndex).find(predicate);
      if (match !== undefined) resolve(match);
      else if (Date.now() >= deadline) reject(new Error(`timed out waiting for ${description}`));
      else setTimeout(poll, 25);
    };
    poll();
  });
}

function send(message) {
  if (socket?.readyState !== WebSocket.OPEN) throw new Error("WebSocket is not open");
  socket.send(message);
  console.log(`SEND ${message}`);
}

async function requestSnapshot(eventName) {
  const startIndex = messages.length;
  send(`EVENT_NAME,${eventName}`);
  const eventMessage = await waitForMessage(
    (message) => message.startsWith("EVENT_NAME,"),
    "event name",
    startIndex,
  );
  await waitForMessage(
    (message) => message === "TYPE_NAME,Done,Done",
    "complete role list",
    startIndex,
  );
  const assignmentMessage = await waitForMessage(
    (message) => message.startsWith("TX_ROLE,"),
    "role assignment",
    startIndex,
  );

  const snapshotMessages = messages.slice(startIndex);
  const roles = snapshotMessages
    .filter((message) => message.startsWith("TYPE_NAME,") && message !== "TYPE_NAME,Done,Done")
    .map((message) => {
      const [, name, assignment] = messageParts(message);
      return { name, assignment, roleIndex: Number.parseInt(assignment.split(":", 1)[0], 10) };
    });
  const frequencies = snapshotMessages
    .filter((message) => messageParts(message).length === 3 && message.startsWith("FREQ,"))
    .map((message) => Number.parseInt(messageParts(message)[1], 10));
  const powers = snapshotMessages
    .filter((message) => messageParts(message).length === 3 && message.startsWith("POWER,"))
    .map((message) => Number.parseInt(messageParts(message)[1], 10));

  return {
    eventName: eventMessage.substring("EVENT_NAME,".length),
    assignment: assignmentMessage.substring("TX_ROLE,".length),
    roles,
    frequencies,
    powers,
  };
}

async function queryRoleValues(roleIndex, expectedFrequency, expectedPower) {
  let startIndex = messages.length;
  send(`FREQ,${roleIndex}`);
  const frequencyMessage = await waitForMessage(
    (message) => /^FREQ,\d+$/.test(message),
    `frequency for role ${roleIndex}`,
    startIndex,
  );
  const frequency = Number.parseInt(messageParts(frequencyMessage)[1], 10);

  startIndex = messages.length;
  send(`POWER,${roleIndex}`);
  const powerMessage = await waitForMessage(
    (message) => /^POWER,\d+$/.test(message),
    `power for role ${roleIndex}`,
    startIndex,
  );
  const power = Number.parseInt(messageParts(powerMessage)[1], 10);

  if (frequency !== expectedFrequency || power !== expectedPower) {
    throw new Error(
      `role ${roleIndex} returned ${frequency} Hz/${power} mW; expected ${expectedFrequency} Hz/${expectedPower} mW`,
    );
  }
  console.log(`PASS role ${roleIndex} reports ${frequency} Hz and ${power} mW`);
}

async function setAndConfirm(assignment, eventName) {
  const saveStart = messages.length;
  send(`TX_ROLE,${assignment}`);
  await waitForMessage(
    (message) => message === "SAVED_EVENT",
    `save confirmation for ${assignment}`,
    saveStart,
  );
  const snapshot = await requestSnapshot(eventName);
  if (snapshot.eventName !== eventName) {
    throw new Error(`loaded event ${snapshot.eventName}; expected ${eventName}`);
  }
  if (snapshot.assignment !== assignment) {
    throw new Error(`assignment reloaded as ${snapshot.assignment}; expected ${assignment}`);
  }
  console.log(`PASS assignment ${assignment} persisted for ${eventName}`);
  return snapshot;
}

async function restoreBaseline() {
  if (!baselineCaptured || restoreInProgress || socket?.readyState !== WebSocket.OPEN) return;
  restoreInProgress = true;
  try {
    const restored = await setAndConfirm(expectedRole, expectedEvent);
    if (restored.assignment !== expectedRole) throw new Error("restoration confirmation mismatch");
    console.log(`PASS restored ${expectedEvent} to ${expectedRole}`);
  } finally {
    restoreInProgress = false;
  }
}

function closeSocket() {
  if (heartbeat) clearInterval(heartbeat);
  if (socket && socket.readyState < WebSocket.CLOSING) socket.close(1000, "test complete");
}

for (const signal of ["SIGINT", "SIGTERM"]) {
  process.once(signal, async () => {
    try {
      await restoreBaseline();
    } catch (error) {
      console.error(`RESTORE ERROR ${error.message}`);
    } finally {
      closeSocket();
      process.exit(1);
    }
  });
}

try {
  await new Promise((resolve, reject) => {
    socket = new WebSocket(websocketUrl);
    const failTimer = setTimeout(() => reject(new Error("WebSocket connection timed out")), 4000);

    socket.addEventListener("open", () => {
      clearTimeout(failTimer);
      console.log(`PASS WebSocket connected ${websocketUrl.href}`);
      heartbeat = setInterval(() => {
        if (socket.readyState === WebSocket.OPEN) send("!&");
      }, 5000);
      resolve();
    });

    socket.addEventListener("message", (event) => {
      const message = String(event.data);
      messages.push(message);
      console.log(`RECV ${message}`);
    });

    socket.addEventListener("error", () => reject(new Error("WebSocket error")));
    socket.addEventListener("close", (event) => {
      if (event.code !== 1000) reject(new Error(`WebSocket closed unexpectedly (${event.code})`));
    });
  });

  await sleep(300);
  const readyStart = messages.length;
  send("SSID");
  const identityReady = waitForMessage(
    (message) => message.startsWith("SSID,"),
    "ESP identity before mutation",
    readyStart,
  );
  await sleep(500);
  const baselineReady = requestSnapshot(expectedEvent);
  await identityReady;
  console.log("PASS ESP WebSocket command path is responsive");
  const baseline = await baselineReady;
  if (baseline.eventName !== expectedEvent || baseline.assignment !== expectedRole) {
    throw new Error(
      `restoration point is ${baseline.eventName}/${baseline.assignment}; expected ${expectedEvent}/${expectedRole}`,
    );
  }
  baselineCaptured = true;
  console.log(`PASS restoration point ${baseline.eventName}/${baseline.assignment}`);

  const firstAssignmentByRole = new Map();
  for (const role of baseline.roles) {
    if (!firstAssignmentByRole.has(role.roleIndex)) {
      firstAssignmentByRole.set(role.roleIndex, role.assignment);
    }
  }
  const roleIndices = [...firstAssignmentByRole.keys()].sort((left, right) => left - right);
  if (
    roleIndices.length < 2 ||
    baseline.frequencies.length !== roleIndices.length ||
    baseline.powers.length !== roleIndices.length
  ) {
    throw new Error("active event does not provide at least two complete role configurations");
  }

  for (const roleIndex of roleIndices) {
    const assignment = firstAssignmentByRole.get(roleIndex);
    await setAndConfirm(assignment, expectedEvent);
    await queryRoleValues(roleIndex, baseline.frequencies[roleIndex], baseline.powers[roleIndex]);
  }
} finally {
  try {
    await restoreBaseline();
    await sleep(250);
  } finally {
    closeSocket();
  }
}
