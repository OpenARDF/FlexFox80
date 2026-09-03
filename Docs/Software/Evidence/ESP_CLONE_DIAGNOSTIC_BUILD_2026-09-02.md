# ESP clone diagnostic build — 2026-09-02

## Scope

This is an ESP-only diagnostic candidate. It identifies itself as ESP `2.29` with the installed AVR version unchanged. It does not change AVR firmware, EEPROM layout, event-file format, or the established clone commands.

The target now records these clone phases: master association, AVR quieting, master handshake, clock-ready wait, clock write/readback, file wait, file receive, file validation, event programming, AVR resume cleanup, master release, completion, and failure. Every phase snapshot is printed to ESP serial as `CDIAG` and, once the target WebSocket is connected, sent to the master as a backward-compatible command that older firmware will ignore.

The master retains the last target `CDIAG` snapshot in RAM. This is specifically intended to preserve the last observed target phase when the target ESP and main-board LEDs all go dark. The target also detects a full clone receive buffer and reports an explicit `file-receive: file receive buffer overflowed` failure instead of silently overwriting the oldest queued line.

## Read-only evidence

Request `/firmware/status` from the master after a test. The most useful fields are:

- `masterLastTargetDiagnostic` and `masterLastTargetDiagnosticMillis` on the master;
- `cloneDiagnosticPhase`, `cloneDiagnosticFailurePhase`, and `cloneDiagnosticFailure`;
- `cloneDiagnosticAttempt`, file index/count, received-line count, and elapsed times;
- `cloneDiagnosticCleanupConfirmed` and `cloneDiagnosticReceiveBufferOverflow`;
- `cloneDiagnosticTemporaryFileReceptionActive`, `cloneDiagnosticTemporaryFileCleanupAttempted`, and `cloneDiagnosticTemporaryFileCleanupConfirmed` on ESP 2.34 and later.

The `CDIAG` wire record is:

```text
CDIAG,<target-ssid>,<phase>,<failure-phase>,<attempt>,<file-index>,<file-count>,<received-lines>,<elapsed-ms>,<cleanup-confirmed>,<receive-overflow>,<failure-text>,<clock-expected>,<clock-readback>,<clock-delta>,<recovered-reset-phase>,<recovered-reset-file-index>,<recovered-reset-file-count>,<recovered-reset-reason>,<free-heap>,<minimum-free-heap>,<largest-free-block>,<heap-fragmentation>,<temp-cleanup-attempted>,<temp-cleanup-confirmed>
```

For hardware trials, run `just wifi-adb-clone-monitor Tx_Master` before powering the target. It writes a timestamped JSONL file beneath ignored `Software/Huzzah/tmp/clone-monitor/`. The capture retains identity, health, `CDIAG`, `SLAVE`, success, and error records while omitting high-volume `SYNC` and event-file `FDAT` contents. This preserves the target's forwarded terminal record after either unit shuts down without adding diagnostic writes to the target filesystem.

Install only `ARDF_Transmitter.ino.bin` for this test. The generated LittleFS image is not required and should not be installed over a field unit's existing filesystem.

## Qualification boundary

Host tests and a pinned HUZZAH compile validate the state model and image. Hardware sync behavior, target power-loss timing, LED state, and master retention of the last target phase remain to be exercised on an authorized master/target pair.
