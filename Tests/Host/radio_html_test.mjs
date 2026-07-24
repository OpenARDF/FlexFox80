#!/usr/bin/env node

import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { dirname, join, resolve } from "node:path";
import { fileURLToPath } from "node:url";
import vm from "node:vm";

const repoRoot = resolve(dirname(fileURLToPath(import.meta.url)), "..", "..");
const radioHtmlPath = join(
  repoRoot,
  "Software",
  "Huzzah",
  "ARDF_Transmitter",
  "data",
  "radio.html",
);
const espSourcePath = join(
  repoRoot,
  "Software",
  "Huzzah",
  "ARDF_Transmitter",
  "ARDF_Transmitter.ino",
);
const avrSourcePath = join(
  repoRoot,
  "Software",
  "AVR128DA48",
  "FlexFox80",
  "main.cpp",
);

const html = readFileSync(radioHtmlPath, "utf8");
const scripts = [...html.matchAll(/<script(?:\s[^>]*)?>([\s\S]*?)<\/script>/gi)];
assert.equal(scripts.length, 1, "radio.html must contain exactly one inline script");
const source = scripts[0][1];

new vm.Script(source, { filename: radioHtmlPath });
assert.match(html, /radio\.html Version: 0\.86 - 24 Jul 2026/);
assert.match(html, /\.keybutton\s*\{[\s\S]*?touch-action:\s*none;/);
assert.match(source, /btn\.onpointerdown = keyPointerDownAction;/);
assert.match(source, /btn\.onpointerup = keyPointerUpAction;/);
assert.match(source, /btn\.onpointercancel = keyPointerUpAction;/);
assert.match(source, /btn\.ontouchstart = keyPointerDownAction;/);
assert.ok(!source.includes("btn.onmouseout"), "minor finger movement must not release the Key control");
assert.match(
  source,
  /function sendKeyDown\(\)[\s\S]*?var btext = "KEY_DOWN," \+ power;/,
  "every manual key-down must carry the currently selected power",
);

{
  const listeners = new Map();
  const sent = [];
  const attributes = new Map();
  const sockets = [];
  const timers = [];

  class FakeWebSocket {
    static OPEN = 1;

    constructor(url) {
      this.OPEN = FakeWebSocket.OPEN;
      this.readyState = 0;
      this.sent = [];
      this.url = url;
      sockets.push(this);
    }

    close() {
      this.readyState = 3;
      if (this.onclose) this.onclose({});
    }

    send(message) {
      this.sent.push(message);
    }
  }

  function createElement() {
    return {
      attributes: new Map(),
      children: [],
      appendChild(child) {
        this.children.push(child);
        return child;
      },
      setAttribute(name, value) {
        this.attributes.set(name, String(value));
        this[name] = String(value);
      },
    };
  }
  const sendButtons = Array.from({ length: 5 }, () => ({ disabled: false }));
  const elements = new Map([
    ["errorreport", { style: {} }],
    ["freq1", { value: 0 }],
    ["keyButton", {
      setAttribute(name, value) {
        attributes.set(name, value);
      },
      value: "Key",
    }],
    ["overlay", { style: {} }],
    ["overlayText", { innerHTML: "" }],
    ["pwrSelect", { value: "5000" }],
    ["statusreport", { style: {} }],
    ["temperature", { textContent: "" }],
    ["user1Text", { value: "MOE" }],
    ["xmtrTime", { innerHTML: "" }],
    ...sendButtons.map((button, index) => [`sendUserText${index + 1}`, button]),
  ]);
  const context = vm.createContext({
    Date,
    Math,
    Number,
    String,
    WebSocket: FakeWebSocket,
    clearInterval() {},
    clearTimeout() {},
    console: { log() {}, warn() {} },
    document: {
      createElement,
      createTextNode: (text) => ({ textContent: String(text) }),
      getElementById: (id) => elements.get(id) ?? null,
      querySelector: (selector) => elements.get(selector.replace(/^#/, "")) ?? null,
    },
    setTimeout(callback, delay) {
      const timer = { callback, delay, ran: false };
      timers.push(timer);
      return timers.length;
    },
    window: {
      addEventListener(type, listener) {
        const registered = listeners.get(type) ?? [];
        registered.push(listener);
        listeners.set(type, registered);
      },
      location: { hostname: "flexfox.test" },
    },
  });
  vm.runInContext(source, context, { filename: radioHtmlPath });
  const realSendToSocket = context.sendToSocket;
  context.sendToSocket = (message) => sent.push(message);

  const transmitterRow = context.eventRow({}, false);
  assert.equal(
    transmitterRow.children.length,
    5,
    "the complete transmitter row must render before saved strings are populated",
  );
  context.setManualTransmitControlsEnabled(true);

  let prevented = 0;
  let capturedPointer = null;
  const touchPointer = {
    button: 0,
    currentTarget: {
      setPointerCapture(pointerId) {
        capturedPointer = pointerId;
      },
    },
    pointerId: 7,
    preventDefault() {
      prevented++;
    },
  };

  context.keyPointerDownAction(touchPointer);
  context.keyPointerDownAction(touchPointer);
  assert.deepEqual(sent, ["KEY_DOWN,5000"], "one touch hold must emit one power-qualified key-down");
  assert.equal(capturedPointer, 7);
  assert.equal(attributes.get("style"), "background-color: #FF0000;");
  assert.equal(prevented, 2);

  listeners.get("pointerup")[0]({});
  listeners.get("mouseup")[0]({});
  assert.deepEqual(sent, ["KEY_DOWN,5000", "KEY_UP"], "release fallbacks must emit one key-up");
  assert.equal(attributes.get("style"), "background-color: #40A1FF;");

  context.sendString1Click();
  assert.equal(
    sent.at(-1),
    "PASS,$POW,M,5000;$KEY,MOE;",
    "Send must apply the selected power before queueing Morse text",
  );

  context.sendToSocket = realSendToSocket;
  context.webSocketStart();
  const firstSocket = sockets.at(-1);
  firstSocket.readyState = FakeWebSocket.OPEN;
  firstSocket.onopen({});

  assert.equal(elements.get("keyButton").disabled, true);
  assert.ok(
    sendButtons.every((button) => button.disabled),
    "manual transmit controls must stay disabled while selected power is unconfirmed",
  );
  context.keyDownAction();
  context.sendString1Click();
  assert.deepEqual(
    firstSocket.sent,
    [],
    "manual transmit actions must be ignored while power is unconfirmed",
  );

  const firstPowerTimer = timers.find((timer) => !timer.ran && timer.delay === 1500);
  assert.ok(firstPowerTimer, "each connection must schedule selected-power initialization");
  firstPowerTimer.ran = true;
  context.g_lastPacketTime = 0;
  firstPowerTimer.callback();
  assert.deepEqual(firstSocket.sent, ["POWER,0,5000"]);

  firstSocket.onmessage({ data: "POWER,4000" });
  assert.equal(
    elements.get("keyButton").disabled,
    true,
    "a different power report must not unlock manual transmission",
  );
  firstSocket.onmessage({ data: "POWER,5000" });
  assert.equal(elements.get("keyButton").disabled, false);
  assert.ok(
    sendButtons.every((button) => !button.disabled),
    "matching AVR power acknowledgment must unlock Key and Send",
  );

  context.webSocketStart();
  const secondSocket = sockets.at(-1);
  secondSocket.readyState = FakeWebSocket.OPEN;
  secondSocket.onopen({});
  assert.equal(elements.get("keyButton").disabled, true);

  firstSocket.onmessage({ data: "POWER,5000" });
  assert.equal(
    elements.get("keyButton").disabled,
    true,
    "a stale socket must not unlock controls on a newer connection",
  );

  const secondPowerTimer = timers.find((timer) => !timer.ran && timer.delay === 1500);
  assert.ok(secondPowerTimer, "a reconnect must schedule power initialization again");
  secondPowerTimer.ran = true;
  context.g_lastPacketTime = 0;
  secondPowerTimer.callback();
  assert.deepEqual(
    secondSocket.sent,
    ["POWER,0,5000"],
    "a reconnect must resend the selected power",
  );
  secondSocket.onmessage({ data: "POWER,5000" });
  assert.equal(elements.get("keyButton").disabled, false);
}

const espSource = readFileSync(espSourcePath, "utf8");
assert.match(
  espSource,
  /equalsIgnoreCase\(SOCK_COMMAND_KEYDOWN\)[\s\S]*?g_LBOutputBuff->put\(powerMsg\);[\s\S]*?g_LBOutputBuff->put\(msgOut\);/,
  "the ESP must queue manual power before key-down",
);

const avrSource = readFileSync(avrSourcePath, "utf8");
assert.match(
  avrSource,
  /bool powerChanged = \(pwr_mW != txGetPowerMw\(\)\);[\s\S]*?ec = txSetParameters\(&pwr_mW, NULL\);/,
  "the AVR must reapply requested power even when its cached value matches",
);
assert.match(
  avrSource,
  /if\(lastMorseCaller\(\) != CALLER_MANUAL_TRANSMISSIONS\)[\s\S]*?makeMorse\(\(char\*\)"\\0", &manualRepeat, null, CALLER_MANUAL_TRANSMISSIONS\);[\s\S]*?queuedText = \(i > 0\);/,
  "manual text must claim the Morse generator before it is queued",
);
assert.match(
  avrSource,
  /if\(queuedText\)[\s\S]*?g_enunciator = LED_AND_RF;[\s\S]*?powerToTransmitter\(ON\);/,
  "queued Send text must enable RF and final-stage power",
);
assert.match(
  avrSource,
  /if\(g_enunciator == LED_AND_RF\)[\s\S]*?powerToTransmitter\(OFF\);[\s\S]*?g_text_buff\.setBusy\(false\)/,
  "manual RF text must turn final-stage power off after completion",
);

console.log("PASS radio.html touch keying, reconnect power acknowledgment, Send RF, and manual-power ordering contracts");
