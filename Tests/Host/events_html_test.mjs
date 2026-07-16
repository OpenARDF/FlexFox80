#!/usr/bin/env node

import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import vm from "node:vm";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const eventsHtmlPath = join(
  repoRoot,
  "Software",
  "Huzzah",
  "ARDF_Transmitter",
  "data",
  "events.html",
);
const html = readFileSync(eventsHtmlPath, "utf8");
const scripts = [...html.matchAll(/<script(?:\s[^>]*)?>([\s\S]*?)<\/script>/gi)];

assert.equal(scripts.length, 1, "events.html must contain exactly one inline script");
const source = scripts[0][1];

class FakeElement {
  constructor(id = "") {
    this.id = id;
    this.attributes = new Map();
    this.checked = false;
    this.children = [];
    this.disabled = false;
    this.options = [];
    this.selected = false;
    this.selectedIndex = -1;
    this.style = {};
    this.textContent = "";
    this.value = "";
  }

  appendChild(child) {
    this.children.push(child);
    return child;
  }

  setAttribute(name, value) {
    this.attributes.set(name, String(value));
    if (name === "value") this.value = String(value);
    if (name === "selected") this.selected = true;
  }
}

function makeSelect(values, selectedIndex = 0) {
  const select = new FakeElement("roleSelect");
  select.options = values.map((value, index) => {
    const option = new FakeElement();
    option.value = value;
    option.selected = index === selectedIndex;
    select[index] = option;
    return option;
  });
  select.length = select.options.length;
  select.selectedIndex = selectedIndex;
  Object.defineProperty(select, "value", {
    configurable: true,
    get() {
      return this.options[this.selectedIndex]?.value ?? "";
    },
    set(value) {
      const index = this.options.findIndex((option) => option.value === value);
      if (index >= 0) this.selectedIndex = index;
    },
  });
  return select;
}

function createPage() {
  const elements = new Map();
  const listeners = new Map();
  const timeouts = [];
  const intervals = [];
  const clearedIntervals = new Set();
  const clearedTimeouts = new Set();
  const sockets = [];

  class FakeWebSocket {
    static OPEN = 1;

    constructor(url) {
      this.OPEN = FakeWebSocket.OPEN;
      this.readyState = FakeWebSocket.OPEN;
      this.sent = [];
      this.url = url;
      sockets.push(this);
    }

    close() {
      this.readyState = 3;
      this.onclose?.({ code: 1000 });
    }

    send(message) {
      if (this.throwOnSend) throw new Error("simulated socket send failure");
      this.sent.push(message);
    }
  }

  let nextTimerId = 1;
  const document = {
    createElement: () => new FakeElement(),
    createTextNode: (text) => ({ textContent: String(text) }),
    getElementById: (id) => elements.get(id) ?? null,
    querySelector: (selector) => {
      if (selector.startsWith("#")) return elements.get(selector.slice(1)) ?? null;
      if (selector === ".freq1val") return elements.get("freq1val") ?? null;
      return null;
    },
  };
  const window = {
    location: { hostname: "flexfox.test" },
    addEventListener(type, listener) {
      const registered = listeners.get(type) ?? [];
      registered.push(listener);
      listeners.set(type, registered);
    },
  };
  const context = vm.createContext({
    Date,
    Math,
    Number,
    String,
    WebSocket: FakeWebSocket,
    clearInterval(id) {
      clearedIntervals.add(id);
    },
    clearTimeout(id) {
      clearedTimeouts.add(id);
    },
    console: { log() {}, warn() {} },
    document,
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
    window,
  });
  vm.runInContext(source, context, { filename: eventsHtmlPath });

  function addElement(id, element = new FakeElement(id)) {
    elements.set(id, element);
    return element;
  }

  for (const id of ["currentTime", "overlay", "overlayText", "xmtrTime"]) addElement(id);

  return {
    addElement,
    clearedIntervals,
    clearedTimeouts,
    context,
    elements,
    intervals,
    listeners,
    sockets,
    timeouts,
  };
}

function lastTimerWithDelay(timers, delay) {
  const matches = timers.filter((timer) => timer.delay === delay);
  assert.ok(matches.length > 0, `expected a ${delay}ms timer`);
  return matches.at(-1);
}

new vm.Script(source, { filename: eventsHtmlPath });
assert.ok(!html.includes('href=\\ "73.73.73.73"'), "home link must use valid HTML syntax");
assert.ok(!source.includes("Date.getTime()"), "date fallback must call getTime on a Date instance");
assert.match(html, /<html lang="en">/);
assert.match(html, /<meta charset="utf-8">/);
assert.match(html, /<meta name="viewport" content="width=device-width, initial-scale=1">/);
assert.match(html, /events\.html Version: 0\.5\.4 - 16 Jul 2026/);
assert.match(html, /Transmitter assignment \(\.me\) files are always preserved\./);
assert.ok(!source.includes('btn.id = "runButton"'), "event selection buttons must not reuse an id");
assert.equal(
  [...source.matchAll(/setAttribute\("id", "datetimeFinishCell"\)/g)].length,
  1,
  "the callsign cell must not duplicate the finish-cell id",
);
assert.match(
  source,
  /\/\/ Finish Time[\s\S]*?setAttribute\("id", "datetimeFinishCell"\)[\s\S]*?\/\/ Call Sign[\s\S]*?setAttribute\("id", "callSignCell"\)/,
  "finish and callsign controls must retain their semantic ids",
);
assert.ok(!source.includes("while (d == n)"), "clock synchronization must not busy-wait on the UI thread");
console.log("PASS events.html JavaScript and critical HTML syntax parse");

{
  const page = createPage();
  const overlay = page.elements.get("overlay");
  const overlayText = page.elements.get("overlayText");
  const transmitterTime = page.elements.get("xmtrTime");

  page.context.start();
  const firstSocket = page.sockets.at(-1);
  firstSocket.onopen({});
  assert.equal(overlay.style.display, "block");
  assert.equal(overlayText.textContent, "Connected\nLoading Events...");
  assert.equal(transmitterTime.textContent, "Connected - Loading events...");

  page.context.g_eventSheetLoaded = true;
  page.context.g_timedOut = true;
  firstSocket.close();
  assert.equal(overlay.style.display, "block");
  assert.equal(overlayText.textContent, "Disconnected\nRetrying...");
  assert.equal(transmitterTime.textContent, "Disconnected");

  lastTimerWithDelay(page.intervals, 2000).callback();
  assert.equal(page.sockets.length, 1, "offline keep-alives must wait for the bounded reconnect timer");
  lastTimerWithDelay(page.timeouts, 2000).callback();
  assert.equal(page.sockets.length, 2, "the keep-alive must retain automatic reconnect behavior");
  const reconnectedSocket = page.sockets.at(-1);
  reconnectedSocket.onopen({});
  assert.equal(overlay.style.display, "none", "cached data must be visible as soon as the socket reconnects");
  assert.equal(transmitterTime.textContent, "Connected - Refreshing events...");
  assert.equal(page.context.g_timedOut, false);
  assert.ok(page.context.g_stillConnected > 0);
  console.log("PASS reconnect updates the page shade immediately while cached events refresh");
}

{
  const page = createPage();
  page.context.start();
  const firstSocket = page.sockets.at(-1);
  firstSocket.onopen({});
  const heartbeat = page.intervals.find((timer) => timer.delay === 2000);
  assert.ok(heartbeat);

  firstSocket.close();
  const reconnectTimers = page.timeouts.filter((timer) => timer.delay === 2000);
  assert.equal(reconnectTimers.length, 1, "a closed socket must schedule one bounded reconnect");
  heartbeat.callback();
  assert.equal(page.sockets.length, 1, "a heartbeat must not create a competing socket");
  assert.equal(
    page.timeouts.filter((timer) => timer.delay === 5000).length,
    0,
    "heartbeats must not accumulate delayed retry callbacks while offline",
  );

  reconnectTimers[0].callback();
  assert.equal(page.sockets.length, 2);
  const secondSocket = page.sockets.at(-1);
  secondSocket.onopen({});
  firstSocket.onclose?.({ code: 1006 });
  assert.equal(page.context.g_websock, secondSocket, "a stale close must not clear the current socket");

  page.listeners.get("pagehide")[0]({});
  assert.equal(page.context.g_websock, null);
  page.listeners.get("pageshow")[0]({ persisted: true });
  assert.equal(page.sockets.length, 3, "a page restored from browser cache must reconnect");
  console.log("PASS WebSocket retries stay bounded and stale sockets cannot win races");
}

{
  const page = createPage();
  page.context.start();
  const socket = page.sockets.at(-1);
  socket.onopen({});
  socket.throwOnSend = true;
  page.context.g_lastPacketTime = 0;

  assert.doesNotThrow(() => page.context.sendToSocket("SSID"));
  assert.equal(page.context.g_websock, null);
  assert.equal(page.timeouts.filter((timer) => timer.delay === 2000).length, 1);
  assert.equal(page.timeouts.filter((timer) => timer.delay === 5000).length, 1);
  console.log("PASS a socket send race falls back to bounded reconnect and command retry");
}

{
  const page = createPage();
  page.context.start();
  assert.ok(
    page.intervals.some((timer) => timer.delay === 1000),
    "the page must refresh displayed time once per second",
  );

  const socket = page.sockets.at(-1);
  socket.onopen({});
  assert.equal(socket.sent[0], "EVENT_NAME,NEW!", "event loading must be the first socket request");
  page.context.g_lastPacketTime = 0;
  lastTimerWithDelay(page.timeouts, 1000).callback();
  assert.equal(socket.sent.at(-1), "CLONE_PRUNE", "startup must query the firmware's current clone option");
  console.log("PASS clock display cadence and event-first startup scheduling");
}

{
  const page = createPage();
  const sent = [];
  const checkbox = page.addElement("clonePruneTargetEvents");
  page.context.sendToSocket = (message) => sent.push(message);

  checkbox.checked = true;
  page.context.setClonePruneTargetEvents();
  assert.equal(sent.at(-1), "CLONE_PRUNE,1");
  checkbox.checked = false;
  page.context.setClonePruneTargetEvents();
  assert.equal(sent.at(-1), "CLONE_PRUNE,0");

  page.context.webSocketStart();
  const socket = page.sockets.at(-1);
  socket.onmessage({ data: "CLONE_PRUNE,1" });
  assert.equal(checkbox.checked, true);
  socket.onmessage({ data: "CLONE_PRUNE,0" });
  assert.equal(checkbox.checked, false);
  console.log("PASS clone cleanup remains explicit, opt-in, and synchronized with firmware state");
}

{
  const page = createPage();
  const sent = [];
  let renderCount = 0;
  page.addElement("freqSet1");
  page.context.sendToSocket = (message) => sent.push(message);
  page.context.displaySelectedEvent = () => renderCount++;
  page.context.updateTextFormatting = () => {};
  page.context.g_eventTableData = [
    {
      name: "Event",
      version: "Ver.",
      start: "Start Time",
      finish: "Finish Time",
      role: "Fox",
      callsign: "Call",
      power: "Power",
      frequency: "Freq",
    },
    {
      name: "Alpha",
      version: "1.0",
      start: 1_000,
      finish: 2_000,
      role: "Fox 1",
      callsign: "N0CALL",
      power: "100",
      freq: "3_550_000",
    },
    {
      name: "Beta",
      version: "1.1",
      start: 3_000,
      finish: 4_000,
      role: "Finish",
      callsign: "N0CALL",
      power: "300",
      freq: "3_600_000",
    },
  ];
  page.context.g_selectedEvent = "Alpha";
  page.context.g_eventSheetLoaded = true;
  page.context.webSocketStart();
  const socket = page.sockets.at(-1);

  page.context.eventSelect("Beta");
  assert.equal(sent.at(-1), "EVENT_NAME,Beta", "legacy firmware must retain full-refresh selection");
  page.context.g_selectedEvent = "Alpha";
  socket.onmessage({ data: "EVENT_CACHE,1" });
  page.context.eventSelect("Beta");
  assert.equal(sent.at(-1), "EVENT_SELECT,Beta");

  socket.onmessage({ data: "EVENT_NAME,Beta" });
  assert.equal(page.context.g_eventTableData.length, 3, "selection must preserve the cached sheet");

  socket.onmessage({
    data: "EVENT_DATA,Beta,1.1,3,4,Finish - MO,N0CALL,3000,3600000",
  });
  assert.equal(page.context.g_eventTableData.length, 3, "fallback refresh must update, not duplicate");
  assert.equal(page.context.g_eventTableData[2].role, "Finish - MO");
  assert.equal(page.context.g_eventTableData[2].power, "3000");
  assert.equal(page.context.g_eventTableData[2].freq, "3600000");

  socket.onmessage({ data: "TYPE_NAME,Done,Done" });
  assert.equal(renderCount, 1);
  assert.equal(page.context.g_eventSheetLoaded, true);
  console.log("PASS cached event sheet survives row selection and summary refreshes");
}

{
  const page = createPage();
  const sent = [];
  const roleSelect = makeSelect(["Fox 1", "Fox 2", "Fox 3"], 0);
  page.elements.set("roleSelect", roleSelect);
  page.context.g_typeTxNames = [
    { name: "Fox 1", indices: "0:0" },
    { name: "Fox 2", indices: "0:1" },
    { name: "Fox 3", indices: "1:0" },
  ];
  page.context.sendToSocket = (message) => sent.push(message);
  page.context.webSocketStart();
  const socket = page.sockets.at(-1);

  socket.onmessage({ data: "TX_ROLE,1:0" });
  assert.equal(roleSelect.selectedIndex, 2);
  assert.equal(page.context.currentRoleIndex(), 1);
  assert.deepEqual(sent, ["FREQ,1", "POWER,1"]);

  assert.doesNotThrow(() => socket.onmessage({ data: "TX_ROLE,9:9" }));
  assert.equal(roleSelect.selectedIndex, 2, "unknown role must preserve the current selection");
  console.log("PASS role selection uses stable select properties and bounds checks");
}

{
  const page = createPage();
  const status = page.addElement("statusreport");
  page.context.webSocketStart();
  const socket = page.sockets.at(-1);

  assert.doesNotThrow(() => socket.onmessage({ data: "SUS" }));
  assert.doesNotThrow(() => socket.onmessage({ data: "SUE" }));
  assert.doesNotThrow(() => socket.onmessage({ data: "FREQ" }));
  assert.doesNotThrow(() => socket.onmessage({ data: "POWER" }));
  socket.onmessage({ data: "SUS,Fox 1 - MOE,Classic 80m Set 1-1" });
  assert.equal(status.textContent, "Success: Fox 1 - MOE synced. Next up: Classic 80m");
  assert.doesNotMatch(status.textContent, /Set 1-1/);
  socket.onmessage({ data: "SUS,Fox 3 - MOE,Sprint3 80m" });
  assert.equal(status.textContent, "Success: Fox 3 - MOE synced. Next up: Sprint3 80m");
  socket.onmessage({ data: "SUE,Checksum mismatch" });
  assert.equal(status.textContent, "Sync Error: Checksum mismatch");
  console.log("PASS clone monitoring tolerates incomplete status frames");
}

{
  const page = createPage();
  const sent = [];
  page.context.sendToSocket = (message) => sent.push(message);
  page.context.g_stillConnected = 50;
  page.addElement("datetimeStart").value = "";
  page.addElement("datetimeFinish").value = "";

  page.context.newStartTime();
  assert.doesNotThrow(() => lastTimerWithDelay(page.timeouts, 750).callback());
  assert.ok(sent.some((message) => /^START_TIME,\d{4}-\d{2}-\d{2}T/.test(message)));

  page.context.newFinishTime();
  assert.doesNotThrow(() => lastTimerWithDelay(page.timeouts, 750).callback());
  assert.ok(sent.some((message) => /^FINISH_TIME,\d{4}-\d{2}-\d{2}T/.test(message)));
  console.log("PASS cleared or invalid datetime fields fall back without throwing");
}

{
  const page = createPage();
  const sent = [];
  const roleSelect = makeSelect(["Fox"], 0);
  page.elements.set("roleSelect", roleSelect);
  page.addElement("frequencyButton").value = "4.000.000";
  page.addElement("freq1Text");
  page.addElement("freq1").value = 0;
  page.addElement("freq1val");
  page.context.g_typeTxNames = [{ name: "Fox", indices: "0:0" }];
  page.context.g_typeFrequency = [{ frequency: 4_000_000, role: "Fox" }];
  page.context.g_eventTableData = [
    { name: "Event" },
    { name: "Fox Event", role: "Fox", power: "100", freq: "4000000" },
  ];
  page.context.g_selectedEvent = "Fox Event";
  page.context.g_eventRadioBand = "80m";
  page.context.g_freq1FrequencyHz = 4_000_000;
  page.context.sendToSocket = (message) => sent.push(message);

  page.context.incFreq1Click();
  assert.equal(page.context.g_typeFrequency[0].frequency, 4_000_000);
  assert.equal(sent.at(-1), "FREQ,0,4000000");

  page.elements.get("freq1").value = 100;
  page.context.g_freq1FrequencyHz = 3_550_000;
  page.context.freq1Change();
  const adjustmentTimer = page.intervals.at(-1);
  adjustmentTimer.callback();
  assert.equal(page.listeners.get("touchcancel")?.length, 1);
  assert.equal(page.listeners.get("pointercancel")?.length, 1);
  assert.equal(page.listeners.get("blur")?.length, 1);
  page.listeners.get("blur")[0]();

  assert.ok(page.clearedIntervals.has(adjustmentTimer.id));
  assert.equal(page.context.g_timer, 0);
  assert.equal(sent.at(-1), "FREQ,0,3550100");
  assert.equal(page.context.g_eventTableData[1].freq, "3550100");
  console.log("PASS frequency limits and final release value are saved exactly");
}

{
  const page = createPage();
  const transmitterTime = page.elements.get("xmtrTime");
  const base = Date.UTC(2026, 6, 15, 12, 0, 0);
  page.context.g_transmitterTimeMs = base;
  page.context.g_transmitterTimeReceivedMs = Date.now() - 1000;
  page.context.updateDisplayedTransmitterTime();
  assert.equal(transmitterTime.textContent, `TX: ${new Date(base + 1000)}`);
  console.log("PASS transmitter display advances between two-second SYNC replies");
}

{
  const page = createPage();
  const sent = [];
  page.context.sendToSocket = (message) => sent.push(message);

  page.context.syncClock();
  const firstSync = page.timeouts.at(-1);
  assert.ok(firstSync.delay > 0 && firstSync.delay <= 1000);
  assert.deepEqual(sent, [], "clock synchronization must return without blocking or sending early");

  page.context.syncClock();
  const secondSync = page.timeouts.at(-1);
  assert.ok(page.clearedTimeouts.has(firstSync.id), "a repeated sync click must replace the pending sync");
  secondSync.callback();
  assert.match(sent.at(-1), /^SYNC,\d{4}-\d{2}-\d{2}T/);
  assert.equal(page.context.g_clockSyncTimer, 0);
  console.log("PASS clock synchronization waits for the next second without blocking the UI");
}
