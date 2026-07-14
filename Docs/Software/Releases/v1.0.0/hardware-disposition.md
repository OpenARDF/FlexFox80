# FlexFox80 v1.0.0 Hardware-Test Disposition

**Decision date:** 2026-07-14

**Decision owner:** Charles Scharlau

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

## Decision

The focused host, exact-build, dummy-load, programming, timing-fault, cloning, corrupt-transfer, and restoration gates recorded in the repository are the hardware evidence basis for v1.0.0. The release owner approved deferring the remaining broad A8 tests for this first maintenance baseline because FlexFox is a mature field-proven product, the release changes are bounded hardening corrections, and R4 was the agreed firmware cutoff.

This is an explicit release disposition, not a claim that the deferred behaviors were tested or that the underlying findings are resolved.

## Deferred for v1.0.0

- repeated extended cold power-cycle campaign;
- additional characterization of the approximately 5–10% clone-retry tail;
- long-duration RTC drift and aging-register work;
- complete real scheduled-start, scheduled-finish, and sleep/wake cycles;
- complete energized Classic, Sprint, Foxoring, and beacon RF-cycle traces;
- antenna removal/reconnection recovery;
- complete Si5351/I2C RF-safe failure and recovery;
- long-duration event operation;
- extended temperature/fan behavior;
- ESP interruption during every clone phase;
- broader watchdog/reset-cause behavior; and
- general power-loss transaction testing.

## Evidence that remains applicable

The repository [release hardware checklist](../../RELEASE_HARDWARE_CHECKLIST_2026-07-14.md) distinguishes passed, partial, pending, and proposed-defer gates and links the underlying evidence. The v1.0.0 release retains those statuses. The following focused gates remain directly applicable:

- deterministic AVR, ESP sketch, and LittleFS builds;
- AVR EEPROM ABI and source contracts;
- installed startup, HTTP/WebSocket, AVR telemetry, and reset recovery;
- normal cloning, exact RTC readback, checksum rejection, and prior-event retention;
- RTC edge-delay recovery and synchronization fault containment;
- schedule predicate boundaries and atomic cross-context stores;
- guarded role-assignment persistence/restoration; and
- repeated byte-preserved flash, EEPROM, and fuse restoration during focused target tests.

## Release boundary

The representative paired-unit gate is not deferred. Before integration and tagging, the selected artifacts must be installed on one Ver 2.1 unit, report `SW_VERSIONS,2.1,0.201`, and retain the expected configuration. Any release-blocking regression observed there reopens the candidate rather than becoming another automatic deferral.
