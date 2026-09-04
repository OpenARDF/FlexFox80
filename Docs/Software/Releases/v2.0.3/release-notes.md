# FlexFox80 v2.0.3

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Firmware:** ESP `2.35`, AVR `0.210` (`SW_VERSIONS,2.35,0.210`)

**Resident bootloader:** `BL0.3`

**Source:** `332d9a37fceeea1da192118616b803096090d704`

## Changes since v2.0.2

- Make ESP-to-ESP cloning reliable under repeated, full-fleet use with bounded progress monitoring, reset recovery, atomic event replacement, and strict command ownership.
- Use the qualified LittleFS 2.5.1 engine while preserving the ESP8266 core 2.7.4, WebSockets 2.3.6, and LittleFS disk-format compatibility.
- Clear stale event data when a transmitter changes identity, and retain checksum validation for cloned event files.
- Improve clone diagnostics, clock readback verification, wrap-safe timing, cleanup, and fleet update/probe tooling.

## Verification

- The full repository suite and complete-history secret scan pass.
- Two pinned AVR-GCC 7.3.0 / AVR-Dx_DFP 1.9.103 build passes are warning-free and byte-identical.
- Two pinned ESP8266 core 2.7.4 / WebSockets 2.3.6 / LittleFS 2.5.1 build passes are warning-free and byte-identical.
- Both masters and all 15 target units received the exact ESP 2.35 candidate. All 14 hardware-capable targets completed full clone acceptance; one unit with a known Linkbus hardware fault passed image/filesystem checks but could not execute a clone.
- The release retains AVR 0.210, BL0.3, and `radio.html` 0.86.

Routine ESP updates write the sketch at `0x000000` and preserve LittleFS. The LittleFS image is factory/recovery material for `0x300000`; installing it replaces stored web assets, events, and settings.
