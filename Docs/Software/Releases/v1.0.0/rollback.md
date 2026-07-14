# FlexFox80 v1.0.0 Rollback Record

**Selection date:** 2026-07-14

**Approved by:** Charles Scharlau

## Policy

The reference rollback material proves that the established AVR `0.200` firmware and mature ESP image remain recoverable. It is not a generic state image for every FlexFox. Before any fleet unit is updated, preserve and verify that unit's complete AVR EEPROM, fuses, and full 4 MiB ESP flash. Unit-specific settings images must not be published on GitHub or copied between transmitters.

## AVR reference

The ignored local reference set is retained under `Software/AVR128DA48/tmp/hardware-probe/`:

| File | Bytes | SHA-256 | Role |
| --- | ---: | --- | --- |
| `flash-2026-07-12.bin` | 44,788 | `b3b1f6be8806dae59c71a4984066ee0747aa5022f239ca4ccf01a256d7c2dc93` | Previous field flash; embedded string reports AVR `0.200` |
| `eeprom-2026-07-12.bin` | 512 | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | Reference test unit's state only; never use as fleet-generic EEPROM |
| `fuses-2026-07-12.bin` | 16 | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | Reference test unit's verified fuse capture |

Fresh size and SHA-256 checks on 2026-07-14 match the programming evidence. [Mac AVR programming evidence](../../Evidence/MAC_AVR_PROGRAMMING_2026-07-12.md) records stable pre-write reads, recovery after a deliberately failed no-erase attempt, explicit EEPROM restoration, and independent flash/EEPROM/fuse comparison.

## ESP reference

The selected mature full-flash rollback baseline is:

| File | Bytes | SHA-256 |
| --- | ---: | --- |
| `Software/Huzzah/tmp/device-backups/huzzah-a4e57c2d69ed-20260712-full.bin` | 4,194,304 | `a03834d378a3b39b70e22839a0f3bf276fb8145a86340efe72c57d9cc1e800f6` |

Fresh size and SHA-256 checks on 2026-07-14 match the recorded baseline. [Mac ESP8266 programming evidence](../../Evidence/MAC_ESP8266_PROGRAMMING_2026-07-12.md) records a complete pre-program read, independent device verification, successful restoration after rejected 3.x-core firmware, another full-image verification, and return of normal LEDs and SSID.

A second verified 4 MiB reference capture remains available for HUZZAH MAC `44:17:93:0f:09:3e` with SHA-256 `ec05ea3f65b0f28be571c3c58e17b0272125b6e4d238b9114401d13ba74a81bf`; it is retained as device-specific secondary recovery material rather than the selected mature baseline.

## Recovery rule

1. Preserve and verify the affected unit's own state before changing it.
2. Stop RF activity and use the documented Atmel-ICE or FTDI recovery path.
3. Restore reference program firmware only with the unit's own validated EEPROM/fuse/ESP-state material as applicable.
4. Independently read back and compare every restored region.
5. Verify startup, SSID, HTTP/WebSocket, AVR telemetry, software versions, event, assignment, callsign, frequency, and power before returning the unit to service.

These ignored local files are deliberately excluded from GitHub release assets because they contain reference-unit state. The public release contains installation firmware, manifest, and checksums only.
