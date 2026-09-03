const capturedMessageTypes = new Set([
  "SSID",
  "MAC",
  "SW_VERSIONS",
  "MASTER",
  "TEMP",
  "BAT",
  "CDIAG",
  "SLAVE",
  "SUS",
  "SUE",
]);

export function cloneMonitorMessageType(message) {
  return String(message ?? "").split(",", 1)[0].trim().toUpperCase();
}

export function cloneMonitorMessageShouldBeCaptured(message) {
  return capturedMessageTypes.has(cloneMonitorMessageType(message));
}

export function cloneMonitorDisconnectIsExpected(monitorMode, livePathReported) {
  return monitorMode === true && livePathReported === true;
}

export function cloneMonitorLogFileName(date = new Date()) {
  if (!(date instanceof Date) || Number.isNaN(date.getTime())) {
    throw new Error("clone monitor log name requires a valid date");
  }
  return `clone-monitor-${date.toISOString().replace(/[:.]/g, "-")}.jsonl`;
}

export function cloneMonitorLogRecord(kind, details = {}, date = new Date()) {
  if (!(date instanceof Date) || Number.isNaN(date.getTime())) {
    throw new Error("clone monitor log record requires a valid date");
  }
  return JSON.stringify({
    recordedAt: date.toISOString(),
    kind: String(kind),
    ...details,
  });
}
