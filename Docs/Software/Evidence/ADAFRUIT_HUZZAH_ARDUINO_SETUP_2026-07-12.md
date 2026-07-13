# Adafruit HUZZAH Arduino Setup Evidence

**Date:** 2026-07-12

**Scope:** ESP8266 build-profile recovery for Checkpoint A2

## Source and provenance

The project operator identified Adafruit's [Using Arduino IDE](https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide) instructions as the procedure used to set up the FlexFox ESP board. An attached 40-page export of the full Adafruit guide was reviewed visually and by text extraction:

- filename: `adafruit-huzzah-esp8266-breakout.pdf`;
- PDF SHA-256: `88a7943a8ebba624fcc57d9f6b67775519943fe117645a9c7a7293737151e6eb`;
- guide export timestamp: 2024-07-16;
- the PDF remains outside Git and is identified by hash only.

The operator confirmed that Arduino IDE recognizes the product and that this procedure describes how the IDE was configured for programming the HUZZAH. It is therefore the authoritative FlexFox board-configuration baseline. It is not, by itself, evidence of the exact historical core/library versions or a deployed binary.

## Recovered setup profile

| Setting | Recovered value | Evidence strength |
| --- | --- | --- |
| Board-manager URL | `http://arduino.esp8266.com/stable/package_esp8266com_index.json` | Explicit guide instruction |
| Board menu selection | `Adafruit Feather HUZZAH ESP8266` | Explicit guide instruction; the guide says to use this definition for the breakout |
| CPU frequency | `80 MHz` | Explicit guide instruction and separately operator-confirmed |
| Upload speed | `115200` baud | Explicit guide recommendation in the procedure used by the project |
| Serial port | Matching FTDI/USB-serial port | Explicit guide instruction; machine-specific value is intentionally not pinned |
| Programmer | Not used; serial upload always applies | Explicit guide instruction |
| Debug level | `None` | Selected value visible in the guide screenshot |
| Debug port | `Disabled` | Selected value visible in the guide screenshot |
| Erase flash | `Only Sketch` | Selected value visible in the guide screenshot |
| Flash size/layout | `4M (1M SPIFFS)` | Selected value visible in the historical guide screenshot |
| lwIP variant | `v2 Lower Memory` | Selected value visible in the guide screenshot |

The guide also specifies manual bootloader entry before upload: hold GPIO0, press RESET, release RESET, then release GPIO0. Upload uses the selected serial port; after the IDE reports a hard reset via RTS, the guide instructs the operator to press RESET to run the sketch.

## Filesystem qualification boundary

The guide's historical Tools-menu screenshot labels the one-megabyte filesystem partition as `SPIFFS`. Current FlexFox source includes `LittleFS.h` and calls LittleFS APIs. The recovered evidence therefore supports a four-megabyte flash with a one-megabyte filesystem allocation, but it does **not** identify the ESP8266 core version where the project migrated from SPIFFS labeling/tooling to LittleFS, nor the exact filesystem image uploader used for the deployed source.

Do not substitute a SPIFFS image workflow for the current LittleFS source merely because the guide screenshot uses that older label. The core version and compatible LittleFS image command remain required before Checkpoint A2 can close.

## Repeatable Mac migration candidate

The recovered configuration has now been implemented in the pinned `just esp-build` workflow using ESP8266 core 3.1.2, FQBN `esp8266:esp8266:huzzah`, WebSockets 2.7.2, and the core-bundled LittleFS image tool. The sketch builds cleanly and repeatably to the same flashable binary on Mac. See [Mac ESP8266 build baseline](MAC_ESP8266_BUILD_BASELINE_2026-07-12.md).

This is not a qualified hardware baseline: the first installed image failed to provide an SSID and was rolled back exactly. Historical WebSockets 2.1.0 cannot compile the checked-in source, and 2.1.1 is incompatible with the selected current core. The repeatable candidate remains useful for controlled characterization that separates toolchain/library/filesystem migration from source behavior.
