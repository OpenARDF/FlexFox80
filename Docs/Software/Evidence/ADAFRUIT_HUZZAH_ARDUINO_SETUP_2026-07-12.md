# Adafruit HUZZAH Arduino Setup Evidence

**Date:** 2026-07-12

**Scope:** ESP8266 build-profile recovery for Checkpoint A2

## Source and provenance

The project operator identified Adafruit's [Using Arduino IDE](https://learn.adafruit.com/adafruit-huzzah-esp8266-breakout/using-arduino-ide) instructions as the procedure used to set up the FlexFox ESP board. An attached 40-page export of the full Adafruit guide was reviewed visually and by text extraction:

- filename: `adafruit-huzzah-esp8266-breakout.pdf`;
- PDF SHA-256: `88a7943a8ebba624fcc57d9f6b67775519943fe117645a9c7a7293737151e6eb`;
- guide export timestamp: 2024-07-16;
- the PDF remains outside Git and is identified by hash only.

This evidence is stronger than a guessed modern configuration, but it is still a setup-instruction baseline rather than a recovered FlexFox `build.options.json` or compile command. Values shown only in the guide's Tools-menu screenshots are labeled accordingly.

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

## Remaining unknowns

- exact ESP8266 Arduino core version;
- exact FQBN spelling for that core version, although the board menu identity is known;
- whether every screenshot-visible default was retained in the final FlexFox build;
- exact LittleFS image-generation and upload tool/version;
- a retained ESP binary/filesystem image or cached compile command for comparison.

Known dependency evidence remains `arduinoWebSockets` 2.1.0 from the Windows historical-source recovery. Together, the Adafruit setup instructions and Windows discovery now leave the ESP8266 core version and LittleFS-era tooling as the principal reproducibility gaps.
