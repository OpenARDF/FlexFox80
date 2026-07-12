#!/usr/bin/env node

const baseUrl = new URL(process.env.FLEXFOX_URL ?? "http://73.73.73.73/");
const trialCount = Number.parseInt(process.env.FLEXFOX_CLOCK_SYNC_TRIALS ?? "30", 10);
const timeoutMs = Number.parseInt(
  process.env.FLEXFOX_CLOCK_SYNC_TIMEOUT_MS ?? String(Math.max(120000, trialCount * 6000)),
  10,
);
const signatureOffsetsSeconds = [8, -8, 0];
const finalSampleTarget = 3;

if (baseUrl.protocol !== "http:") {
  throw new Error("FLEXFOX_URL must use http:// because the deployed module does not serve TLS");
}
if (!Number.isInteger(trialCount) || trialCount < 3 || trialCount > 300) {
  throw new Error("FLEXFOX_CLOCK_SYNC_TRIALS must be an integer from 3 through 300");
}
if (!Number.isInteger(timeoutMs) || timeoutMs < 30000 || timeoutMs > 3600000) {
  throw new Error("FLEXFOX_CLOCK_SYNC_TIMEOUT_MS must be an integer from 30000 through 3600000");
}

const websocketUrl = new URL(baseUrl);
websocketUrl.protocol = "ws:";
websocketUrl.port = "81";
websocketUrl.pathname = "/";
websocketUrl.search = "";
websocketUrl.hash = "";

if (process.env.FLEXFOX_CLOCK_SYNC_DRY_RUN === "1") {
  console.log(`HTTP target: ${baseUrl.href}`);
  console.log(`WebSocket target: ${websocketUrl.href}`);
  console.log(`Clock-set trials: ${trialCount}`);
  console.log(`Signature offsets: ${signatureOffsetsSeconds.join(", ")} seconds, repeated`);
  console.log("Final action: set current Mac time and collect three confirmation samples");
  console.log("WebSocket sends: SYNC timestamps and !& heartbeat only");
  process.exit(0);
}

if (process.env.FLEXFOX_ALLOW_CLOCK_SET !== "1") {
  throw new Error(
    "clock setting is disabled; review the dry run, then set FLEXFOX_ALLOW_CLOCK_SET=1",
  );
}

let socket;
let heartbeat;
let finishTimer;
let trialIndex = 0;
let activeTrial;
let finalSetSent = false;
let finalRestoreConfirmed = false;
const results = [];
const finalSamples = [];

function closeSocket() {
  if (heartbeat) clearInterval(heartbeat);
  if (finishTimer) clearTimeout(finishTimer);
  if (socket && socket.readyState < WebSocket.CLOSING) socket.close(1000, "clock test complete");
}

function median(values) {
  const sorted = [...values].sort((a, b) => a - b);
  const middle = Math.floor(sorted.length / 2);
  return sorted.length % 2 ? sorted[middle] : (sorted[middle - 1] + sorted[middle]) / 2;
}

function sendClock(offsetSeconds, phase) {
  const sentMs = Date.now();
  const requestedMs = sentMs + offsetSeconds * 1000;
  const iso = new Date(requestedMs).toISOString();
  const message = `SYNC,${iso}`;
  socket.send(message);
  console.log(
    `SET ${phase} offset_s=${offsetSeconds} sent=${new Date(sentMs).toISOString()} requested=${iso}`,
  );
  return { offsetSeconds, sentMs, requestedMs, reportsSeen: 0 };
}

function normalizedErrorMs(targetEpoch, receivedMs, offsetSeconds) {
  return receivedMs - targetEpoch * 1000 + offsetSeconds * 1000;
}

function sendNextTrial() {
  const offsetSeconds = signatureOffsetsSeconds[trialIndex % signatureOffsetsSeconds.length];
  activeTrial = sendClock(offsetSeconds, `${trialIndex + 1}/${trialCount}`);
}

async function bestEffortRestore(reason) {
  if (finalRestoreConfirmed || !socket || socket.readyState !== WebSocket.OPEN) return;
  try {
    activeTrial = sendClock(0, reason);
    console.error("RESTORE waiting briefly for the AVR to apply current Mac time");
    await new Promise((resolve) => setTimeout(resolve, 2500));
  } catch (error) {
    console.error(`RESTORE failed: ${error.message}`);
  }
}

for (const signal of ["SIGINT", "SIGTERM"]) {
  process.once(signal, async () => {
    await bestEffortRestore("signal-restore");
    closeSocket();
    process.exit(1);
  });
}

try {
  const response = await fetch(baseUrl, { signal: AbortSignal.timeout(3000) });
  if (!response.ok) throw new Error(`HTTP probe returned ${response.status}`);
  console.log(`PASS HTTP ${response.status} ${baseUrl.href}`);

  await new Promise((resolve, reject) => {
    socket = new WebSocket(websocketUrl);
    finishTimer = setTimeout(
      () => reject(new Error(`timed out after ${results.length}/${trialCount} verified trials`)),
      timeoutMs,
    );

    socket.addEventListener("open", () => {
      console.log(`PASS WebSocket connected ${websocketUrl.href}`);
      socket.send("!&");
      heartbeat = setInterval(() => {
        if (socket.readyState === WebSocket.OPEN) socket.send("!&");
      }, 5000);
      setTimeout(sendNextTrial, 250);
    });

    socket.addEventListener("message", (event) => {
      const match = /^SYNC,(\d+)$/.exec(String(event.data).trim());
      if (!match || !activeTrial) return;

      const receivedMs = Date.now();
      const targetEpoch = Number.parseInt(match[1], 10);
      const errorMs = normalizedErrorMs(targetEpoch, receivedMs, activeTrial.offsetSeconds);

      if (finalSetSent) {
        finalSamples.push(errorMs);
        console.log(
          `FINAL_SAMPLE ${finalSamples.length}/${finalSampleTarget} target=${targetEpoch} ` +
            `normalized_error_ms=${errorMs}`,
        );
        if (finalSamples.length >= finalSampleTarget) {
          finalRestoreConfirmed = true;
          resolve();
        }
        return;
      }

      activeTrial.reportsSeen++;
      const matched = errorMs >= -500 && errorMs <= 2500;
      console.log(
        `REPORT trial=${trialIndex + 1} target=${targetEpoch} normalized_error_ms=${errorMs} ` +
          `match=${matched ? "yes" : "no"}`,
      );

      if (!matched) {
        if (activeTrial.reportsSeen >= 4) {
          reject(new Error(`trial ${trialIndex + 1} did not produce the requested clock signature`));
        }
        return;
      }

      results.push({
        trial: trialIndex + 1,
        offsetSeconds: activeTrial.offsetSeconds,
        reportsSeen: activeTrial.reportsSeen,
        errorMs,
      });
      trialIndex++;

      if (trialIndex < trialCount) {
        setTimeout(sendNextTrial, 100);
      } else {
        setTimeout(() => {
          activeTrial = sendClock(0, "final-restore");
          finalSetSent = true;
        }, 100);
      }
    });

    socket.addEventListener("error", () => reject(new Error("WebSocket error")));
    socket.addEventListener("close", (event) => {
      if (finalSamples.length < finalSampleTarget) {
        reject(new Error(`WebSocket closed before final confirmation (${event.code})`));
      }
    });
  });

  const firstReportMatches = results.filter((result) => result.reportsSeen === 1).length;
  const finalMedian = median(finalSamples);
  console.log(
    `RESULT verified_trials=${results.length}/${trialCount} ` +
      `first_report_matches=${firstReportMatches}/${trialCount} ` +
      `final_median_error_ms=${finalMedian} ` +
      `final_min_error_ms=${Math.min(...finalSamples)} ` +
      `final_max_error_ms=${Math.max(...finalSamples)}`,
  );

  if (results.length !== trialCount) throw new Error("not every clock-set trial was verified");
  if (finalMedian < -500 || finalMedian > 2500) {
    throw new Error(`final restored clock is outside tolerance (${finalMedian} ms)`);
  }
} finally {
  await bestEffortRestore("failure-restore");
  closeSocket();
}
