# ESP Clone Synchronization Controls

**Date:** 2026-07-12

**Path:** B-TIME-01

**Status:** Source contracts and pinned ESP build pass; ESP deployment and live clone test pending

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
- ESP8266 core 3.1.2 plus WebSockets 2.7.2 compiles the full sketch with zero warnings under the operator-confirmed Adafruit HUZZAH profile.
- Independent builds produce the same 512,448-byte firmware binary with SHA-256 `87e46f71595522434985f585ea543af075b034f6293e6e116f60cd53f6df257d`.

No ESP flash or LittleFS write has been performed. Do not describe the field bug as fixed until a preserved/rollback-capable ESP programming procedure and live master-target clone test demonstrate quieting, next-edge delivery, exact readback, resume, and repeatable phase spread.
