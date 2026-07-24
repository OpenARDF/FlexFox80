# FlexFox80 v2.0.1 Release Record

**Status:** Released and independently verified

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Embedded versions:** AVR `0.210`, ESP `2.27`, resident AVR bootloader `BL0.3`

**Tagged release commit:** `8a23b565821bbd27dca414b8b93809bd9ccc044c`

**GitHub release:** <https://github.com/OpenARDF/FlexFox80/releases/tag/v2.0.1>

## Release basis

This maintenance release corrects manual radio power initialization and keying. It applies the selected RF power before key-down, makes manual text claim the Morse generator and enable RF, waits for the AVR power acknowledgement before enabling browser transmit controls, reliably releases touch/pointer keying, and prevents stale reconnect work from overriding the active socket.

The product tag changed to `v2.0.1`; the embedded strings remain ESP `2.27` and AVR `0.210`. Operators must use the v2.0.1 manifest, source commit, and SHA-256 checksums to distinguish these images from v2.0.0.

## Installation boundary

- Routine AVR application updates may use BL0.3 through the guarded ESP workflow.
- New-unit or boot-chain recovery uses `FlexFox80-AVR-First-Install-0.210.hex` with Atmel-ICE/UPDI, `CODESIZE=0x00`, and `BOOTSIZE=0x20`.
- The first-install HEX does not write fuses or EEPROM.
- `FlexFox80-AVR-0.210.hex` is the application-only legacy-address image and does not contain BL0.3.
- Routine ESP updates write the sketch at `0x000000` and preserve LittleFS.
- The LittleFS image is factory/recovery material for `0x300000`; installing it replaces stored web assets, events, and settings.

See the [release checklist](release-checklist.json), [release notes](release-notes.md), [hardware disposition](hardware-disposition.md), [rollback record](rollback.md), and [GitHub verification evidence](../../Evidence/V2_0_1_GITHUB_RELEASE_VERIFICATION_2026-07-24.md).
