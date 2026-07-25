# FlexFox80 v2.0.2 Recovery and Rollback Record

The approved rollback point is the independently verified public `v2.0.1` release. Preserve each unit's own EEPROM and fuses during programmer-based recovery; never copy EEPROM between units.

For v2.0.2 recovery:

- use the guarded wireless AVR workflow for the normal AVR `0.210` application update on units already provisioned with BL0.3;
- use `FlexFox80-AVR-First-Install-0.210.hex` only for programmer-based BL0.3 plus relocated-application setup or boot-chain recovery;
- use `FlexFox80-AVR-0.210.hex` only for deliberate application-only legacy-address recovery;
- write `FlexFox80-ESP-2.28.bin` at `0x000000` without erasing LittleFS; and
- install `FlexFox80-LittleFS-2.28.bin` at `0x300000` only when factory/filesystem recovery is explicitly intended.

After recovery, verify the written image, exact unit identity, configuration, telemetry, manual-radio behavior, event operation, and `SW_VERSIONS,2.28,0.210`.
