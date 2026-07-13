# AVR Clock-Edge and Jitter Audit

**Date:** 2026-07-13

**Scope:** AVR128DA48 system-clock reads, RTC-edge handling, interrupt latency, and shared schedule state

**Status:** Static review complete; highest-priority recovery candidate passes host and exact-build gates, with target verification pending

## Reason for this review

The edge-aligned boot correction removed the reproduced reset-dependent whole-second quantization, and the first two corrected clones completed with repeatable target relationships. Long-duration drift and DS3231 aging work are now deliberately postponed. This review instead asks whether the AVR can still lose, delay, or inconsistently consume a one-second transition after boot.

The review covered every direct `time()`, `set_system_time()`, and `ds3231_get_epoch()` reference in the AVR application, the RTC and timer interrupt paths, Si5351 keying, I2C timeout behavior, event-boundary helpers, and multi-byte time-related globals shared between foreground and interrupts.

## Main conclusion

There is no second ordinary `time()` call that independently rephases the system clock. The pinned AVR libc protects its 32-bit system-time read and write with interrupt masking, so callers receive a coherent integer second. A foreground report may be delivered late, but reading the system clock does not itself move that clock.

The most important remaining risk is earlier in the chain: **the RTC rising-edge interrupt can be blocked by retrying I2C work inside another normal-priority interrupt**. If that blockage spans more than one RTC edge, the port interrupt flag records only that an edge is pending; it does not count how many edges occurred. The eventual RTC handler calls `system_tick()` only once, permanently losing one or more seconds from AVR system time. That is a credible rare multi-second schedule-error mechanism and is materially more important than expected DS3231 drift over the currently relevant interval.

This is source-supported risk, not yet a reproduced hardware root cause for the field outlier. It should be addressed through characterization and fault injection before changing the mature transmit path.

A narrow recovery candidate now counts RTC rising edges from the already-running Level-1 timeout timer and replays the existing one-second handler for each elapsed edge. It does not reorganize RF keying or enable clocks during standby. Design and verification evidence is in [AVR RTC edge recovery](AVR_RTC_EDGE_RECOVERY_2026-07-13.md).

## Findings

### 1. Critical: retrying Si5351 I2C runs inside normal-priority ISRs

The RTC square wave is a rising-edge interrupt on `PORTA`. Its handler calls `system_tick()` once per serviced port flag. `TCB0`, which runs at 300 Hz and drives Morse and event transitions, has the same normal interrupt priority.

The following call paths can perform polling and retrying I2C while normal-priority interrupts are blocked:

- `TCB0_INT_vect` -> `keyTransmitter()` -> `si5351_clock_enable()` -> `si5351_read_bulk()` / `si5351_write_bulk()` -> I2C polling;
- `PORTA_PORT_vect` -> `handle_1sec_tasks()` -> event finish -> `keyTransmitter(OFF)` -> the same I2C path;
- other `TCB0` RF/enunciation transitions also call `keyTransmitter()`.

The I2C timeout counter does continue to run because `TCB2` is explicitly assigned the sole Level-1 interrupt vector. At the configured 24 MHz peripheral clock, `TCB2` uses clock/2 and a 65,536-count period, so one timeout tick is approximately 5.46 ms. Each I2C polling wait loads 50 ticks, approximately 273 ms. The Si5351 bulk helpers make as many as five attempts, and `keyTransmitter()` adds as many as five outer recovery attempts with transmitter shutdown/restart work between them. The operation is therefore bounded only at a duration that can readily exceed one second during a peripheral fault.

Consequences:

- a successful, short I2C transaction can still delay an RTC edge by the remaining duration of the active `TCB0` ISR, creating small RF/schedule-edge jitter;
- a timeout/retry path can delay `PORTA_PORT_vect` across multiple RTC edges;
- repeated physical edges coalesce into one pending `PORTA` flag;
- `system_tick()` then advances once rather than once per elapsed second;
- all later `time()` reads are coherent but permanently behind by the number of lost ticks until the next explicit RTC resynchronization or reset.

This mechanism can produce whole-second error without a bad RTC write, checksum failure, or long-term oscillator drift. It also explains why auditing only call sites of `time()` would miss the most consequential path.

**Candidate status:** Host tests cover ISR/sampler order, coalesced edges, counter wrap, and sleep restart. Firmware contracts and an exact zero-warning AVR build pass. Hardware delay/fault injection remains required before this finding is closed.

### 2. High: several 32-bit schedule values cross foreground/ISR boundaries without atomic transfer

The AVR is an 8-bit processor. `volatile` prevents optimization across accesses but does not make a 32-bit `time_t` or `int32_t` access atomic.

The following values are shared across contexts without a consistent critical-section or snapshot contract:

- `g_event_start_epoch` and `g_event_finish_epoch` are written by foreground Linkbus/configuration code and read by the RTC and timer interrupt paths;
- `g_time_to_wake_up` is written by foreground or `TCB0` and read by the RTC interrupt;
- `g_on_the_air` is a 32-bit countdown/state value used by foreground, `TCB0`, and the RTC interrupt.

The two normal-priority ISRs do not preempt each other, but either can interrupt a foreground multi-byte read or write. A partially updated epoch can therefore be observed at an event start, finish, or wake decision. Clone quiet mode suppresses reports and owns the one-shot clock state correctly, but it does not make event start/finish updates transactional.

The clone one-shot epoch is a useful positive example: `serviceCloneSyncReport()` copies `g_clone_sync_epoch` inside a critical section before using it in foreground.

### 3. High robustness risk: RTC synchronization waits forever and represents an edge with one bit

`syncSystemTimeToRTC()` clears `g_seconds_transition`, spins until the RTC ISR sets it, reads the RTC, and calls `set_system_time()`.

It has two weaknesses:

- if the square wave stops, the foreground hangs indefinitely;
- a Boolean can say only that at least one edge occurred, not how many elapsed or whether an edge was coalesced while interrupts were blocked.

There is also a fault-sensitive read/set window. Under normal I2C timing, the RTC read and `set_system_time()` complete comfortably before the next second. If RTC I2C retries extend that operation across another edge, the helper can install a stale epoch after the later edge has already been serviced.

The unbounded wait was already listed in the general reliability review. The ISR/I2C audit raises its timing significance: a bounded synchronization helper should use a counted edge observation and reject a read whose edge generation changed during the transaction.

### 4. Medium: exact event boundaries use inconsistent comparisons and multiple `now` snapshots

The actual RTC event-start and event-finish decisions use `>=`, which is appropriate for not missing a delayed edge. Some foreground status helpers differ:

- `eventScheduledForNow()` requires `start < now` and `finish > now`;
- `eventScheduledForTheFuture()` requires `start > now`;
- at `now == start`, both return false;
- `eventScheduled()` reads `now`, then calls both helpers, each of which reads `time()` again.

`clockConfigurationCheck()` likewise uses strict `now > start` and `now > finish`. These paths can report inconsistent state for the exact boundary second and can straddle a transition because they do not share one captured `now`. The RTC ISR remains the schedule owner, so this is not presently a demonstrated persistent phase shift, but it is a concrete boundary defect suitable for deterministic host tests.

### 5. Low phase risk: ordinary clock reports are not edge timestamps

The periodic report path notices that `time()` changed and then formats/sends the value from foreground. `$ESP,0` also sends the current integer epoch immediately when WiFi announces that it is awake. Linkbus, ESP, WiFi, tunnel, and host scheduling can delay receipt.

These paths explain observation jitter, including isolated long delivery tails, but they do not alter the RTC or AVR system clock. The clone path no longer depends on them: its one-shot epoch is captured in the RTC ISR and copied atomically before foreground transmission.

## Recommended TDD checkpoints

No ISR or RF behavior should be reorganized from static reasoning alone. The next safe sequence is:

1. **Capture the current timing contract.** Add a source/call-graph regression that identifies every I2C-capable call reachable from `PORTA_PORT_vect` and `TCB0_INT_vect`. Record the current red result.
2. **Measure the normal path.** Instrument RTC-edge arrival, RTC-handler entry, `TCB0` entry/exit, and Si5351 transaction duration on the dummy-loaded unit. Establish normal and worst observed latency before changing priorities or RF keying.
3. **Reproduce the fault path safely.** Force Si5351 NACK/timeout behavior with RF inhibited and confirm whether RTC edge count and AVR system time lose seconds. Preserve reset cause and error state.
4. **Separate edge capture from slow work.** The current candidate completes the counted-edge portion using the already-running Level-1 timeout timer and leaves RF keying unchanged. Do not transmit or perform general I2C from the higher-priority sampler. Hardware fault injection remains the gate.
5. **Make 32-bit transfers explicit.** Introduce small atomic snapshot/update helpers and apply start/finish configuration as a coherent transaction before enabling the event.
6. **Add boundary tests.** Exercise `start-1`, `start`, `start+1`, `finish-1`, `finish`, and `finish+1` using one supplied `now` value.
7. **Bound RTC synchronization.** Test missing square wave, I2C timeout, an intervening edge during readback, and success at the expected edge before changing `syncSystemTimeToRTC()`.
8. **Repeat hardware timing gates.** Re-run corrected reset and clone qualification, plus classic, Sprint, foxoring, beacon, ID, sleep/wake, and event-finish traces.

## Priority decision

Long-duration drift and aging-register work remains bookmarked. The RTC edge-loss candidate is now source-verified; its next gate is dummy-loaded target verification and delayed-ISR fault injection. After that gate, the next AVR timing-hardening slice is explicit atomic transfer of shared schedule values. These changes target rare discrete jitter and whole-second loss, which are better aligned with the observed failure class than expected short-term oscillator drift.
