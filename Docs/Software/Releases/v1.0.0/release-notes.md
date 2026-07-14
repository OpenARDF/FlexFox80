# FlexFox80 v1.0.0 Maintenance Release

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Firmware:** ESP `2.1`, AVR `0.201` (`SW_VERSIONS,2.1,0.201`)

**Maintainer approval:** Approved for annotated `v1.0.0` tagging and GitHub publication by the release owner on 2026-07-14 after the representative installed-version and clean-main reproduction gates passed.

## Summary

This first tagged maintenance baseline preserves the established FlexFox80 operating model while correcting bounded reliability defects found during the July 2026 hardening review. It emphasizes clone-time coordination, malformed-input containment, persistence-width correctness, and recovery from timing delays. It does not introduce a new event format or change normal transmitter assignments, frequencies, powers, or user workflows.

## User-visible improvements

- Wireless cloning pauses unsolicited AVR broadcasts, aligns the master's one-shot time report to an RTC edge, prioritizes the target time write, and requires exact RTC epoch readback before event-file transfer.
- AVR startup aligns system time to an RTC edge, reducing reset-dependent whole-second phase differences between otherwise synchronized units.
- Clone-received event files require the existing matching transfer checksum before replacing a prior valid file. Ordinary legacy and web-edited event files remain compatible without a stored checksum.
- ESP role assignment retains the complete role index, including multi-digit roles.

## Reliability corrections

- Delayed RTC edges are counted and replayed so a multi-second normal-priority interrupt blockage does not silently lose elapsed seconds.
- RTC synchronization waits are bounded, failed reads are rejected, and a read crossing an RTC edge cannot commit stale time.
- Foreground schedule, event-window, wake-time, and on-air state transfers are atomic where interrupt readers could otherwise observe torn multi-byte values.
- Linkbus input enforces message-ID, field-count, and field-length bounds, and internal message IDs preserve their wire characters without legacy numeric aliases.
- AVR text-output helpers copy percent characters literally, enforce destination bounds, and reject oversized frames.
- Circular buffers handle zero capacity and allocation failure safely; documented LIFO `pop()` behavior is corrected.
- EEPROM writes use the deployed widths, persisted enum fields have a fixed-width 274-byte schema, and Goertzel sample storage no longer depends on dynamic allocation during normal operation.

## Compatibility and installation cautions

- The AVR EEPROM schema is 274 bytes (`0x112`). Historical 268-byte Release EEPROM requires the documented validated migration before use.
- Existing stored event files remain valid; the checksum requirement applies only to clone-transfer `/Temp` files.
- Routine AVR installation must preserve and verify unit-specific EEPROM and fuses.
- Routine ESP sketch installation writes the sketch at address `0x000000` without replacing LittleFS.
- The separately published LittleFS image is recovery/factory content for address `0x300000` under the qualified 4M1M profile. Installing it replaces stored web assets, events, and settings.

## Verification

- The complete host, policy, firmware-contract, and EEPROM-layout suite passes.
- Pinned AVR and ESP Release builds are warning-free and deterministic.
- Focused dummy-load testing covers programming identity, EEPROM/fuse preservation, Linkbus bounds and recovery, RTC delay recovery, RTC synchronization faults, event boundaries, role assignment, normal cloning, corrupt-clone rejection, prior-file retention, and configuration restoration.
- Release assets include a manifest and SHA-256 checksum list and are independently verified after packaging.

## Known deferred work

- `B-CLONE-02`: approximately 5–10% of clones may require a retry. The workaround is to retry; timeout and autonomous recovery characterization remain deferred.
- `B-TIME-01`: the corrected clone path removes observed whole-second reset quantization and prevents counted-edge loss, while long-duration RTC drift/aging work remains deferred.
- Broader watchdog/reset-cause policy, post-start RTC hardware-failure policy, cross-processor numeric-input validation, remaining EEPROM transaction/version work, and open-AP/`PASS` bridge policy remain documented for later hardening.

These deferrals reflect the mature product's field reliability and the decision to freeze behavior after R4. They are not declarations that the findings are resolved.
