# FlexFox80 v2.0.0 Release Record

**Status:** Released and independently verified

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Embedded versions:** AVR `0.210`, ESP `2.27`, resident AVR bootloader `BL0.3`

**Candidate source commit:** `6976f80e908526ccbbec44a19e897f72d82ac887`

**Tagged release commit:** `b3a05fee2dcf47d2ec6b99e92b4b0d5fc36d5556`

**GitHub release:** <https://github.com/OpenARDF/FlexFox80/releases/tag/v2.0.0>

**Asset amendment:** The release now includes `FlexFox80-AVR-First-Install-0.210.hex`, the combined BL0.3 plus relocated AVR 0.210 programmer image for new units.

## Release basis

This is the first approved FlexFox80 2.x product release and supersedes the withdrawn v1.0.0 record. It combines the corrected AVR sleep/wake scheduler, guarded wireless AVR and ESP maintenance, resilient ESP filesystem/file-update handling, startup-temperature validity, events-page reliability corrections, and repeatable fleet provisioning and verification tools.

All existing FlexFox hardware—15 foxes, one beacon, and one master/spectator—has been provisioned with ESP `2.27`, AVR `0.210`, and BL0.3. The fifteen uniquely named foxes have retained machine-readable final verification summaries reporting `2.27,0.210`; the release owner separately confirmed the beacon and master/spectator and reported completion of hardware testing.

## Installation boundary

- Routine AVR application updates may use BL0.3 through the guarded ESP workflow. Preserve unit EEPROM and verify the final combined version report.
- Initial bootloader installation and boot-chain recovery require Atmel-ICE/UPDI access. Use `FlexFox80-AVR-First-Install-0.210.hex`; it contains BL0.3 and the matching relocated AVR 0.210 application.
- The first-install HEX does not write fuses. The qualified setup requires `CODESIZE=0x00` and `BOOTSIZE=0x20`, plus EEPROM/fuse preservation and exact readback verification.
- `FlexFox80-AVR-0.210.hex` is the application-only legacy-address image. It does not contain BL0.3 and is not the normal choice for a new or BL0.3-equipped unit.
- Routine ESP updates replace only the sketch at address `0x000000` and preserve LittleFS.
- The published LittleFS image is factory/recovery material. Installing it at `0x300000` replaces stored web assets, events, and settings.
- The withdrawn AVR `0.201` image must not be used for scheduled operation.

See the [release checklist](release-checklist.json), [release notes](release-notes.md), [hardware disposition](hardware-disposition.md), and [recovery record](rollback.md).
