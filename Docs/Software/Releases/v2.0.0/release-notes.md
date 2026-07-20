# FlexFox80 v2.0.0

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Firmware:** ESP `2.27`, AVR `0.210` (`SW_VERSIONS,2.27,0.210`)

**Bootloader:** AVR BL0.3 is already resident on the provisioned fleet; it is not included as a routine release asset.

## Highlights

- Adds guarded wireless AVR application updates through the ESP, with exact-device authorization, preflight validation, resumable page transfer, visible progress, bounded maintenance power, and final verification.
- Adds protected ESP wireless sketch updates that preserve LittleFS, verify the selected image, recover cleanly from interrupted uploads, and close completed upload sessions.
- Corrects the critical AVR sleep/wake RTC accounting defect that caused the withdrawn AVR `0.201` release to miss starts or finish early.
- Prevents invalid startup temperatures from being recorded or displayed; unavailable readings are carried explicitly as `!TEM,NA` and rejected independently by the web pages.
- Makes event Apply/Clear operations persistent and reliable, improves date/time picker behavior and event status feedback, and fixes frequency-control synchronization.
- Improves clone/event transfer integrity, startup recovery, device identity handling, and Android-assisted maintenance access.
- Provides repeatable Atmel-ICE and wireless fleet-upgrade workflows with per-unit identity, artifact, phase, and final-version evidence.

## Verification

- The complete repository policy, documentation, host-test, firmware-contract, and 274-byte EEPROM-layout suite passes.
- Two pinned AVR-GCC 7.3.0 / AVR-Dx_DFP 1.9.103 Release builds are warning-free and byte-identical.
- Two pinned ESP8266 core 2.7.4 / WebSockets 2.3.6 builds are warning-free and byte-identical, including the selected LittleFS image.
- All 15 uniquely named foxes retain final automated verification summaries for ESP `2.27` and AVR `0.210`; the beacon and master/spectator were also upgraded and confirmed by the release owner.
- Hardware work during qualification covered scheduled starts and finishes, Sprint and Classic operation, event editing/apply behavior, clone/sync behavior, interrupted/retried maintenance, ESP and AVR wireless updates, reset/power-cycle recovery, and normal return to battery-saving shutdown.

## Installation cautions

- Preserve and verify each unit's EEPROM and fuses when using Atmel-ICE. Do not copy EEPROM between units.
- The AVR HEX is application firmware. BL0.3 initial installation or recovery remains a programmer operation.
- Write the ESP sketch BIN at `0x000000`. Do not erase the whole flash during a routine update.
- The LittleFS BIN is recovery/factory content for `0x300000` under the qualified 4M1M layout. It replaces stored settings, event files, and web files.
- Cryptographic firmware signatures are intentionally not part of this release. Wireless AVR access uses the target's MAC-derived SSID suffix as the operator unlock and is limited by the AVR-controlled maintenance window.

## Known boundaries

- ESP access remains intentionally time-limited to protect the battery. Maintenance workflows extend the lease only while verified progress/keep-alives continue.
- Moto devices may refuse automatic reassociation with the ESP open AP; the provided ADB relay/reassociation workflow is the qualified debug and upgrade path.
- BL0.3 watchdog/self-check expansion and dual-image ESP rollback remain future boot-chain hardening opportunities. They are not known release-blocking defects.
- Fleet-soak-specific automation is mothballed; it is not required for ordinary event creation, sync/clone, or release operation.

The historical `v1.0.0` tag remains an audit marker only. Its deleted GitHub release and AVR `0.201` assets must not be treated as approved firmware.
