#!/usr/bin/env node

import { mkdirSync, writeFileSync } from "node:fs";
import { dirname, resolve } from "node:path";
import { fileURLToPath } from "node:url";

const baseUrl = new URL(process.env.FLEXFOX_URL ?? "http://73.73.73.73/");
const baselineCount = Number.parseInt(process.env.FLEXFOX_CLOCK_PHASE_BASELINE ?? "12", 10);
const trialCount = Number.parseInt(process.env.FLEXFOX_CLOCK_PHASE_TRIALS ?? "30", 10);
const edgesPerWrite = Number.parseInt(process.env.FLEXFOX_CLOCK_PHASE_EDGES_PER_WRITE ?? "3", 10);
const authorized = process.env.FLEXFOX_CLOCK_PHASE_TEST === "1";
const dryRun = process.env.FLEXFOX_CLOCK_PHASE_DRY_RUN === "1";
const signatureOffsetsSeconds = [8, -8, 0];
const sendPhaseMs = 100;
const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const outputPath = resolve(
  process.env.FLEXFOX_CLOCK_PHASE_OUTPUT ??
    resolve(repoRoot, "Software", "Huzzah", "tmp", "clock-phase-latest.json"),
);
const commands = {
  quiet: "$ESP,C;",
  edge: "$ESP,S;",
  resume: "$ESP,R;",
};

if (baseUrl.protocol !== "http:") {
  throw new Error("FLEXFOX_URL must use http:// because the deployed module does not serve TLS");
}
if (!Number.isInteger(baselineCount) || baselineCount < 3 || baselineCount > 100) {
  throw new Error("FLEXFOX_CLOCK_PHASE_BASELINE must be an integer from 3 through 100");
}
if (!Number.isInteger(trialCount) || trialCount < 3 || trialCount > 300) {
  throw new Error("FLEXFOX_CLOCK_PHASE_TRIALS must be an integer from 3 through 300");
}
if (!Number.isInteger(edgesPerWrite) || edgesPerWrite < 2 || edgesPerWrite > 10) {
  throw new Error("FLEXFOX_CLOCK_PHASE_EDGES_PER_WRITE must be an integer from 2 through 10");
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
  console.log(`Baseline one-shot edges: ${baselineCount}`);
  console.log(`Queued clock-write/edge trials: ${trialCount}`);
  console.log(`One-shot edges per write: ${edgesPerWrite}`);
  console.log(`Signature offsets: ${signatureOffsetsSeconds.join(", ")} seconds, repeated`);
  console.log(`Controlled send phase: ${sendPhaseMs} ms after the Mac second boundary`);
  console.log(`Ignored evidence output: ${outputPath}`);
  console.log("Final action: queued current-time restore, one-shot edge confirmation, and resume");
  console.log("No EEPROM, event, RF, or filesystem state on the FlexFox is changed.");
  process.exit(0);
}

if (!authorized) {
  throw new Error(
    "set FLEXFOX_CLOCK_PHASE_TEST=1 only for an authorized dummy-loaded test unit",
  );
}

let socket;
let heartbeat;
let quietRequested = false;
let restored = false;
const syncMessages = [];
const baselineSamples = [];
const writeSamples = [];
const followupSamples = [];

function sleep(milliseconds) {
  return new Promise((resolveSleep) => setTimeout(resolveSleep, milliseconds));
}

function sendPass(command) {
  socket.send(`PASS,${command}`);
}

function positiveModulo(value, modulus) {
  return ((value % modulus) + modulus) % modulus;
}

function median(values) {
  const sorted = [...values].sort((a, b) => a - b);
  const middle = Math.floor(sorted.length / 2);
  return sorted.length % 2 ? sorted[middle] : (sorted[middle - 1] + sorted[middle]) / 2;
}

function linearStats(values) {
  const mean = values.reduce((sum, value) => sum + value, 0) / values.length;
  const squaredDeviation = values.reduce((sum, value) => sum + (value - mean) ** 2, 0);
  return {
    count: values.length,
    meanMs: mean,
    sampleStdDevMs: values.length > 1 ? Math.sqrt(squaredDeviation / (values.length - 1)) : 0,
    medianMs: median(values),
    minMs: Math.min(...values),
    maxMs: Math.max(...values),
    atLeastOneSecondCount: values.filter((value) => value >= 1000).length,
  };
}

function circularStats(phasesMs) {
  const angles = phasesMs.map((phase) => (phase / 1000) * 2 * Math.PI);
  const meanSin = angles.reduce((sum, angle) => sum + Math.sin(angle), 0) / angles.length;
  const meanCos = angles.reduce((sum, angle) => sum + Math.cos(angle), 0) / angles.length;
  const resultantLength = Math.hypot(meanSin, meanCos);
  const meanAngle = positiveModulo(Math.atan2(meanSin, meanCos), 2 * Math.PI);
  return {
    count: phasesMs.length,
    meanPhaseMs: (meanAngle / (2 * Math.PI)) * 1000,
    circularStdDevMs:
      (Math.sqrt(Math.max(0, -2 * Math.log(Math.max(resultantLength, Number.EPSILON)))) /
        (2 * Math.PI)) *
      1000,
    resultantLength,
  };
}

function signedCircularDifferenceMs(value, reference) {
  return positiveModulo(value - reference + 500, 1000) - 500;
}

function waitForNextSync(startIndex, description, timeoutMs = 5000) {
  const deadline = Date.now() + timeoutMs;
  return new Promise((resolveWait, reject) => {
    const poll = () => {
      if (syncMessages.length > startIndex) resolveWait(syncMessages[startIndex]);
      else if (Date.now() >= deadline) reject(new Error(`timed out waiting for ${description}`));
      else setTimeout(poll, 20);
    };
    poll();
  });
}

async function waitForControlledSendPhase() {
  const now = Date.now();
  let target = Math.floor(now / 1000) * 1000 + sendPhaseMs;
  if (target - now < 50) target += 1000;
  await sleep(target - now);
}

function edgeSample(message, offsetSeconds, sentMs = null, requestedEpoch = null) {
  const effectiveEpoch = message.epoch - offsetSeconds;
  const edgeErrorMs = message.receivedMs - effectiveEpoch * 1000;
  return {
    sentMs,
    requestedEpoch,
    reportedEpoch: message.epoch,
    offsetSeconds,
    receivedMs: message.receivedMs,
    edgeErrorMs,
    phaseMs: positiveModulo(edgeErrorMs, 1000),
    wholeSecondComponent: Math.floor(edgeErrorMs / 1000),
  };
}

async function requestOneShotEdge(description) {
  const startIndex = syncMessages.length;
  sendPass(commands.edge);
  return waitForNextSync(startIndex, description);
}

async function queueClockWriteAndEdge(offsetSeconds, description) {
  await waitForControlledSendPhase();
  const sentMs = Date.now();
  const requestedEpoch = Math.floor(sentMs / 1000) + offsetSeconds;
  const iso = new Date(requestedEpoch * 1000).toISOString().replace(".000Z", "Z");
  const startIndex = syncMessages.length;
  sendPass(`$TIM,${iso},C;`);
  sendPass(commands.edge);
  const message = await waitForNextSync(startIndex, description, 6000);
  if (message.epoch < requestedEpoch || message.epoch > requestedEpoch + 3) {
    throw new Error(
      `${description} returned epoch ${message.epoch}, expected ${requestedEpoch} through ${requestedEpoch + 3}`,
    );
  }
  return edgeSample(message, offsetSeconds, sentMs, requestedEpoch);
}

async function bestEffortRestore(reason) {
  if (restored || !socket || socket.readyState !== WebSocket.OPEN) return;
  try {
    const sample = await queueClockWriteAndEdge(0, reason);
    restored = true;
    console.error(
      `RESTORE confirmed reported_epoch=${sample.reportedEpoch} edge_error_ms=${sample.edgeErrorMs}`,
    );
  } catch (error) {
    console.error(`RESTORE failed: ${error.message}`);
  }
}

function closeSocket() {
  if (heartbeat) clearInterval(heartbeat);
  if (socket && socket.readyState < WebSocket.CLOSING) socket.close(1000, "phase test complete");
}

for (const signal of ["SIGINT", "SIGTERM"]) {
  process.once(signal, async () => {
    await bestEffortRestore("signal restore");
    if (quietRequested && socket?.readyState === WebSocket.OPEN) sendPass(commands.resume);
    closeSocket();
    process.exit(1);
  });
}

try {
  const response = await fetch(baseUrl, { signal: AbortSignal.timeout(3000) });
  if (!response.ok) throw new Error(`HTTP probe returned ${response.status}`);
  console.log(`PASS HTTP ${response.status} ${baseUrl.href}`);

  await new Promise((resolveOpen, reject) => {
    socket = new WebSocket(websocketUrl);
    const failTimer = setTimeout(() => reject(new Error("WebSocket connection timed out")), 4000);

    socket.addEventListener("open", () => {
      clearTimeout(failTimer);
      console.log(`PASS WebSocket connected ${websocketUrl.href}`);
      socket.send("!&");
      heartbeat = setInterval(() => {
        if (socket.readyState === WebSocket.OPEN) socket.send("!&");
      }, 5000);
      resolveOpen();
    });

    socket.addEventListener("message", (event) => {
      const match = /^SYNC,(\d+)$/.exec(String(event.data).trim());
      if (!match) return;
      syncMessages.push({ epoch: Number.parseInt(match[1], 10), receivedMs: Date.now() });
    });

    socket.addEventListener("error", () => reject(new Error("WebSocket error")));
  });

  await waitForNextSync(0, "initial ordinary clock report", 7000);
  sendPass(commands.quiet);
  quietRequested = true;
  await sleep(750);
  const quietIndex = syncMessages.length;
  await sleep(2500);
  if (syncMessages.length !== quietIndex) {
    throw new Error("ordinary clock reports continued after clone quiet command");
  }
  console.log("PASS clone quiet mode suppresses ordinary clock reports");

  for (let index = 0; index < baselineCount; index++) {
    const message = await requestOneShotEdge(`baseline edge ${index + 1}/${baselineCount}`);
    const sample = edgeSample(message, 0);
    baselineSamples.push(sample);
    console.log(
      `BASELINE ${index + 1}/${baselineCount} epoch=${sample.reportedEpoch} ` +
        `edge_error_ms=${sample.edgeErrorMs} phase_ms=${sample.phaseMs}`,
    );
    await sleep(100);
  }

  for (let index = 0; index < trialCount; index++) {
    const offsetSeconds = signatureOffsetsSeconds[index % signatureOffsetsSeconds.length];
    const sample = await queueClockWriteAndEdge(
      offsetSeconds,
      `write edge ${index + 1}/${trialCount}`,
    );
    sample.trial = index + 1;
    sample.edgeInTrial = 1;
    writeSamples.push(sample);
    console.log(
      `WRITE ${index + 1}/${trialCount} offset_s=${offsetSeconds} requested=${sample.requestedEpoch} ` +
        `reported=${sample.reportedEpoch} edge_error_ms=${sample.edgeErrorMs} ` +
        `phase_ms=${sample.phaseMs} whole_s=${sample.wholeSecondComponent}`,
    );

    let previousEpoch = sample.reportedEpoch;
    for (let edgeIndex = 2; edgeIndex <= edgesPerWrite; edgeIndex++) {
      const message = await requestOneShotEdge(
        `follow-up edge ${edgeIndex}/${edgesPerWrite} for write ${index + 1}/${trialCount}`,
      );
      if (message.epoch <= previousEpoch || message.epoch > previousEpoch + 3) {
        throw new Error(
          `write ${index + 1} follow-up epoch ${message.epoch} did not follow ${previousEpoch}`,
        );
      }
      const followup = edgeSample(message, offsetSeconds, sample.sentMs, sample.requestedEpoch);
      followup.trial = index + 1;
      followup.edgeInTrial = edgeIndex;
      followupSamples.push(followup);
      previousEpoch = followup.reportedEpoch;
      console.log(
        `FOLLOWUP write=${index + 1}/${trialCount} edge=${edgeIndex}/${edgesPerWrite} ` +
          `reported=${followup.reportedEpoch} edge_error_ms=${followup.edgeErrorMs} ` +
          `phase_ms=${followup.phaseMs} whole_s=${followup.wholeSecondComponent}`,
      );
    }
  }

  const restoreSample = await queueClockWriteAndEdge(0, "final current-time restore");
  restored = true;
  console.log(
    `RESTORE confirmed reported_epoch=${restoreSample.reportedEpoch} ` +
      `edge_error_ms=${restoreSample.edgeErrorMs}`,
  );

  sendPass(commands.resume);
  await sleep(750);
  quietRequested = false;

  const baselineLinear = linearStats(baselineSamples.map((sample) => sample.edgeErrorMs));
  const writeLinear = linearStats(writeSamples.map((sample) => sample.edgeErrorMs));
  const followupLinear = linearStats(followupSamples.map((sample) => sample.edgeErrorMs));
  const baselineCircular = circularStats(baselineSamples.map((sample) => sample.phaseMs));
  const writeCircular = circularStats(writeSamples.map((sample) => sample.phaseMs));
  const followupCircular = circularStats(followupSamples.map((sample) => sample.phaseMs));
  const phaseShiftMs = signedCircularDifferenceMs(
    writeCircular.meanPhaseMs,
    baselineCircular.meanPhaseMs,
  );
  const evidence = {
    generatedAt: new Date().toISOString(),
    target: baseUrl.href,
    baseline: { linear: baselineLinear, circular: baselineCircular, samples: baselineSamples },
    writes: {
      firstEdge: { linear: writeLinear, circular: writeCircular, samples: writeSamples },
      followupEdges: {
        linear: followupLinear,
        circular: followupCircular,
        samples: followupSamples,
      },
    },
    phaseShiftMs,
    restore: restoreSample,
  };

  mkdirSync(dirname(outputPath), { recursive: true });
  writeFileSync(outputPath, `${JSON.stringify(evidence, null, 2)}\n`);
  console.log(
    `RESULT baseline_mean_ms=${baselineLinear.meanMs.toFixed(1)} ` +
      `baseline_stddev_ms=${baselineLinear.sampleStdDevMs.toFixed(1)} ` +
      `baseline_tail_count=${baselineLinear.atLeastOneSecondCount}/${baselineLinear.count} ` +
      `baseline_phase_mean_ms=${baselineCircular.meanPhaseMs.toFixed(1)} ` +
      `baseline_phase_stddev_ms=${baselineCircular.circularStdDevMs.toFixed(1)} ` +
      `write_mean_ms=${writeLinear.meanMs.toFixed(1)} ` +
      `write_stddev_ms=${writeLinear.sampleStdDevMs.toFixed(1)} ` +
      `write_tail_count=${writeLinear.atLeastOneSecondCount}/${writeLinear.count} ` +
      `write_phase_mean_ms=${writeCircular.meanPhaseMs.toFixed(1)} ` +
      `write_phase_stddev_ms=${writeCircular.circularStdDevMs.toFixed(1)} ` +
      `followup_mean_ms=${followupLinear.meanMs.toFixed(1)} ` +
      `followup_stddev_ms=${followupLinear.sampleStdDevMs.toFixed(1)} ` +
      `followup_tail_count=${followupLinear.atLeastOneSecondCount}/${followupLinear.count} ` +
      `followup_phase_mean_ms=${followupCircular.meanPhaseMs.toFixed(1)} ` +
      `followup_phase_stddev_ms=${followupCircular.circularStdDevMs.toFixed(1)} ` +
      `phase_shift_ms=${phaseShiftMs.toFixed(1)}`,
  );
  console.log(`EVIDENCE ${outputPath}`);
} finally {
  await bestEffortRestore("failure restore");
  if (quietRequested && socket?.readyState === WebSocket.OPEN) {
    sendPass(commands.resume);
    await sleep(750);
  }
  closeSocket();
}
