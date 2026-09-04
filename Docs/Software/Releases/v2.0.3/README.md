# FlexFox80 v2.0.3 Release Record

**Status:** Released and independently verified

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Embedded versions:** AVR `0.210`, ESP `2.35`, resident AVR bootloader `BL0.3`

**Tagged release commit:** `332d9a37fceeea1da192118616b803096090d704`

**GitHub release:** <https://github.com/OpenARDF/FlexFox80/releases/tag/v2.0.3>

## Release basis

This maintenance release makes ESP-to-ESP clone synchronization reliable during repeated fleet use. It adds bounded progress and reset recovery, atomic event replacement, strict command ownership, clock readback checks, stale-event clearing on identity changes, and improved fleet diagnostics. It uses the qualified LittleFS 2.5.1 engine while retaining ESP8266 core 2.7.4, WebSockets 2.3.6, the compatible LittleFS 2.0 disk format, AVR 0.210, BL0.3, and `radio.html` 0.86.

The exact ESP 2.35 candidate was installed on both masters and all 15 target units. All 14 hardware-capable targets completed full clone acceptance. The remaining target, November, passed image and filesystem checks but could not execute a clone because of its known Linkbus hardware fault.

## Installation boundary

- Routine ESP and AVR fleet updates are wireless.
- New-unit or boot-chain recovery uses `FlexFox80-AVR-First-Install-0.210.hex` with Atmel-ICE/UPDI, `CODESIZE=0x00`, and `BOOTSIZE=0x20`.
- The first-install HEX does not write fuses or EEPROM.
- `FlexFox80-AVR-0.210.hex` is the application-only legacy-address image and does not contain BL0.3.
- Routine ESP updates write the sketch at `0x000000` and preserve LittleFS.
- The LittleFS image is factory/recovery material for `0x300000`; installing it replaces stored web assets, events, and settings.

See the [release checklist](release-checklist.json), [release notes](release-notes.md), [hardware disposition](hardware-disposition.md), [rollback record](rollback.md), and [GitHub verification evidence](../../Evidence/V2_0_3_GITHUB_RELEASE_VERIFICATION_2026-09-03.md).
