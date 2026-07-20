#!/usr/bin/env node

import { spawn } from "node:child_process";
import { existsSync } from "node:fs";
import { join } from "node:path";
import { setTimeout as sleep } from "node:timers/promises";
import {
  createFlexFoxAdbRelay,
  defaultFlexFoxRelayHttpPort,
  defaultFlexFoxRelayWebSocketPort,
  parseFlexFoxRelayPort,
} from "./lib/flexfox-adb-relay.mjs";
import {
  normalizeFlexFoxSsid,
  parseAdbDevices,
  selectAdbDevice,
} from "./lib/flexfox-fleet-upgrade.mjs";
import { fetchWithTimeout } from "./lib/flexfox-http.mjs";

const ssid = normalizeFlexFoxSsid(process.env.FLEXFOX_SSID ?? process.argv[2]);
if (!ssid) throw new Error("set FLEXFOX_SSID or pass the exact Tx_Master or Tx_XXXXXXXX SSID");

function resolveAdbPath() {
  const explicit = String(process.env.FLEXFOX_ADB ?? "").trim();
  if (explicit) return explicit;
  const androidHome = String(process.env.ANDROID_HOME ?? "").trim();
  if (androidHome) {
    const candidate = join(androidHome, "platform-tools", "adb");
    if (existsSync(candidate)) return candidate;
  }
  const userHome = String(process.env.HOME ?? "").trim();
  if (userHome) {
    const candidate = join(userHome, "Library", "Android", "sdk", "platform-tools", "adb");
    if (existsSync(candidate)) return candidate;
  }
  return "adb";
}

function run(command, args, { allowFailure = false, timeoutMs = 15000 } = {}) {
  return new Promise((resolvePromise, rejectPromise) => {
    const child = spawn(command, args, { stdio: ["ignore", "pipe", "pipe"] });
    let stdout = "";
    let stderr = "";
    let timedOut = false;
    const timer = setTimeout(() => {
      timedOut = true;
      child.kill("SIGKILL");
    }, timeoutMs);
    child.stdout.on("data", chunk => { stdout += chunk.toString(); });
    child.stderr.on("data", chunk => { stderr += chunk.toString(); });
    child.once("error", rejectPromise);
    child.once("close", code => {
      clearTimeout(timer);
      const result = { code, stdout, stderr };
      if (timedOut) rejectPromise(new Error(`${command} timed out after ${timeoutMs} ms`));
      else if (code !== 0 && !allowFailure) {
        rejectPromise(new Error(stderr.trim() || `${command} exited ${code}`));
      } else resolvePromise(result);
    });
  });
}

const adbPath = resolveAdbPath();
const devicesResult = await run(adbPath, ["devices", "-l"]);
const adbSerial = selectAdbDevice(
  parseAdbDevices(devicesResult.stdout),
  process.env.FLEXFOX_ADB_SERIAL,
);
const adb = (args, options = {}) =>
  run(adbPath, ["-s", adbSerial, ...args], options);
const httpPort = parseFlexFoxRelayPort(
  process.env.FLEXFOX_RELAY_HTTP_PORT ?? defaultFlexFoxRelayHttpPort,
  "HTTP relay",
);
const webSocketPort = parseFlexFoxRelayPort(
  process.env.FLEXFOX_RELAY_WEBSOCKET_PORT ?? defaultFlexFoxRelayWebSocketPort,
  "WebSocket relay",
);
const relay = createFlexFoxAdbRelay({ adb, expectedSsid: ssid, httpPort, webSocketPort });

process.stdout.write(`Moto ADB device: ${adbSerial}\n`);
process.stdout.write(`Joining ${ssid} and preparing localhost-only ADB relays...\n`);
await relay.waitForWifiAssociation();
await relay.prepareRelays();
let response;
let lastHttpError;
for (let attempt = 1; attempt <= 5; attempt++) {
  try {
    response = await fetchWithTimeout(relay.httpUrl, { cache: "no-store" }, 5000);
    if (response.ok) break;
    lastHttpError = new Error(`FlexFox root returned HTTP ${response.status}`);
  } catch (error) {
    lastHttpError = error;
  }
  await relay.waitForWifiAssociation(10000);
  await relay.prepareRelays();
  await sleep(1000);
}
if (!response?.ok) {
  throw new Error(`FlexFox HTTP relay did not become ready: ${lastHttpError?.message ?? "timeout"}`);
}

process.stdout.write(`PASS HTTP ${response.status} ${relay.httpUrl}\n`);
process.stdout.write(`PASS relay ready: ${relay.webSocketUrl}\n`);
process.stdout.write("No RNDIS, DroidTether, administrator prompt, or Mac route change was used.\n");
