# FlexFox80 v2.0.2 Release Record

**Status:** Released and independently verified

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Embedded versions:** AVR `0.210`, ESP `2.28`, resident AVR bootloader `BL0.3`

**Tagged release commit:** `1120ee63804bf6b69dcea89c1c17772236081556`

**GitHub release:** <https://github.com/OpenARDF/FlexFox80/releases/tag/v2.0.2>

## Release basis

This maintenance release keeps WiFi powered through a prolonged wireless AVR update handoff so a missed first handoff returns to browser status and retry instead of timing out. It includes the v2.0.1 manual-radio power/keying corrections and `radio.html` 0.86. The deferred post-standby voltage-freshness issue remains telemetry-only near-term roadmap work.

The complete 17-unit fleet ran the exact ESP `2.28`, AVR `0.210`, BL0.3, and web candidate before publication. The final reflash will deliberately reinstall the published images wirelessly on all 17 units and verify each unit from the durable release assets.

## Installation boundary

- Routine ESP and AVR fleet updates are wireless.
- Force the AVR application update during the final reflash even when a unit already reports `0.210`.
- New-unit or boot-chain recovery uses `FlexFox80-AVR-First-Install-0.210.hex` with Atmel-ICE/UPDI, `CODESIZE=0x00`, and `BOOTSIZE=0x20`.
- The first-install HEX does not write fuses or EEPROM.
- `FlexFox80-AVR-0.210.hex` is the application-only legacy-address image and does not contain BL0.3.
- Routine ESP updates write the sketch at `0x000000` and preserve LittleFS.
- The LittleFS image is factory/recovery material for `0x300000`; installing it replaces stored web assets, events, and settings.

See the [release checklist](release-checklist.json), [release notes](release-notes.md), [hardware disposition](hardware-disposition.md), [rollback record](rollback.md), and [GitHub verification evidence](../../Evidence/V2_0_2_GITHUB_RELEASE_VERIFICATION_2026-07-25.md).
