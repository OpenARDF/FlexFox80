#!/usr/bin/env node

/*
 * Fleet coordinator only: destructive work remains in the qualified ESP, AVR,
 * and web deployment scripts. This wrapper owns target identity, Moto relays,
 * phase order, live-state skip decisions, reassociation, and audit evidence.
 */

import { spawn } from "node:child_process";
import {
  appendFileSync,
  existsSync,
  mkdirSync,
  readFileSync,
  writeFileSync,
} from "node:fs";
import { basename, dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import { setTimeout as sleep } from "node:timers/promises";
import {
  digest,
  esp8266SketchMd5Candidates,
  fetchWithTimeout,
} from "./lib/flexfox-http.mjs";
import {
  createFlexFoxAdbRelay,
  defaultFlexFoxRelayHttpPort,
  defaultFlexFoxRelayWebSocketPort,
  parseFlexFoxRelayPort,
} from "./lib/flexfox-adb-relay.mjs";
import {
  espStatusMatchesArtifact,
  expectedMasterValue,
  fleetWebFiles,
  normalizeFleetUnitId,
  normalizeFlexFoxSsid,
  parseAdbDevices,
  parseProbeReplies,
  probeTemperatureIsPlausible,
  selectAdbDevice,
  wifiStatusMatchesSsid,
} from "./lib/flexfox-fleet-upgrade.mjs";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..");
const dryRun = process.env.FLEXFOX_FLEET_UPGRADE_DRY_RUN === "1";
const wirelessOnly = process.env.FLEXFOX_FLEET_WIRELESS_ONLY === "1";
const unitId = normalizeFleetUnitId(process.env.FLEXFOX_UNIT_ID);
const ssid = normalizeFlexFoxSsid(process.env.FLEXFOX_SSID);
const localHttpPort = parseFlexFoxRelayPort(
  process.env.FLEXFOX_RELAY_HTTP_PORT ?? defaultFlexFoxRelayHttpPort,
  "HTTP relay",
);
const localWebSocketPort = parseFlexFoxRelayPort(
  process.env.FLEXFOX_RELAY_WEBSOCKET_PORT ?? defaultFlexFoxRelayWebSocketPort,
  "WebSocket relay",
);
const startedAt = new Date();
const stamp = startedAt.toISOString().replaceAll(":", "-");

if (!unitId) fail("set FLEXFOX_UNIT_ID to a safe unit label such as fox-01 or beacon");
if (!ssid) fail("set FLEXFOX_SSID to Tx_Master or the complete MAC-derived Tx_XXXXXXXX SSID");
if (wirelessOnly && ssid === "Tx_Master") {
  fail("wireless-only AVR updates require the unique MAC-derived Tx_XXXXXXXX SSID");
}
if (!dryRun && process.env.FLEXFOX_FLEET_UPGRADE_CONFIRM !== "UPGRADE FLEXFOX UNIT") {
  fail("set FLEXFOX_FLEET_UPGRADE_CONFIRM='UPGRADE FLEXFOX UNIT' after identifying the unit");
}

const evidenceDir = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "tmp",
  "fleet-upgrade",
  `${stamp}-${unitId}`,
);
mkdirSync(evidenceDir, { recursive: true });
const logPath = join(evidenceDir, "upgrade.log");
const summaryPath = join(evidenceDir, "summary.json");
const summary = {
  format: "flexfox-fleet-upgrade-v1",
  unitId,
  expectedSsid: ssid,
  dryRun,
  wirelessOnly,
  startedAt: startedAt.toISOString(),
  completedAt: null,
  result: "running",
  artifacts: {},
  identity: {},
  phases: {},
};

function fail(message) {
  throw new Error(`FlexFox fleet upgrade: ${message}`);
}

function recordSummary() {
  writeFileSync(summaryPath, `${JSON.stringify(summary, null, 2)}\n`);
}

function log(message = "") {
  const line = `${message}\n`;
  process.stdout.write(line);
  appendFileSync(logPath, line);
}

function logError(message = "") {
  const line = `${message}\n`;
  process.stderr.write(line);
  appendFileSync(logPath, line);
}

async function runProcess(command, args, options = {}) {
  const {
    allowFailure = false,
    echo = true,
    env = process.env,
    timeoutMs = 120000,
  } = options;
  return new Promise((resolvePromise, rejectPromise) => {
    const child = spawn(command, args, {
      cwd: repoRoot,
      env,
      stdio: ["ignore", "pipe", "pipe"],
    });
    let stdout = "";
    let stderr = "";
    let timedOut = false;
    let forceKillTimer;
    const timer = setTimeout(() => {
      timedOut = true;
      child.kill("SIGTERM");
      forceKillTimer = setTimeout(() => child.kill("SIGKILL"), 3000);
    }, timeoutMs);

    child.stdout.on("data", (chunk) => {
      const text = chunk.toString();
      stdout += text;
      appendFileSync(logPath, text);
      if (echo) process.stdout.write(text);
    });
    child.stderr.on("data", (chunk) => {
      const text = chunk.toString();
      stderr += text;
      appendFileSync(logPath, text);
      if (echo) process.stderr.write(text);
    });
    child.once("error", (error) => {
      clearTimeout(timer);
      clearTimeout(forceKillTimer);
      rejectPromise(error);
    });
    child.once("close", (code, signal) => {
      clearTimeout(timer);
      clearTimeout(forceKillTimer);
      const result = { code, signal, stdout, stderr };
      if (timedOut) {
        rejectPromise(new Error(`${command} timed out after ${timeoutMs} ms`));
      } else if (code !== 0 && !allowFailure) {
        const detail = stderr.trim().split(/\r?\n/).slice(-1)[0] || `exit ${code}`;
        rejectPromise(new Error(`${command} failed: ${detail}`));
      } else {
        resolvePromise(result);
      }
    });
  });
}

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

const adbPath = resolveAdbPath();
let adbSerial;

async function adb(args, options = {}) {
  const serialArgs = adbSerial ? ["-s", adbSerial] : [];
  return runProcess(adbPath, [...serialArgs, ...args], {
    echo: false,
    timeoutMs: 15000,
    ...options,
  });
}

const {
  httpUrl,
  webSocketUrl,
  readWifiStatus,
  requestWifiAssociation,
  waitForWifiAssociation,
  prepareRelays,
} = createFlexFoxAdbRelay({
  adb,
  expectedSsid: ssid,
  httpPort: localHttpPort,
  webSocketPort: localWebSocketPort,
});

async function selectMoto() {
  const devicesResult = await runProcess(adbPath, ["devices", "-l"], {
    echo: false,
    timeoutMs: 15000,
  });
  const devices = parseAdbDevices(devicesResult.stdout);
  adbSerial = selectAdbDevice(devices, process.env.FLEXFOX_ADB_SERIAL);
  log(`Moto ADB device: ${adbSerial}`);
}

async function readFirmwareStatus(timeoutMs = 5000) {
  const url = new URL("firmware/status", httpUrl);
  url.searchParams.set("cache", String(Date.now()));
  const response = await fetchWithTimeout(url, { cache: "no-store" }, timeoutMs);
  if (!response.ok) fail(`firmware status returned HTTP ${response.status}`);
  return response.json();
}

async function waitForFirmwareStatus(timeoutMs = 240000) {
  const deadline = Date.now() + timeoutMs;
  let lastError;
  while (Date.now() < deadline) {
    try {
      await waitForWifiAssociation(10000);
      await prepareRelays();
      return await readFirmwareStatus(5000);
    } catch (error) {
      lastError = error;
      await sleep(1500);
    }
  }
  fail(`firmware status did not return: ${lastError?.message ?? "timeout"}`);
}

async function runWithWifiRecovery(command, args, options) {
  let finished = false;
  let associationWasLost = false;
  const monitor = (async () => {
    while (!finished) {
      await sleep(1500);
      if (finished) break;
      try {
        const status = await readWifiStatus();
        if (!wifiStatusMatchesSsid(status, ssid)) {
          if (!associationWasLost) {
            associationWasLost = true;
            log(`Moto association with ${ssid} was lost; automatic reconnect active.`);
          }
          await requestWifiAssociation();
        } else if (associationWasLost) {
          associationWasLost = false;
          log(`Moto rejoined ${ssid}.`);
        }
      } catch {
        // The child command owns the operation result; this loop only assists reassociation.
      }
    }
  })();
  try {
    return await runProcess(command, args, options);
  } finally {
    finished = true;
    await monitor;
  }
}

function loadArtifacts() {
  const espBuildDir = join(repoRoot, "Software", "Huzzah", "tmp", "esp-build");
  const espPath = join(espBuildDir, "ARDF_Transmitter.ino.bin");
  const espEvidencePath = join(espBuildDir, "build-evidence.json");
  const espVersionHeaderPath = join(
    repoRoot,
    "Software",
    "Huzzah",
    "ARDF_Transmitter",
    "esp8266.h",
  );
  const avrManifestPath = join(
    repoRoot,
    "Software",
    "AVR128DA48",
    "tmp",
    "avr-boot-chain",
    "FlexFox80-AVR-Release-Info.json",
  );
  for (const requiredPath of [espPath, espEvidencePath, espVersionHeaderPath, avrManifestPath]) {
    if (!existsSync(requiredPath)) fail(`required release artifact is missing: ${requiredPath}`);
  }

  const espBytes = readFileSync(espPath);
  const espEvidence = JSON.parse(readFileSync(espEvidencePath, "utf8"));
  const espEvidenceEntry = espEvidence.artifacts?.find(
    (entry) => entry.file === basename(espPath),
  );
  const espSha256 = digest("sha256", espBytes);
  if (
    !espEvidenceEntry ||
    espEvidenceEntry.bytes !== espBytes.length ||
    espEvidenceEntry.sha256 !== espSha256
  ) {
    fail("ESP sketch does not match build-evidence.json");
  }
  const versionHeader = readFileSync(espVersionHeaderPath, "utf8");
  const espVersion = versionHeader.match(/#define\s+WIFI_SW_VERSION\s+\(\"([^\"]+)\"\)/)?.[1];
  if (!espVersion) fail("could not read the ESP source version");
  const installedMd5Values = new Set(
    esp8266SketchMd5Candidates(espBytes).map(({ md5 }) => md5),
  );

  const avrManifest = JSON.parse(readFileSync(avrManifestPath, "utf8"));
  if (
    avrManifest.format !== "flexfox-avr-update-v2" ||
    avrManifest.protocolVersion !== 2 ||
    !avrManifest.applicationVersion ||
    !avrManifest.bootloaderVersion
  ) {
    fail("AVR release manifest does not describe the permanent boot chain");
  }
  const initialInstallPath = resolve(dirname(avrManifestPath), avrManifest.initialInstallFile);
  if (!existsSync(initialInstallPath)) fail(`AVR first-install image is missing: ${initialInstallPath}`);

  const webRoot = join(repoRoot, "Software", "Huzzah", "ARDF_Transmitter", "data");
  const webFiles = fleetWebFiles.map((name) => {
    const path = join(webRoot, name);
    if (!existsSync(path)) fail(`fleet web file is missing: ${path}`);
    const content = readFileSync(path);
    return { name, path, content, bytes: content.length, sha256: digest("sha256", content) };
  });

  return {
    esp: {
      path: espPath,
      version: espVersion,
      bytes: espBytes.length,
      sha256: espSha256,
      installedMd5Values,
    },
    avr: {
      version: avrManifest.applicationVersion,
      bootloaderVersion: avrManifest.bootloaderVersion,
      manifestPath: avrManifestPath,
      initialInstallPath,
    },
    webFiles,
  };
}

async function runProbe(timeoutMs = 4000) {
  const result = await runProcess("just", ["wifi-probe"], {
    env: {
      ...process.env,
      FLEXFOX_URL: httpUrl,
      FLEXFOX_WEBSOCKET_URL: webSocketUrl,
      FLEXFOX_PROBE_TIMEOUT_MS: String(timeoutMs),
    },
    timeoutMs: timeoutMs + 15000,
  });
  return parseProbeReplies(result.stdout);
}

function verifyIdentity(replies, artifacts, requireCurrentVersions) {
  for (const name of ["SSID", "MAC", "SW_VERSIONS", "MASTER", "TEMP", "BAT"]) {
    if (!replies[name]) fail(`probe did not return ${name}`);
  }
  if (replies.SSID !== ssid) fail(`connected unit reports ${replies.SSID}, expected ${ssid}`);
  const expectedMaster = expectedMasterValue(ssid);
  if (replies.MASTER !== expectedMaster) {
    fail(`connected unit reports MASTER,${replies.MASTER}; expected ${expectedMaster} for ${ssid}`);
  }
  if (!probeTemperatureIsPlausible(replies.TEMP)) {
    fail(`probe temperature ${replies.TEMP} is malformed or outside the AVR sanity range`);
  }
  if (requireCurrentVersions) {
    const expectedVersions = `${artifacts.esp.version},${artifacts.avr.version}`;
    if (replies.SW_VERSIONS !== expectedVersions) {
      fail(`combined version is ${replies.SW_VERSIONS}; expected ${expectedVersions}`);
    }
  }
}

function probeReportsAvrVersion(replies, expectedVersion) {
  const versions = String(replies.SW_VERSIONS ?? "").split(",");
  return versions.length === 2 && versions[1] === expectedVersion;
}

async function runVerifiedProbe(artifacts, requireCurrentVersions, timeoutMs = 12000) {
  let lastError;
  for (let attempt = 1; attempt <= 3; attempt++) {
    const replies = await runProbe(timeoutMs);
    try {
      verifyIdentity(replies, artifacts, requireCurrentVersions);
      return { replies, attempts: attempt };
    } catch (error) {
      lastError = error;
      const detail = error instanceof Error ? error.message : String(error);
      if (!detail.includes("probe temperature") || attempt === 3) throw error;
      log(`WARN: ${detail}; retrying live telemetry (${attempt}/3).`);
    }
  }
  throw lastError;
}

async function remoteFileMatches(file) {
  try {
    const url = new URL(encodeURIComponent(file.name), httpUrl);
    url.searchParams.set("cache", String(Date.now()));
    const response = await fetchWithTimeout(url, { cache: "no-store" }, 10000);
    if (!response.ok) return false;
    const stored = Buffer.from(await response.arrayBuffer());
    return stored.length === file.bytes && digest("sha256", stored) === file.sha256;
  } catch {
    return false;
  }
}

async function deployWebFile(file) {
  let lastError;
  for (let attempt = 1; attempt <= 2; attempt++) {
    try {
      await runProcess("just", ["wifi-web-deploy"], {
        env: {
          ...process.env,
          FLEXFOX_URL: httpUrl,
          FLEXFOX_WEBSOCKET_URL: webSocketUrl,
          FLEXFOX_WEB_FILE: file.path,
          FLEXFOX_WEB_CONFIRM: "UPDATE FLEXFOX WEB FILE",
        },
        timeoutMs: 180000,
      });
      if (await remoteFileMatches(file)) return;
      lastError = new Error(`/${file.name} failed final readback verification`);
    } catch (error) {
      lastError = error;
    }

    log(`WARN: /${file.name} deployment response was interrupted; recovering the exact-SSID link.`);
    await waitForFirmwareStatus();
    if (await remoteFileMatches(file)) {
      log(`PASS: /${file.name} checksum matches despite the interrupted deployment response.`);
      return;
    }
    if (attempt === 1) log(`Retrying /${file.name} once after checksum mismatch.`);
  }
  throw lastError;
}

async function main() {
  writeFileSync(logPath, "");
  recordSummary();
  log(
    `FlexFox fleet ${dryRun ? "preflight" : wirelessOnly ? "wireless upgrade" : "upgrade"}: ` +
      `${unitId} (${ssid})`,
  );
  log(`Evidence: ${evidenceDir}`);

  const artifacts = loadArtifacts();
  summary.artifacts = {
    espVersion: artifacts.esp.version,
    espBytes: artifacts.esp.bytes,
    espSha256: artifacts.esp.sha256,
    avrVersion: artifacts.avr.version,
    bootloaderVersion: artifacts.avr.bootloaderVersion,
    avrManifest: artifacts.avr.manifestPath,
    webFiles: artifacts.webFiles.map(({ name, bytes, sha256 }) => ({ name, bytes, sha256 })),
  };
  recordSummary();

  await selectMoto();
  log(`Joining ${ssid} and preparing scoped ADB relays...`);
  await waitForWifiAssociation();
  await prepareRelays();
  const initialStatus = await waitForFirmwareStatus();
  if (initialStatus.filesystemProtected !== true) fail("ESP does not report LittleFS protection");
  if (
    initialStatus.cloneActive ||
    initialStatus.updateActive ||
    initialStatus.restartPending ||
    initialStatus.linkbusEventTransactionActive
  ) {
    fail("device is busy with a clone, update, restart, or event transaction");
  }
  log("Running read-only identity and telemetry probe...");
  const initialProbe = await runVerifiedProbe(artifacts, false);
  const initialReplies = initialProbe.replies;
  summary.identity = {
    ssid: initialReplies.SSID,
    mac: initialReplies.MAC,
    master: initialReplies.MASTER,
    versionsBefore: initialReplies.SW_VERSIONS,
    preflightTelemetryAttempts: initialProbe.attempts,
  };
  summary.phases.preflight = { result: "pass" };
  recordSummary();

  if (dryRun) {
    summary.phases.esp = {
      result: espStatusMatchesArtifact(initialStatus, artifacts.esp) ? "would-skip" : "would-update",
    };
    summary.phases.avr = {
      result:
        wirelessOnly && probeReportsAvrVersion(initialReplies, artifacts.avr.version)
          ? "would-skip-already-current"
          : wirelessOnly
            ? "would-update-through-resident-bootloader"
            : "would-verify-or-provision-with-atmel-ice",
    };
    summary.phases.web = {};
    for (const file of artifacts.webFiles) {
      summary.phases.web[file.name] = {
        result: (await remoteFileMatches(file)) ? "would-skip" : "would-update",
      };
    }
    summary.result = "preflight-pass";
    summary.completedAt = new Date().toISOString();
    recordSummary();
    log("PASS: read-only fleet upgrade preflight completed; no firmware or files were written.");
    return;
  }

  if (espStatusMatchesArtifact(initialStatus, artifacts.esp)) {
    summary.phases.esp = { result: "skipped-already-current" };
    log(`PASS: ESP ${artifacts.esp.version} exact image is already installed; skipping write.`);
  } else {
    log(`Updating ESP wirelessly to ${artifacts.esp.version}...`);
    await runWithWifiRecovery("just", ["wifi-esp-update"], {
      env: {
        ...process.env,
        FLEXFOX_URL: httpUrl,
        FLEXFOX_WEBSOCKET_URL: webSocketUrl,
        FLEXFOX_UPDATE_CONFIRM: "UPDATE FLEXFOX ESP",
      },
      timeoutMs: 420000,
    });
    const afterEsp = await waitForFirmwareStatus();
    if (!espStatusMatchesArtifact(afterEsp, artifacts.esp)) {
      fail("ESP updater returned without the exact release image installed");
    }
    summary.phases.esp = { result: "updated-and-verified" };
  }
  recordSummary();

  if (wirelessOnly && probeReportsAvrVersion(initialReplies, artifacts.avr.version)) {
    summary.phases.avr = { result: "skipped-already-current" };
    log(`PASS: AVR ${artifacts.avr.version} already reports as installed; skipping wireless rewrite.`);
  } else if (wirelessOnly) {
    log(
      `Updating AVR wirelessly through the resident ${artifacts.avr.bootloaderVersion} ` +
        `to ${artifacts.avr.version}...`,
    );
    await runWithWifiRecovery("just", ["wifi-avr-update"], {
      env: {
        ...process.env,
        FLEXFOX_HOST: `127.0.0.1:${localHttpPort}`,
        FLEXFOX_EXPECTED_DEVICE_SSID: ssid,
        FLEXFOX_AVR_SSID_SUFFIX: ssid.slice(-4),
        FLEXFOX_AVR_UPDATE_CONFIRM: `UPDATE-AVR-${artifacts.avr.version}`,
        FLEXFOX_ADB_SERIAL: adbSerial,
        FLEXFOX_ADB: adbPath,
      },
      timeoutMs: 35 * 60 * 1000,
    });
    summary.phases.avr = { result: "updated-wirelessly-and-verified" };
  } else {
    log(`Verifying or provisioning ${artifacts.avr.bootloaderVersion} + AVR ${artifacts.avr.version} with Atmel-ICE...`);
    const avrResult = await runProcess("just", ["avr-provision-boot-chain"], {
      env: {
        ...process.env,
        FLEXFOX_UNIT_ID: unitId,
        FLEXFOX_PROVISION_CONFIRM: "PROVISION-BOOTLOADER",
        FLEXFOX_FUSE_CONFIRM: "WRITE-BOOTSIZE-0x20",
        FLEXFOX_PROVISION_SKIP_IF_CURRENT: "1",
      },
      timeoutMs: 240000,
    });
    summary.phases.avr = {
      result: avrResult.stdout.includes("already matches the exact boot-chain image")
        ? "skipped-already-current"
        : "provisioned-and-verified",
    };
  }
  recordSummary();

  log(`Waiting for ${ssid} after the AVR restart...`);
  const afterAvrStatus = await waitForFirmwareStatus();
  if (!espStatusMatchesArtifact(afterAvrStatus, artifacts.esp)) {
    fail("ESP release image did not return after the AVR restart");
  }

  summary.phases.web = {};
  for (const file of artifacts.webFiles) {
    if (await remoteFileMatches(file)) {
      summary.phases.web[file.name] = { result: "skipped-already-current" };
      log(`PASS: /${file.name} already matches; skipping write.`);
    } else {
      log(`Deploying /${file.name}...`);
      await deployWebFile(file);
      summary.phases.web[file.name] = { result: "updated-and-verified" };
    }
    recordSummary();
  }

  log("Running final combined firmware and telemetry verification...");
  const finalProbe = await runVerifiedProbe(artifacts, true);
  const finalReplies = finalProbe.replies;
  const finalStatus = await readFirmwareStatus();
  if (!espStatusMatchesArtifact(finalStatus, artifacts.esp)) {
    fail("final firmware status does not match the exact ESP release image");
  }
  if (
    finalStatus.cloneActive ||
    finalStatus.updateActive ||
    finalStatus.restartPending ||
    finalStatus.linkbusEventTransactionActive
  ) {
    fail("final firmware status reports an active transaction");
  }
  summary.identity.versionsAfter = finalReplies.SW_VERSIONS;
  summary.identity.finalTelemetryAttempts = finalProbe.attempts;
  summary.phases.finalVerification = { result: "pass" };
  summary.result = "pass";
  summary.completedAt = new Date().toISOString();
  recordSummary();
  log(
    `PASS: ${unitId} is complete at ESP ${artifacts.esp.version}, ` +
      `${artifacts.avr.bootloaderVersion}, AVR ${artifacts.avr.version}.`,
  );
}

try {
  await main();
} catch (error) {
  summary.result = "failed";
  summary.completedAt = new Date().toISOString();
  summary.error = error instanceof Error ? error.message : String(error);
  try {
    recordSummary();
    logError(summary.error);
    logError(`Evidence retained at ${evidenceDir}`);
  } catch {
    process.stderr.write(`${summary.error}\n`);
  }
  process.exitCode = 2;
}
