# ESP-to-AVR Linkbus Reliability Review

**Date:** 2026-07-17

**Paths:** B-LINK-04 and B-HTTP-03

**Status:** ESP-only 2.6 correction implemented and host/build qualified; master/target pilot pending

## Scope and operating assumption

This review treats the repaired fleet hardware as the reliable baseline. It does not propose PCB changes, continuity tests, or a hardware redesign. The earlier directional Linkbus fault remains useful only as evidence that diagnostics must distinguish ESP transmit, ESP receive, AVR receive, and AVR transmit failures.

The current production path is a 9600-baud ASCII Linkbus between the ESP8266 and AVR128DA48. The ESP queues commands and permits one unacknowledged command on the wire at a time. The AVR parses commands into three receive buffers and queues replies in four transmit buffers.

## Executive conclusion

Several software behaviors can turn one transient missed response into a misleading success, a long stale queue, or a later shutdown:

1. `sendEventToATMEGA()` does not treat an AVR NAK as failure. It resets only the timeout flag at entry, and its success condition does not inspect `g_linkBusNacksReceived`.
2. The ESP prequeues the complete 17-command event load. On failure it waits only about ten seconds, returns, and leaves the remaining commands in the shared queue.
3. Clone keep-alives use that same queue. They are scheduled immediately and then every 20 seconds, including while event programming is in progress.
4. AVR acknowledgements confirm that a recognized handler ran, not that the requested setting was successfully applied. Frequency can fail silently, while power and event activation can record an internal error; all can still send `!ACK;`.
5. The ESP sends `PRM` before `GO,2`. `PRM` saves all current settings unconditionally, so an interrupted transfer can persist a partial configuration before `GO,2` checks whether every required field arrived.
6. The AVR silently drops output when all four transmit buffers are occupied. The short 200-iteration foreground spin is not a baud-rate-scale wait, and most callers ignore the send result. A dropped ACK is therefore indistinguishable from a broken link at the ESP.
7. ACK and NAK frames carry no command or sequence identity. A delayed or duplicated ACK can be attributed to the wrong pending command after recovery advances the queue.
8. Neither processor records enough transport telemetry to distinguish framing errors, receive-buffer exhaustion, transmit-buffer exhaustion, a NAK, or a response timeout.

These are sufficient reasons to harden the software even when the physical UART works reliably most of the time.

## Current event-transfer behavior

`sendEventToATMEGA()` queues, in order:

1. prepare (`GO,0`);
2. finish time;
3. pattern;
4. off, on, offset, and ID intervals;
5. band, power, modulation, and frequency;
6. callsign and both code speeds;
7. start time;
8. unconditional persistence (`PRM`); and
9. activation (`GO,2`).

The function advances its local state whenever the 25-entry ESP queue is not full. It does not wait for the preceding command's ACK before adding the next command. The underlying `linkbusLoop()` still transmits only one pending command at a time, but ownership and outcome are lost once the whole transaction is mixed into the shared queue.

For an unanswered command, `linkbusLoop()` uses three approximately three-second acknowledgement windows. It marks the timeout flag on the first retry. After the final window it clears the pending state and advances to the next queued command. It also clears the accumulated NAK count at that point, erasing useful failure evidence.

Once `GO,2` has been queued, `sendEventToATMEGA()` waits at most approximately ten seconds for the entire shared queue to become empty. A first command that receives no response consumes almost that whole allowance. The function then reports failure while most event commands remain queued and continue to be transmitted later.

This design has three consequences:

- the reported failure phase is only “Processor not responding,” not the command that failed;
- recovery, clone cleanup, and shutdown messages can queue behind stale configuration frames; and
- later ACKs and maintenance frames can change global state after the event operation has already reported failure.

## NAK and semantic-ACK defects

The ESP's `handleLBMessage()` clears the pending flag and increments `g_linkBusNacksReceived` when it receives `!NAK;`. However, `sendEventToATMEGA()` neither resets that counter at transaction start nor checks it before declaring success. A syntactically rejected event command can therefore produce an apparently successful event transfer if no timeout occurs.

The AVR has a separate semantic problem. `handleLinkBusMsgs()` begins each recognized command with `send_ack = true`. Examples that still ACK after failure include:

- a power-setting failure from `txSetParameters()`;
- a frequency-setting failure from `txSetFrequency()`; and
- `GO,2` with an incomplete `g_Event_Configuration_Check` mask.

The RTC-setting path is stronger: it suppresses ACK and sends NAK when its write/read/synchronization qualification fails. Event programming should adopt that explicit outcome model.

## Persistence and partial-state risk

The current AVR does not stage a candidate event. Each accepted field directly mutates the live configuration globals. `GO,0` clears the received-field mask and counters but does not create an isolated copy of the prior event.

`PRM` then calls `saveAllEEPROM()` without checking that the event is complete. `GO,2` performs the completeness check and already saves changed settings on the successful path. Sending `PRM` before `GO,2` is therefore redundant for a valid changed event and unsafe for an interrupted event.

Removing `PRM` from the ESP transaction would prevent incomplete settings from being persisted and avoid redundant EEPROM writes. It would not make the transaction atomic in RAM. Full protection requires AVR-side staging followed by validation and one commit operation.

## AVR queue and UART observability

The AVR can queue four outgoing Linkbus frames. `lb_send_text()` returns failure when the bus is disabled, the text is invalid, or no transmit buffer becomes available. When all buffers are occupied, it spins for only 200 foreground iterations while a 9600-baud frame needs milliseconds to leave the UART. The result is normally ignored by `lb_send_msg()`, `lb_broadcast_str()`, and command handlers, including ACK/NAK sends.

The USART receive ISR reads only `RXDATAL`. It does not record framing, parity, or hardware receive-overflow status. If all three parsed receive buffers are occupied, incoming bytes are ignored until a buffer becomes available. Neither condition is exposed to the ESP or browser.

The normal ESP one-command-at-a-time wire discipline makes AVR receive-buffer exhaustion less likely than transmit-buffer loss. AVR output can still burst: a command-specific reply, an ACK, and foreground time/error/status reports can be generated close together. Clone quiet mode reduces that traffic and is a useful existing reliability pattern.

## Permanent Linkbus-disable path

The legacy `$WI,2;` path tells the AVR to disable Linkbus for wired ESP programming and has no exit without a power cycle. The ESP writes this command directly rather than through the acknowledged queue when its programming-button indication is latched. The AVR disables Linkbus inside the handler before its ordinary ACK can be sent.

This review does not identify `$WI,2;` as the cause of a particular field failure. It is nevertheless a software path that can deliberately create the same permanent communication symptom. Now that protected Wi-Fi ESP updating exists, the path should be removed from ordinary runtime handling or gated behind an explicit, separately confirmed service mode.

## Correction to the prior 178-second timing model

The live observation that `/firmware/status` first answered at 178.332 seconds remains valid. The prior explanation that all 17 event commands block startup for about 153 seconds does not match the current `sendEventToATMEGA()` control flow. The function prequeues the commands and gives up after about ten seconds; it does not synchronously wait three retry windows for each command before returning.

The abandoned queue can still require roughly 17 times nine seconds to drain if every command is unanswered, with additional maintenance frames, but that draining occurs after the event function has already reported failure. By itself, the backlog does not prove that `g_http_server.begin()` was delayed for the whole drain interval.

The 178-second HTTP symptom must therefore remain open until phase timestamps identify the controlling delay. Required timestamps are:

- ESP reset and `setup()` completion;
- first `$ESP,0;` enqueue and transmit;
- first valid AVR frame;
- master-search start and finish;
- event transaction start and failure/success;
- `WiFi.softAP()` start/result;
- `g_http_server.begin()`; and
- first accepted TCP/HTTP request.

This separates a late server start from a server that started but was not reachable through the AP/network path.

## Recommended implementation order

### Phase 1 — ESP-only, low fleet risk

1. Add a small, dependency-free Linkbus transaction state machine with host tests.
2. Give event programming exclusive queue ownership. Pause clone/browser/firmware maintenance insertion for the bounded transaction.
3. Drain the existing queue, reset both timeout and NAK state, and require a fresh bidirectional preflight such as a new `$VER?` response plus its ACK.
4. Queue one event command, wait for its outcome, and only then queue the next. Abort on the first final timeout or any NAK. Do not leave transaction frames queued after failure.
5. Track the phase, command label, attempt count, last valid AVR-frame time, and failure reason. Expose them through a read-only status response and use them in clone/browser error text.
6. Remove the pre-activation `PRM` after a focused compatibility test proves that successful `GO,2` retains the intended EEPROM behavior.
7. Suspend periodic keep-alive insertion only for the short transaction. Send one acknowledged keep-alive before the transaction; clone quiet mode already has a 900-second AVR fail-safe.
8. Gate or remove automatic `$WI,2;` transmission outside an explicit wired-programming service flow.

This phase requires only ESP updates. It makes failures bounded and diagnosable and prevents stale commands from affecting cleanup. It cannot provide atomic AVR configuration or trustworthy semantic ACKs.

### Phase 2 — AVR correction at a later service opportunity

1. Send NAK, not ACK, when a requested setting cannot be applied or `GO,2` finds an incomplete event.
2. Return and handle every Linkbus enqueue result. Reserve or prioritize capacity for command ACK/NAK over unsolicited telemetry.
3. Replace the 200-iteration pseudo-wait with a nonblocking, observable transmit-queue policy.
4. Count USART framing/overflow, parsed-RX exhaustion, TX exhaustion, and dropped critical replies; expose counters through a versioned diagnostic query.
5. Suppress nonessential reports during any configuration transaction, not only cloning.

These changes alter the AVR image and should not trigger an immediate fleet reflash solely for hardening. They should be bundled with the next necessary AVR service release and tested against the current ESP for backward compatibility.

### Phase 3 — atomic versioned transaction

Stage all candidate event values separately on the AVR, validate ranges and completeness, verify a transaction ID and checksum, then atomically publish and persist the event. Define idempotent replay for a lost commit ACK and leave the prior active/persisted event untouched on abort.

This is the strongest design but has the broadest timing, persistence, compatibility, and fleet-update risk. It should follow the ESP-only containment rather than block it.

## Required tests

### Host tests

- all commands acknowledged in order;
- NAK at every transaction phase;
- missing ACK on first, middle, and final commands;
- bounded retry then success, and bounded retry then abort;
- unrelated AVR telemetry while a command is pending;
- duplicate/late ACK rejection or safe handling;
- maintenance keep-alive due during a transaction;
- queue-full and millisecond-wrap boundaries;
- no queued event frames after failure;
- no success when any NAK or timeout occurred; and
- source contract preventing unconditional `PRM` before `GO,2`.

### Pilot master/target tests

- ordinary cold start and browser availability;
- target with and without a master in range;
- repeated successful clone cycles with prune both enabled and disabled;
- retry after a deliberately interrupted clone without a power cycle;
- correct next-event name and role;
- exact programmed frequency and power readback where the protocol supports it;
- normal cleanup, AVR wake/sleep behavior, and no battery-draining ESP holdover; and
- Chrome behavior unchanged, with Safari and Firefox-compatible HTTP checks where available.

The pilot evidence must record phase timestamps and the new diagnostic state. A generic success message is not enough to qualify another fleet deployment.

## Decision

Proceed first with the ESP-only transaction containment and instrumentation. It addresses the observed intermittent failure mode without reopening repaired hardware or requiring the fleet's AVR processors to be reflashed. Defer semantic AVR ACKs and atomic commit until the next justified AVR service cycle.

## ESP 2.6 implementation

The low-risk Phase 1 containment is implemented without changing AVR source or the Linkbus wire protocol:

- event programming takes exclusive ownership while clone keep-alive insertion is paused;
- any existing queue work must drain before event programming begins;
- an acknowledged `$ESP,Z;` keep-alive and a fresh `$VER?` reply plus ACK prove both directions immediately before configuration;
- one event command is queued at a time and the next command is unreachable until the current command has completed cleanly;
- any NAK fails the transaction, and a command that required an ACK retry is conservatively rejected even if a later ACK arrives;
- a separate wrap-safe 12-second deadline prevents a transaction wait from becoming unbounded;
- failed transaction-owned work is removed instead of being left for clone cleanup or shutdown;
- the pre-validation `$PRM;` write is removed; successful `$GO,2;` retains the AVR's existing completeness check and changed-value persistence path; and
- `/firmware/status` now exposes transaction activity, last valid AVR-frame time, phase, attempts, and retained failure reason.

Detailed failure text continues through the existing target-to-master clone error path. Operational timeout/NAK flags are cleared after the failure is copied into the retained diagnostic fields, so a failed event operation does not poison a later unrelated Linkbus operation.

The correction does not change the AVR's broad ACK semantics or make its event update atomic in RAM. Those Phase 2/3 limitations remain deferred.

## Host and build evidence

The dependency-free transaction test began red because the production helper did not exist. It now covers queued and pending work, clean ACK, NAK with queued follow-up work, retry-rescued ACK rejection, the exact local deadline, null input, and `millis()` wrap.

The firmware source contract additionally requires exclusive ownership, clone-maintenance suppression, keep-alive and version preflight, retained diagnostics, one-command transaction use, and absence of the old `PRM` enqueue before activation.

Two consecutive exact pinned ESP8266 builds complete with zero warnings and produce the same sketch size and SHA-256. Compared with the preceding ESP 2.5 source commit `8b5d78f`:

| Resource | ESP 2.5 | ESP 2.6 candidate | Change |
| --- | ---: | ---: | ---: |
| Total sketch | 523,652 bytes | 524,592 bytes | +940 bytes |
| IROM | 486,276 bytes | 486,660 bytes | +384 bytes |
| IRAM | 27,676 bytes | 27,676 bytes | 0 |
| Dynamic memory | 41,588 bytes | 42,184 bytes | +596 bytes |
| DATA | 1,360 bytes | 1,360 bytes | 0 |
| RODATA | 8,340 bytes | 8,896 bytes | +556 bytes |
| BSS | 31,888 bytes | 31,928 bytes | +40 bytes |

The candidate retains 39,736 bytes of dynamic-memory headroom and 5,092 bytes of IRAM headroom. The sketch binary is 528,752 bytes with SHA-256 `78eb0232c08e30ad1654d450e2a8444ee6fb009b163309e93154c638fa00ad1b`.

Live qualification remains intentionally limited to sprint master and sprint fox 1 before any wider ESP deployment.

## Sprint master pilot

At `2026-07-17T14:17:40Z`, the protected Wi-Fi updater completed the first ESP 2.6 pilot on `Tx_Master` (`22:C8:8E:CF:AB:84`):

- the read-only preflight identified the unit as a master running ESP 2.5 and AVR 0.201;
- the updater established its bounded AVR heartbeat before accepting the sketch;
- the accepted binary was 528,752 bytes with CRC32 `6a39b59b` and SHA-256 `78eb0232c08e30ad1654d450e2a8444ee6fb009b163309e93154c638fa00ad1b`;
- post-reboot verification reported ESP 2.6, installed-sketch MD5 `893a3c947cdc8f302172a8f7558b0cb7`, and a reset uptime;
- the protected endpoint continued to report LittleFS preservation;
- a second live WebSocket probe identified the same unit as a master running ESP 2.6 and AVR 0.201 and returned fresh AVR temperature and battery data; and
- `/firmware/status` reported no active update or clone, no active event transaction, `linkbusLastAttempts: 1`, `linkbusLastPhase: "complete"`, and an empty `linkbusLastFailure`.

The pilot changed only the ESP sketch. It did not update the AVR or replace the ESP filesystem. Sprint fox 1 remains the second required pilot before wider deployment.

## Sprint fox 1 pilot installation

At `2026-07-17T14:30:36Z`, the second ESP 2.6 pilot was verified on the operator-selected sprint fox 1, identified as `Tx_7C2D5963` (`32:22:2C:F0:74:7D`):

- the read-only preflight identified a target running ESP 2.5 and AVR 0.201 and returned fresh AVR temperature and battery data;
- the updater established its bounded AVR heartbeat and accepted the same 528,752-byte ESP 2.6 binary used on the master;
- the unit did not return to HTTP within the updater's 240-second verification window, and the tether then reported the target network unreachable;
- after one operator power cycle and tether reconnection, the target reported ESP 2.6 and AVR 0.201 without a second upload;
- the installed sketch size and MD5 matched the master at 528,752 bytes and `893a3c947cdc8f302172a8f7558b0cb7`;
- LittleFS protection remained enabled; and
- fresh WebSocket and `/firmware/status` reads reported no active transaction, `linkbusLastAttempts: 1`, `linkbusLastPhase: "complete"`, and an empty `linkbusLastFailure`.

This proves that the accepted sketch survived the delayed-return/power-down condition and that neither an AVR update nor a repeated ESP write was necessary. A real master-to-target clone and its event/role result remain required before expanding the ESP 2.6 pilot to the fleet.

## Pilot clone result

At `2026-07-17T14:36:39Z`, the operator reported that the first master-to-target clone/sync succeeded. Two additional consecutive syncs under the requested no-power-cycle conditions also succeeded, for three successful operations in sequence with the target-only event cleanup option enabled. No ATMEGA communication error or shutdown was reported.

The tether was no longer reachable when a post-clone diagnostic read was attempted, so no additional firmware-status telemetry is attributed to these three operations. The operator-observed result satisfies the critical sprint master/sprint fox 1 pilot gate. Wider deployment should retain identity-first preflight, exact sketch verification, and per-unit post-reboot ESP/AVR checks.
