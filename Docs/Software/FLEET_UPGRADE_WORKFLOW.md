# FlexFox80 Fleet Upgrade Workflow

This workflow upgrades one opened fleet unit at a time while reusing the qualified ESP, AVR, web-deployment, and probe commands. It is intended for the one-time Atmel-ICE bootloader installation and for safe restart after an interrupted attempt.

## Scope and order

The guarded sequence is:

1. identify the exact FlexFox through its SSID and live ESP/AVR telemetry;
2. install the qualified ESP sketch wirelessly, unless the exact version, installed-image MD5, byte length, and LittleFS protection already match;
3. use the connected Atmel-ICE to verify or provision the resident bootloader and relocated AVR application while preserving EEPROM;
4. reconnect the Moto automatically after resets;
5. replace only `events.html`, `radio.html`, and `test.html`, skipping any file whose downloaded bytes already match;
6. require the exact combined versions, identity, role, HTTP/WebSocket path, and live AVR telemetry before reporting completion.

The workflow never writes a complete LittleFS image. It does not delete or replace `.event`, `.me`, settings, or other unit-specific files.

For a closed unit that already has the qualified resident bootloader, use the wireless-only recipes. They retain the same identity, ESP, web-file, and final telemetry gates, but replace the Atmel-ICE phase with the protected `wifi-avr-update` workflow. `Tx_Master` is intentionally rejected by this mode because its non-unique name cannot supply the MAC-derived SSID authorization required for wireless AVR bootloading.

## Prerequisites

- Build and qualify the release artifacts before starting the fleet loop.
- Connect exactly one Moto through ADB. USB tethering keeps ADB available while the Moto changes Wi-Fi networks.
- Power the intended FlexFox, connect its Atmel-ICE UPDI lead, and make sure the reported SSID belongs to that same opened unit.
- Use `Tx_Master` for the master/beacon. Use the complete uppercase MAC-derived `Tx_XXXXXXXX` SSID for other units.
- Do not start while an event, clone, firmware transaction, or event-programming transaction is active.

## Read-only preflight

Run this before authorizing a unit when its identity or connection is uncertain:

```sh
just fleet-upgrade-preflight fox-01 Tx_7C2D6FD3
```

The preflight joins the SSID, prepares temporary localhost ADB relays, checks the release artifacts, probes identity and telemetry, and reports which ESP and web writes would be skipped. It does not write firmware, fuses, EEPROM, or filesystem files, and it does not enter UPDI mode.

## Upgrade one unit

After confirming that the SSID and Atmel-ICE lead refer to the same opened unit:

```sh
FLEXFOX_FLEET_UPGRADE_CONFIRM='UPGRADE FLEXFOX UNIT' \
  just fleet-upgrade-unit fox-01 Tx_7C2D6FD3
```

For the master/beacon:

```sh
FLEXFOX_FLEET_UPGRADE_CONFIRM='UPGRADE FLEXFOX UNIT' \
  just fleet-upgrade-unit beacon Tx_Master
```

The command discovers a single connected ADB device automatically. Set `FLEXFOX_ADB_SERIAL` when more than one device is present. Set `FLEXFOX_ADB` only when `adb` is outside `PATH`, `ANDROID_HOME`, and the normal macOS Android SDK location.

## Wireless-only catch-up

For a unit whose BL0.3 bootloader was already independently provisioned and verified:

```sh
just fleet-wireless-upgrade-preflight fox-01 Tx_7C2D6FD3

FLEXFOX_FLEET_UPGRADE_CONFIRM='UPGRADE FLEXFOX UNIT' \
  just fleet-wireless-upgrade-unit fox-01 Tx_7C2D6FD3
```

The guarded order is ESP sketch, AVR application through BL0.3, the three approved web files, and final combined identity/version/telemetry verification. The Moto is repeatedly associated with the exact SSID while either processor restarts. No Atmel-ICE connection is used or required.

## Interruption and retry behavior

Rerun the same command with the same unit label and SSID. Completion is not trusted merely because an earlier local log says that a phase passed:

- ESP is skipped only after matching the release version, installed-image MD5, byte length, and LittleFS-protection flag.
- AVR programming is skipped only after Atmel-ICE verifies the exact combined image, `CODESIZE=0x00`, `BOOTSIZE=0x20`, and the cleared EEPROM recovery marker.
- Each web file is skipped only after a fresh download matches its release byte length and SHA-256.
- Final success always requires a new live combined-version and telemetry probe.

If AVR provisioning was interrupted, leave the Atmel-ICE attached and rerun the command. The existing provisioning script performs a full EEPROM/fuse capture before any write and independently verifies flash, restored EEPROM, and fuses afterward.

## Evidence

Each invocation writes an ignored per-unit directory beneath:

```text
Software/AVR128DA48/tmp/fleet-upgrade/<timestamp>-<unit-id>/
```

`upgrade.log` contains the composed command output. `summary.json` records exact artifact hashes, identity, phase results, timestamps, and the final outcome. The AVR provisioning script retains its separate EEPROM, fuse, and readback evidence beneath `Software/AVR128DA48/tmp/fleet-provision/`.

Do not close a chassis based only on an accepted upload or an intermediate LED pattern. Close it only after the wrapper reports its final `PASS` line.
