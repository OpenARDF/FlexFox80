import { setTimeout as sleep } from "node:timers/promises";

export const flexFoxTargetHost = "73.73.73.73";
export const defaultFlexFoxRelayHttpPort = 18080;
export const defaultFlexFoxRelayWebSocketPort = 18081;

export function parseFlexFoxRelayPort(value, label) {
  const port = Number.parseInt(value, 10);
  if (!Number.isInteger(port) || port < 1024 || port > 65535) {
    throw new Error(`${label} port must be an integer from 1024 through 65535`);
  }
  return port;
}

export function flexFoxRelayUrls(httpPort, webSocketPort) {
  return {
    httpUrl: `http://127.0.0.1:${httpPort}/`,
    webSocketUrl: `ws://127.0.0.1:${webSocketPort}/`,
  };
}

/*
 * Keep the Android relay implementation in one place. The Moto owns the Wi-Fi
 * route to the FlexFox, toybox nc exposes device-local listeners, and adb
 * forward carries only the two FlexFox application ports over USB to the Mac.
 */
export function createFlexFoxAdbRelay({
  adb,
  expectedSsid,
  httpPort = defaultFlexFoxRelayHttpPort,
  webSocketPort = defaultFlexFoxRelayWebSocketPort,
  targetHost = flexFoxTargetHost,
}) {
  if (typeof adb !== "function") throw new Error("ADB relay requires an adb command function");
  if (expectedSsid !== "Tx_Master" && !/^Tx_[0-9A-F]{8}$/.test(expectedSsid)) {
    throw new Error("ADB relay requires an exact FlexFox SSID");
  }

  const deviceHttpPort = httpPort;
  const deviceWebSocketPort = webSocketPort;
  const urls = flexFoxRelayUrls(httpPort, webSocketPort);

  async function readWifiStatus() {
    const result = await adb(["shell", "cmd", "wifi", "status"], { allowFailure: true });
    return result.stdout;
  }

  async function requestWifiAssociation() {
    await adb(
      ["shell", "cmd", "wifi", "connect-network", expectedSsid, "open", "-r", "none"],
      { allowFailure: true },
    );
  }

  function wifiStatusMatchesExpectedSsid(output) {
    return String(output ?? "").includes(`Wifi is connected to \"${expectedSsid}\"`);
  }

  async function waitForWifiAssociation(timeoutMs = 90000) {
    const deadline = Date.now() + timeoutMs;
    let joinRequestedAt = 0;
    while (Date.now() < deadline) {
      const status = await readWifiStatus();
      if (wifiStatusMatchesExpectedSsid(status)) return;
      if (Date.now() - joinRequestedAt >= 3000) {
        await requestWifiAssociation();
        joinRequestedAt = Date.now();
      }
      await sleep(1000);
    }
    throw new Error(
      `Moto did not associate with ${expectedSsid} within ${Math.ceil(timeoutMs / 1000)} seconds`,
    );
  }

  async function devicePortIsListening(port) {
    const result = await adb(
      ["shell", "toybox", "nc", "-z", "-w", "1", "127.0.0.1", String(port)],
      { allowFailure: true, timeoutMs: 5000 },
    );
    return result.code === 0;
  }

  async function ensureDeviceRelay(devicePort, targetPort) {
    if (await devicePortIsListening(devicePort)) return;
    const relayCommand =
      `toybox nc -s 127.0.0.1 -p ${devicePort} -L ` +
      `toybox nc -w 8 ${targetHost} ${targetPort} >/dev/null 2>&1 &`;
    await adb(["shell", relayCommand], { timeoutMs: 5000 });
    await sleep(250);
    if (!(await devicePortIsListening(devicePort))) {
      throw new Error(`Moto relay did not start on device port ${devicePort}`);
    }
  }

  async function prepareRelays() {
    await ensureDeviceRelay(deviceHttpPort, 80);
    await ensureDeviceRelay(deviceWebSocketPort, 81);
    await adb(["forward", `tcp:${httpPort}`, `tcp:${deviceHttpPort}`]);
    await adb(["forward", `tcp:${webSocketPort}`, `tcp:${deviceWebSocketPort}`]);
  }

  return Object.freeze({
    ...urls,
    readWifiStatus,
    requestWifiAssociation,
    waitForWifiAssociation,
    prepareRelays,
  });
}
