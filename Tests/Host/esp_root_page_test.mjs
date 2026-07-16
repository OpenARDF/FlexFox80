#!/usr/bin/env node

import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import vm from "node:vm";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const rootPagePath = join(
  repoRoot,
  "Software",
  "Huzzah",
  "ARDF_Transmitter",
  "RootPage.h",
);
const rootPageSource = readFileSync(rootPagePath, "utf8");
const firmware = readFileSync(
  join(repoRoot, "Software", "Huzzah", "ARDF_Transmitter", "ARDF_Transmitter.ino"),
  "utf8",
);
const pageMatch = rootPageSource.match(
  /static const char ROOT_PAGE_HTML\[\] PROGMEM = R"ROOTPAGE\(([\s\S]*?)\)ROOTPAGE";/,
);

assert.ok(pageMatch, "the firmware root page must remain a testable flash-resident raw string");
const html = pageMatch[1];
const scripts = [...html.matchAll(/<script(?:\s[^>]*)?>([\s\S]*?)<\/script>/gi)];
assert.equal(scripts.length, 1, "the firmware root page must contain one inline script");
assert.match(html, /href="\/events\.html"/);
assert.match(html, /href="\/upload\.html"/);
assert.match(html, /href="\/download\.html"/);
assert.match(html, /href="\/delete\.html"/);
assert.match(html, /href="\/firmware"/);
assert.ok(!html.includes('href="http://73.73.73.73'), "root-page links must follow the current device host");
assert.ok(
  !/href=.*73\.73\.73\.73/.test(firmware),
  "firmware-generated utility pages must use valid relative HOME links",
);

const status = { className: "", textContent: "" };
const sockets = [];
const timeouts = [];
const intervals = [];
const clearedIntervals = new Set();
const clearedTimeouts = new Set();
const listeners = new Map();
let nextTimerId = 1;

class FakeWebSocket {
  constructor(url) {
    this.readyState = 0;
    this.sent = [];
    this.url = url;
    sockets.push(this);
  }

  open() {
    this.readyState = 1;
    this.onopen?.({});
  }

  close() {
    this.readyState = 3;
    this.onclose?.({ code: 1000 });
  }

  send(message) {
    this.sent.push(message);
  }
}

const context = vm.createContext({
  WebSocket: FakeWebSocket,
  clearInterval(id) {
    clearedIntervals.add(id);
  },
  clearTimeout(id) {
    clearedTimeouts.add(id);
  },
  console: { log() {} },
  document: {
    getElementById(id) {
      return id === "connectionStatus" ? status : null;
    },
  },
  location: { hostname: "flexfox.test" },
  setInterval(callback, delay) {
    const timer = { callback, delay, id: nextTimerId++ };
    intervals.push(timer);
    return timer.id;
  },
  setTimeout(callback, delay) {
    const timer = { callback, delay, id: nextTimerId++ };
    timeouts.push(timer);
    return timer.id;
  },
  window: {
    addEventListener(type, listener) {
      listeners.set(type, listener);
    },
  },
});

const source = scripts[0][1];
new vm.Script(source, { filename: `${rootPagePath}:ROOT_PAGE_HTML` });
vm.runInContext(source, context, { filename: `${rootPagePath}:ROOT_PAGE_HTML` });

assert.equal(sockets.length, 1);
assert.equal(sockets[0].url, "ws://flexfox.test:81/");
assert.equal(status.textContent, "Connecting...");

sockets[0].open();
assert.equal(status.textContent, "Connected - keeping transmitter awake");
assert.deepEqual(sockets[0].sent, ["!&"]);
const heartbeat = intervals.find((timer) => timer.delay === 2000);
assert.ok(heartbeat, "the root page must send a two-second heartbeat");
heartbeat.callback();
assert.deepEqual(sockets[0].sent, ["!&", "!&"]);

sockets[0].close();
assert.equal(status.textContent, "Disconnected - retrying...");
const reconnect = timeouts.find((timer) => timer.delay === 2000);
assert.ok(reconnect, "the root page must retry a closed socket after two seconds");
reconnect.callback();
assert.equal(sockets.length, 2);

sockets[1].open();
const secondHeartbeat = intervals.at(-1);
sockets[1].close();
const secondReconnect = timeouts.at(-1);
listeners.get("pagehide")();
assert.ok(clearedIntervals.has(heartbeat.id));
assert.ok(clearedIntervals.has(secondHeartbeat.id));
assert.ok(clearedTimeouts.has(secondReconnect.id));

console.log("PASS firmware root page keeps the ESP awake and reconnects safely");
