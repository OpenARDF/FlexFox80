# FlexFox80 v2.0.0 Recovery and Rollback Record

**Selection date:** 2026-07-20

**Approved by:** Charles Scharlau

## Supported recovery point

There is no approved earlier public firmware release to which a v2.0.0 unit should be downgraded. The v1.0.0 GitHub release was withdrawn because AVR `0.201` is unsafe for scheduled operation. Recovery therefore means restoring the exact v2.0.0 application images while preserving the affected unit's own configuration.

The public recovery set is:

- `FlexFox80-AVR-0.210.hex` for AVR application flash;
- `FlexFox80-AVR-First-Install-0.210.hex` for programmer-based installation of BL0.3 and the matching relocated AVR 0.210 application on a new unit;
- `FlexFox80-ESP-2.27.bin` for ESP sketch flash at `0x000000`; and
- `FlexFox80-LittleFS-2.27.bin` only when filesystem factory/recovery replacement is explicitly intended.

The first-install HEX is not a wireless-update file and does not embed fuse writes. It requires the qualified Atmel-ICE/UPDI provisioning workflow, `CODESIZE=0x00`, `BOOTSIZE=0x20`, EEPROM/fuse preservation, and exact readback verification. A healthy resident BL0.3 can reinstall the AVR application wirelessly without using the first-install image.

## Recovery rules

1. Stop RF activity and identify the exact unit before any write.
2. Preserve and verify the unit's AVR EEPROM and fuses before an Atmel-ICE operation. Never use another transmitter's EEPROM as a generic image.
3. Prefer the guarded wireless sketch or AVR application workflow when its preflight passes. Use FTDI or Atmel-ICE only for the processor that cannot reach its wireless recovery path.
4. Do not replace LittleFS during a routine ESP sketch recovery. If filesystem replacement is required, first preserve any needed settings and events and acknowledge that the factory/recovery image erases them.
5. Read back or otherwise independently verify the written image, then confirm SSID, HTTP/WebSocket service, AVR telemetry, `SW_VERSIONS,2.27,0.210`, assignment, callsign, frequency, power, event behavior, and return to normal WiFi shutdown.

Unit-specific flash, EEPROM, fuse, SSID, event, and settings captures are deliberately excluded from GitHub release assets.
