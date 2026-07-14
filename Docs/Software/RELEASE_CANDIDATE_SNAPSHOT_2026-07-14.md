# FlexFox80 Release-Candidate Snapshot

**Snapshot date:** 2026-07-14

**Development branch:** `Development_AVR128DA48`

**Qualification base commit:** `730d5bad796c9561f483382a0119dea3284cb125`

**Status:** Firmware behavior is frozen, release firmware identities are assigned, and automated/build gates and R4 pass; official release approval and the remaining A8 hardware decisions are pending

## Purpose

This document preserves the first complete release-candidate identity after R4. It is not a release announcement and does not authorize branch integration or fleet programming.

## Firmware identity

| Processor | Embedded version | Last source commit | Release artifact | SHA-256 |
| --- | --- | --- | --- | --- |
| AVR128DA48 | `0.201` | `99bc9e93397f86c996870731c158cb4d060cd8ec` | `FlexFox80.hex` | `897076608a3473d48c0226dfd8800f5ac058e553286a2f97450bfd03a77f1ba4` |
| ESP8266 | `2.1` | `99bc9e93397f86c996870731c158cb4d060cd8ec` | `ARDF_Transmitter.ino.bin` | `0749b0e493254d526cb7a5e8afd938bd65b04c22bfffaa6c5150614f6315bad8` |
| ESP8266 LittleFS | data inputs at `c2e4e98` | `c2e4e989f8ce5f1bcb4124bcea72ac88c7381446` | `ARDF_Transmitter.littlefs.bin` | `0b45a6ad86ea7774fc6f964a2325417f6d8978f7d2213305d3786b9ee77f0c41` |

The ESP source has no separate embedded firmware-date identifier. The table therefore records the source commit and this build date rather than inventing one. The sole supported release hardware target is **FlexFox Ver 2.1 (Mar 2022)**.

## Exact build evidence

### AVR

Two consecutive builds used the repository Release wrapper, AVR-GCC 7.3.0, and Atmel `AVR-Dx_DFP` 1.9.103. Both builds completed with zero warnings and byte-identical artifacts.

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `32f1d263ab4ed1d7080a80f0a49d771b2a6061cf579ed67382f988b3f5c6f1eb` |
| `FlexFox80.hex` | `897076608a3473d48c0226dfd8800f5ac058e553286a2f97450bfd03a77f1ba4` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `6f24be84f2a0cc868ab1835d8a0664de7295939274a0309b7ba3ea79788d0df8` |
| `FlexFox80.lss` | `2dbc7a33d41fc12c9eb894730ae18d2124aa00122bb5d11b47faa0486f378108` |
| `FlexFox80.srec` | `dc5600fbecefa5482a9a22effed011033e0bca2e4966b6e0a1a89c787f7cd753` |

Resource totals are 41,010 bytes text, 1,112 bytes data, and 1,572 bytes BSS. The linker map reports `.eeprom = 0x112` (274 bytes).

### ESP8266

Two consecutive builds used Arduino CLI 1.2.0, ESP8266 core 2.7.4, WebSockets 2.3.6, `mklittlefs` 2.5.0, Adafruit Feather HUZZAH at 80 MHz, 4 MB flash / 1 MB filesystem, lwIP v2 Lower Memory, and debug disabled. Both builds completed with zero warnings and produced the same sketch hash.

The sketch uses 499,676 bytes of program storage and 39,732 bytes of dynamic memory, leaving 42,188 bytes for local allocations. IRAM use is 27,612 of 32,768 bytes.

The selected sketch image has SHA-256 `0749b0e493254d526cb7a5e8afd938bd65b04c22bfffaa6c5150614f6315bad8`. The selected LittleFS image remains `0b45a6ad86ea7774fc6f964a2325417f6d8978f7d2213305d3786b9ee77f0c41` because the firmware identity change does not alter filesystem inputs.

The LittleFS tool does not promise byte-identical images across all invocations even when inputs are unchanged. Both builds in this checkpoint produced the hash shown above; release packaging must publish the actual selected filesystem image and its hash rather than assume a future rebuild matches.

## Verification completed

- `just check` passed every repository policy, Markdown, sanitizer-enabled host, firmware-contract, and 65-field EEPROM-layout check.
- The firmware source contract requires AVR `0.201` and ESP `2.1` and verifies that their existing reporting paths compose `SW_VERSIONS,2.1,0.201`.
- All 70 project-owned production firmware files carry standardized, dated MIT and module-purpose headers. Generated and externally licensed sources retain their upstream notices.
- Exact post-header AVR and ESP builds remained byte-identical to the selected release images, confirming that the source-documentation pass changed no executable firmware.
- The focused ESP event-file integrity regression passed all legacy, required-checksum, malformed, missing, duplicate, ordering, and bounds cases.
- [R4 target qualification](Evidence/ESP_EVENT_FILE_INTEGRITY_2026-07-13.md) passed installed startup, normal clone, controlled mismatched-checksum rejection, prior-file retention, cleanup, and two-unit restoration.
- The test master and R4 candidate were restored to their recorded event, callsign, role, master/receiver, frequency, power, and telemetry baselines.
- The accumulated AVR corrections have focused host, exact-build, and applicable connected-target evidence linked from the [hardening tracker](HARDENING_AND_BUG_PLAN.md).
- The selected ESP image has installed HTTP, WebSocket, AVR telemetry, role-assignment, clone-control, normal-clone, and rejection-path evidence.

## Branch readiness

After pushing the source-header checkpoint, `Development_AVR128DA48` and `origin/Development_AVR128DA48` both pointed to `99bc9e9`. The development branch was 118 commits ahead of `origin/AVR128DA48` and zero commits behind. Integration can therefore be a fast-forward if that relationship remains unchanged, but no integration was performed.

The separately planned transition from the AVR128DA48 product line to `main` remains out of scope.

## Rollback position

Known-good AVR and ESP rollback captures already exist outside ordinary source commits. The programming and recovery procedures are documented in [release workflow](RELEASE_WORKFLOW.md), [Mac AVR programming evidence](Evidence/MAC_AVR_PROGRAMMING_2026-07-12.md), and [Mac ESP8266 programming evidence](Evidence/MAC_ESP8266_PROGRAMMING_2026-07-12.md). At the operator's direction, R4 did not duplicate full-flash captures at every checkpoint.

This establishes available rollback material; it does not by itself satisfy A8's requirement that the final selected rollback be explicitly verified for the official release.

## Open release gates and decisions

The following are not silently treated as passed:

- install both selected artifacts on a representative paired unit and require the live combined response `SW_VERSIONS,2.1,0.201`;
- execute or explicitly approve skips in the [release hardware checklist](RELEASE_HARDWARE_CHECKLIST_2026-07-14.md), including representative classic, sprint, foxoring, beacon, antenna removal/reconnection, scheduled sleep/wake, temperature/fan, long-duration event, and broader fault-recovery checks;
- select and verify the final rollback package;
- approve the user-readable release notes;
- re-run the clean-checkout build and status gates at the exact integration commit;
- explicitly approve fast-forward integration into `AVR128DA48` and any later release distribution.

No additional firmware behavior work is planned unless one of these gates reproduces a release-blocking regression.
