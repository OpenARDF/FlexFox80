# FlexFox80 v2.0.3 Recovery and Rollback Record

The approved rollback point is the independently verified public `v2.0.2` release. Preserve each unit's own EEPROM and fuses during programmer-based recovery; never copy EEPROM between units.

For v2.0.3 recovery:

- use the guarded wireless AVR workflow for the normal AVR `0.210` application update on units already provisioned with BL0.3;
- use `FlexFox80-AVR-First-Install-0.210.hex` only for programmer-based BL0.3 plus relocated-application setup or boot-chain recovery;
- use `FlexFox80-AVR-0.210.hex` only for deliberate application-only legacy-address recovery;
- write `FlexFox80-ESP-2.35.bin` at `0x000000` without erasing LittleFS; and
- install `FlexFox80-LittleFS-2.35.bin` at `0x300000` only when factory/filesystem recovery is explicitly intended.

After recovery, verify the written image, exact unit identity, configuration, telemetry, event operation, clone synchronization where hardware permits, and `SW_VERSIONS,2.35,0.210`.
