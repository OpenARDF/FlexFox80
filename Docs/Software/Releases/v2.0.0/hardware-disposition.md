# FlexFox80 v2.0.0 Hardware-Test Disposition

**Decision date:** 2026-07-20

**Decision owner:** Charles Scharlau

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

## Decision

The release owner authorized the full v2.0.0 GitHub release after provisioning all existing FlexFox hardware and completing extensive hardware testing. The release population is 17 units: 15 foxes, one beacon, and one master/spectator.

The fifteen uniquely named foxes each retain a final machine-readable wireless-upgrade summary with `result: pass`, exact expected SSID, ESP `2.27`, AVR `0.210`, BL0.3, and final `versionsAfter: 2.27,0.210`. The beacon and master/spectator use the shared `Tx_Master` name and were upgraded and confirmed interactively rather than represented by those unique-SSID summaries. On 2026-07-20 the owner confirmed that this is the entire FlexFox fleet, that the update sequence was complete, and that the hardware had been thoroughly tested.

## Qualification covered

- initial Atmel-ICE bootloader/application provisioning with EEPROM and fuse preservation;
- wireless AVR application recovery, interrupted transfer handling, visible progress, completion, and return to normal operation;
- protected ESP sketch updates, interrupted upload/filesystem recovery, and LittleFS-preserving upgrades;
- future scheduled starts, event completion, sleep/wake behavior, reset and power-cycle recovery;
- Sprint and Classic event operation, web event selection/edit/apply/clear behavior, and RF-off disabled-event behavior;
- clone/sync and two-unit interactions;
- startup temperature validity and unavailable-temperature handling; and
- final per-unit identity, embedded-version, web-file, and telemetry gates.

## Residual boundaries

Extended fleet-soak automation was deliberately mothballed because it diverged from the ordinary events.html and sync/clone paths under test. Its absence does not change the ordinary operational paths or the completed fleet qualification. Long-duration environmental aging, deliberately induced bootloader NVM/serial stalls, redundant boot self-checks, and ESP dual-image rollback remain future hardening work rather than known release-blocking defects.

Any future bootloader change requires renewed programmer-access qualification. Ordinary AVR and ESP application corrections may use the now-qualified wireless maintenance paths.
