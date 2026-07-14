# Mac ESP8266 Hardware-Compatible Development Baseline

**Date:** 2026-07-12

**Scope:** Checkpoint A2 and ESP integration for `B-TIME-01`

**Status:** Pinned build, standalone startup, installed WiFi/AVR path, and single-unit clone controls pass

## Decision boundary

The Adafruit HUZZAH setup procedure is the authoritative board-configuration source for FlexFox. The exact historical ESP8266 core was not recovered, but the mature standalone board's full flash was preserved and used as the rollback baseline. Characterization rejected ESP8266 core 3.x and established a hardware-compatible pinned development build. It does not claim byte identity with mature field firmware or full master-target clone qualification.

The isolated Mac profile uses:

- Arduino IDE 2.3.5 with bundled Arduino CLI 1.2.0;
- ESP8266 Arduino core 2.7.4;
- board FQBN `esp8266:esp8266:huzzah`;
- WebSockets 2.3.6;
- the core-bundled `mklittlefs` package `2.5.0-4-fe5bb56`;
- 80 MHz CPU, 115200 baud, 4 MB flash with the 1 MB filesystem layout, disabled debug port, no debug level, lwIP v2 Lower Memory, and Only Sketch erase.

The complete FQBN used by the wrapper is:

```text
esp8266:esp8266:huzzah:baud=115200,xtal=80,eesz=4M1M,dbg=Disabled,lvl=None____,ip=lm2f,wipe=none
```

## WebSockets compatibility finding

The historical Windows recovery found a vendored WebSockets 2.1.0 tree, but the checked-in 2022-era FlexFox source calls `connectedClients(false)` and `close()`, which are absent from 2.1.0. It also called a non-public `isRunning()` extension that is absent from every inspected published release.

A source-owned WebSocket-server lifecycle flag replaces that untracked extension. This is a compatibility correction, not a change to the intended start/close behavior.

Dependency and hardware trials established the pin:

- WebSockets 2.1.1 supplies the missing public methods but fails against ESP8266 core 3.1.2 because it calls the removed `WiFiClientSecure::verify()` API.
- WebSockets 2.7.2 compiles with core 3.1.2, but both pre-clone and clone-sync source repeatedly reset on the standalone HUZZAH and never advertised an SSID.
- Core 3.0.2 with WebSockets 2.3.6 also compiled but failed the same standalone startup gate.
- Core 2.7.4 with WebSockets 2.3.6 passed first with pre-clone source and then with the complete clone-sync source.

## Build results

Two independent clean compiles produced the same firmware binary:

| Measurement | Result |
| --- | --- |
| Firmware binary size | 503,392 bytes |
| Firmware SHA-256 | `3b6b5ad8e20d9662c9ee833f9c8072b955b27f61d895cc9dde95a3d13f4a796e` |
| Global/static RAM | 39,704 / 81,920 bytes (48%) |
| IRAM | 27,612 / 32,768 bytes (84%) |
| Flash code | 462,388 bytes IROM; 499,232 bytes total sketch use (47%) |
| Compiler warnings | 0 |

IRAM headroom is 5,156 bytes. Future ESP work must keep resource comparison as a required gate and avoid broad library or debug-option changes.

ELF and map hashes vary with the build directory; the flashable `.bin` was identical across independent build directories. LittleFS images are exactly 1,024,000 bytes (`0xFA000`), matching the selected partition boundaries. Repeated image creation can change the image hash while the checked-in data tree is unchanged, so the wrapper records each generated image hash but does not claim deterministic filesystem bytes.

## Repository workflow

Run:

```text
just esp-build
```

The wrapper installs the exact core and WebSockets dependency into the ignored `Software/Huzzah/tmp/arduino/` profile when needed. Set `ESP_ARDUINO_DATA_ROOT` to reuse another isolated profile, or `ARDUINO_CLI` when the executable is not on `PATH` and Arduino IDE is not installed at the standard Mac location.

Outputs and `build-evidence.json` are written under ignored `Software/Huzzah/tmp/esp-build/`. The wrapper builds the sketch and LittleFS image only; it never uploads either artifact.

## Hardware result and remaining qualification

- The standalone HUZZAH backup/programming/rollback path is qualified; see [Mac ESP8266 programming evidence](MAC_ESP8266_PROGRAMMING_2026-07-12.md).
- The final candidate passed standalone reset/SSID, independent flash verification, installed HTTP/WebSocket/AVR identity and telemetry, and live quiet/one-shot-edge/resume controls.
- Require a standalone reset/SSID smoke test before installing every future candidate.
- A second updated FlexFox is still required to exercise the complete master-target clone handshake, target RTC write/readback, cleanup failures, and repeated phase spread.
