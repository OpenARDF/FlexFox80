# FlexFox80 Firmware Release Notes — Draft

**Draft date:** 2026-07-14

**Release label:** Pending

**Status:** Engineering draft; not yet approved for publication or fleet deployment

## Summary

This candidate preserves the established FlexFox80 operating model while correcting bounded reliability defects found during the July 2026 hardening review. The work emphasizes clone-time coordination, malformed-input containment, persistence-width correctness, and recovery from timing delays. It does not introduce a new event format or change normal transmitter assignments, frequencies, powers, or user workflows.

## User-visible improvements

- Wireless cloning now pauses unsolicited AVR broadcasts, aligns the master's one-shot time report to an RTC edge, prioritizes the target time write, and requires exact RTC epoch readback before event-file transfer.
- AVR startup aligns system time to an RTC edge, reducing reset-dependent whole-second phase differences between otherwise synchronized units.
- Clone-received event files must carry the existing matching transfer checksum before replacing a prior valid file. Ordinary legacy and web-edited event files remain compatible without a stored checksum.
- ESP role assignment retains the complete role index, including multi-digit roles, rather than using only its first character.

## Reliability corrections

- Delayed RTC edges are counted and replayed so a multi-second normal-priority interrupt blockage does not silently lose elapsed seconds.
- RTC synchronization waits are bounded, failed reads are rejected, and a read crossing an RTC edge cannot commit stale time.
- Foreground schedule, event-window, wake-time, and on-air state transfers are made atomic where interrupt readers could otherwise observe torn multi-byte values.
- Linkbus input now enforces message-ID, field-count, and field-length bounds, and internal message IDs preserve their wire characters without legacy numeric aliases.
- AVR text-output helpers copy percent characters literally, enforce destination bounds, and reject oversized frames.
- Circular buffers now handle zero capacity and allocation failure safely; documented LIFO `pop()` behavior is corrected.
- EEPROM writes for the I2C failure counter and RF-power initialization use the deployed 16-bit widths, and persisted enum fields have a fixed-width 274-byte schema.
- Goertzel sample storage no longer depends on dynamic allocation during normal operation.

## Compatibility

- AVR EEPROM schema: 274 bytes (`0x112`). Historical 268-byte Release EEPROM requires the documented validated migration before use with this candidate.
- Existing stored event files remain valid; the checksum requirement applies only to clone-transfer `/Temp` files.
- Qualified ESP build environment remains ESP8266 core 2.7.4 with WebSockets 2.3.6 on the 80 MHz Adafruit HUZZAH profile.
- The normal Web interface, event definitions, RF frequencies, power values, and Linkbus wire labels remain unchanged.

## Verification summary

- Full host and source-contract suite passes with sanitizers.
- Two exact AVR Release builds are warning-free and byte-identical.
- Two exact ESP builds are warning-free and produce the same selected sketch image.
- Focused dummy-load target tests cover programming identity, EEPROM/fuse preservation, Linkbus bounds and recovery, RTC delay recovery, RTC synchronization faults, event boundaries, role assignment, normal cloning, corrupt-clone rejection, prior-file retention, and final configuration restoration.
- Exact artifact hashes and detailed evidence are recorded in [release-candidate snapshot](RELEASE_CANDIDATE_SNAPSHOT_2026-07-14.md).

## Known deferred work

- `B-CLONE-02`: approximately 5–10% of clones may require a retry. The workaround is to retry; timeout and autonomous recovery characterization are deferred.
- `B-TIME-01`: the corrected clone path removes observed whole-second reset quantization and prevents counted-edge loss, but broader clone-tail evidence and long-duration RTC drift/aging work are deferred.
- Broader watchdog/reset-cause policy, post-start RTC hardware-failure policy, cross-processor numeric-input validation, remaining EEPROM transaction/version work, and open-AP/`PASS` bridge policy remain documented for later hardening.

These deferrals reflect the mature product's field reliability and the decision to freeze behavior after R4. They are not declarations that the findings are resolved.

## Before release

The release label, board revision description, remaining A8 hardware tests or approved skips, rollback package, integration commit, and publication approval are still required. Do not deploy from this draft alone.
