# ESP Clone Synchronization Controls

**Date:** 2026-07-12

**Path:** B-TIME-01

**Status:** Source contracts, compatible pinned build, installed controls, and first two-unit clone/readback qualification pass; repeated phase and drift series pending

## Expected behavior

Clone operations should give every target the same repeatable relationship to the master's RTC second boundary. Absolute UTC accuracy is secondary. Ordinary AVR broadcasts must not compete with the synchronization transaction, and a completed clone must prove the target RTC contains the epoch it was told to write.

## Protocol sequence

| Stage | Master side | Target side |
| --- | --- | --- |
| Connection | Queues `$ESP,C;` and waits for the AVR ACK before confirming the slave | Queues `$ESP,C;`, drains Linkbus, and requires its AVR ACK before beginning the WebSocket clone flow |
| Ready | Sends slave confirmation only after master quiet mode is active | Sends `SLAVE,S` after confirmation and entering the clock-sync state |
| Boundary | Queues `$ESP,S;`; the AVR ISR captures the next RTC-edge epoch and foreground code reports it | Waits without ordinary target broadcasts |
| Clock write | Broadcasts the normal `SYNC,<epoch>` WebSocket frame generated from the one-shot AVR time report | Queues `$TIM,<ISO>,C;` to its AVR instead of writing directly to serial |
| Verification | Remains quiet during transfer | Accepts completion only after `!TIM,C,<epoch>;` exactly matches the requested epoch and the ordinary ACK clears |
| Cleanup | Queues `$ESP,R;` when the clone is released, disconnected, or aborted | Sends `$ESP,R;` during normal shutdown and on local failure cleanup |

The AVR retains its independent 900-second quiet-mode timeout, so loss of both ESP cleanup and communications cannot suppress reports indefinitely.

## Failure behavior

- A queue-drain or AVR ACK timeout prevents the target clone connection from proceeding.
- An AVR NAK, missing clone readback, mismatched epoch, or ten-second verification timeout closes the target clone flow.
- A master quiet-mode NAK/timeout releases the slave instead of falsely confirming it.
- Disconnect and explicit slave-release paths request broadcast resumption.
- The target never treats a normal Linkbus ACK alone as proof that the RTC contains the requested value.

## Compatibility correction

The existing sketch used `WebSocketsServer::isRunning()`, which is not part of the published WebSockets API inspected for this build. A source-owned boolean now tracks successful `begin()` and every `close()` call through paired start/stop helpers. This removes an undocumented custom-library dependency while retaining the intended duplicate-start guard.

## Verification

- The dependency-light firmware contract requires the quiet/ready/edge/write/readback sequence, NAK handling, cleanup controls, and source-owned WebSocket lifecycle state.
- All host tests and the EEPROM-layout contract pass.
- ESP8266 core 2.7.4 plus WebSockets 2.3.6 compiles the full sketch with zero warnings under the operator-confirmed Adafruit HUZZAH profile.
- Independent builds produce the same 503,392-byte firmware binary with SHA-256 `3b6b5ad8e20d9662c9ee833f9c8072b955b27f61d895cc9dde95a3d13f4a796e`.

Initial images built with ESP8266 cores 3.1.2 and 3.0.2 repeatedly reset even with pre-clone source. Core 2.7.4/WebSockets 2.3.6 starts normally with both pre-clone and clone-sync source, isolating the failure from these changes. The installed compatible image passes HTTP, WebSocket, live AVR telemetry, quiet suppression, one next-edge report, one-shot retention, and explicit resume.

Two operator-requested target resets then produced two complete master-target clone cycles. Both crossed the exact RTC-readback gate before file transfer and completed normal release. A first same-path comparison placed the target approximately 0.54–0.56 seconds behind the master rather than multiple seconds away. See [Two-unit clone synchronization qualification](TWO_UNIT_CLONE_SYNC_2026-07-13.md). Do not describe the field bug as fixed until repeated clone phase-spread, cleanup-failure, and drift measurements are complete.
