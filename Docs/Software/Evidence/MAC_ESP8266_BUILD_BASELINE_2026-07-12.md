# Mac ESP8266 Qualified Build Baseline

**Date:** 2026-07-12

**Scope:** Checkpoint A2 and ESP integration for `B-TIME-01`

**Status:** Clean Mac build and LittleFS-image workflow qualified; no ESP was flashed

## Decision boundary

The Adafruit HUZZAH setup procedure is the authoritative board-configuration source for FlexFox. The exact historical ESP8266 core and deployed ESP binary were not recovered. This evidence therefore qualifies a new pinned migration baseline; it does not claim byte identity with firmware on mature field units.

The isolated Mac profile uses:

- Arduino IDE 2.3.5 with bundled Arduino CLI 1.2.0;
- ESP8266 Arduino core 3.1.2;
- board FQBN `esp8266:esp8266:huzzah`;
- WebSockets 2.7.2;
- the core-bundled `mklittlefs` package `3.1.0-gcc10.3-e5f9fec`;
- 80 MHz CPU, 115200 baud, 4 MB flash with the 1 MB filesystem layout, disabled debug port, no debug level, lwIP v2 Lower Memory, and Only Sketch erase.

The complete FQBN used by the wrapper is:

```text
esp8266:esp8266:huzzah:baud=115200,xtal=80,eesz=4M1M,dbg=Disabled,lvl=None____,ip=lm2f,wipe=none
```

## WebSockets compatibility finding

The historical Windows recovery found a vendored WebSockets 2.1.0 tree, but the checked-in 2022-era FlexFox source calls `connectedClients(false)` and `close()`, which are absent from 2.1.0. It also called a non-public `isRunning()` extension that is absent from every inspected published release.

A source-owned WebSocket-server lifecycle flag replaces that untracked extension. This is a compatibility correction, not a change to the intended start/close behavior.

Two dependency trials then established the pin:

- WebSockets 2.1.1 supplies the missing public methods but fails against ESP8266 core 3.1.2 because it calls the removed `WiFiClientSecure::verify()` API.
- WebSockets 2.7.2 compiles the complete sketch without warnings after the lifecycle correction.

## Build results

Two independent clean compiles produced the same firmware binary:

| Measurement | Result |
| --- | --- |
| Firmware binary size | 512,448 bytes |
| Firmware SHA-256 | `87e46f71595522434985f585ea543af075b034f6293e6e116f60cd53f6df257d` |
| Global/static RAM | 40,320 / 80,192 bytes (50%) |
| IRAM | 61,139 / 65,536 bytes (93%) |
| Flash code | 470,548 / 1,048,576 bytes (44%) |
| Compiler warnings | 0 |

IRAM headroom is only 4,397 bytes. Future ESP work must keep resource comparison as a required gate and avoid broad library or debug-option changes.

ELF and map hashes vary with the build directory; the flashable `.bin` was identical across independent build directories. LittleFS images are exactly 1,024,000 bytes (`0xFA000`), matching the selected partition boundaries. Repeated image creation can change the image hash while the checked-in data tree is unchanged, so the wrapper records each generated image hash but does not claim deterministic filesystem bytes.

## Repository workflow

Run:

```text
just esp-build
```

The wrapper installs the exact core and WebSockets dependency into the ignored `Software/Huzzah/tmp/arduino/` profile when needed. Set `ESP_ARDUINO_DATA_ROOT` to reuse another isolated profile, or `ARDUINO_CLI` when the executable is not on `PATH` and Arduino IDE is not installed at the standard Mac location.

Outputs and `build-evidence.json` are written under ignored `Software/Huzzah/tmp/esp-build/`. The wrapper builds the sketch and LittleFS image only; it never uploads either artifact.

## Remaining qualification

- Preserve or read back a known field ESP image if a reliable comparison path becomes available.
- The standalone HUZZAH backup and firmware-only programming path is now qualified; see [Mac ESP8266 programming evidence](MAC_ESP8266_PROGRAMMING_2026-07-12.md). LittleFS replacement remains deliberately unperformed.
- Run the clone quiet/edge/write/readback protocol on connected hardware, then compare multiple clone operations for phase spread.
