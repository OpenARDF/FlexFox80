export const fleetWebFiles = Object.freeze([
  "events.html",
  "radio.html",
  "test.html",
]);

export function normalizeFleetUnitId(value) {
  const unitId = String(value ?? "").trim();
  return /^[A-Za-z0-9][A-Za-z0-9._-]*$/.test(unitId) ? unitId : undefined;
}

export function normalizeFlexFoxSsid(value) {
  const ssid = String(value ?? "").trim();
  if (ssid === "Tx_Master") return ssid;
  return /^Tx_[0-9A-F]{8}$/.test(ssid) ? ssid : undefined;
}

export function parseAdbDevices(output) {
  return String(output ?? "")
    .split(/\r?\n/)
    .slice(1)
    .map((line) => line.trim().split(/\s+/, 2))
    .filter((fields) => fields.length === 2 && fields[1] === "device")
    .map(([serial]) => serial);
}

export function selectAdbDevice(devices, requestedSerial) {
  const requested = String(requestedSerial ?? "").trim();
  if (requested) {
    if (!devices.includes(requested)) {
      throw new Error(`requested ADB device is unavailable: ${requested}`);
    }
    return requested;
  }
  if (devices.length !== 1) {
    throw new Error(`expected exactly one ADB device, found ${devices.length}`);
  }
  return devices[0];
}

export function wifiStatusMatchesSsid(output, expectedSsid) {
  return String(output ?? "").includes(`Wifi is connected to \"${expectedSsid}\"`);
}

export function espStatusMatchesArtifact(status, artifact) {
  return status?.version === artifact.version &&
    status?.filesystemProtected === true &&
    status?.filesystemMounted === true &&
    status?.recoveryMode === false &&
    status?.currentSketchBytes === artifact.bytes &&
    artifact.installedMd5Values.has(String(status?.currentSketchMd5 ?? "").toLowerCase());
}

export function parseProbeReplies(output) {
  const replies = {};
  for (const line of String(output ?? "").split(/\r?\n/)) {
    const match = line.match(/^RECV ([A-Z_]+),(.*)$/);
    if (match) replies[match[1]] = match[2];
  }
  return replies;
}

export function probeTemperatureIsPlausible(value) {
  // Enforce the same inclusive physical range as the AVR and ESP.
  const match = String(value ?? "").trim().match(/^(-?\d+(?:\.\d+)?)C$/);
  if (!match) return false;
  const temperatureC = Number(match[1]);
  return Number.isFinite(temperatureC) && temperatureC >= -20 && temperatureC <= 120;
}

export function expectedMasterValue(ssid) {
  return ssid === "Tx_Master" ? "1" : "0";
}
