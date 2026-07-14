# AVR Bounded RTC Synchronization Guard

**Date:** 2026-07-13

**Scope:** `syncSystemTimeToRTC()` and the event-launch RTC-edge wait

**Status:** Source, host-test, firmware-contract, generated-code, and exact-build gates pass; isolated connected-target fault injection pending

## Confirmed failure modes

The original helper cleared `g_seconds_transition` and spun forever until the normal-priority RTC port ISR set the Boolean. A stopped DS3231 square wave, broken connection, disabled port interrupt, or corrupted pin configuration could therefore hang the foreground permanently even though the CPU timers and WiFi module still worked.

The helper then read the RTC and unconditionally installed any successful result. If the read and its retries crossed another RTC rising edge, the ISR could advance system time and the helper could subsequently replace it with an epoch sampled for the older edge.

There was one additional direct infinite wait in `activateEventUsingCurrentSettings()`. It did not read the RTC, but had the same missing-square-wave hang.

## Caller boundary

There are seven active calls to `syncSystemTimeToRTC()`:

- boot alignment;
- Linkbus clock write and clock query;
- `startEventUsingRTC()`;
- initial `setupForFox()` alignment;
- `START_EVENT_NOW_AND_RUN_FOREVER` setup;
- configuration-error reporting.

The Linkbus clock-write and run-now paths already inspect the returned error. Other legacy callers continue from the existing system time on an error; they no longer hang and the helper never installs an unqualified RTC value. Scheduled event activation now has its own bounded edge result and propagates `ERROR_CODE_RTC_NONRESPONSIVE`, so it does not enter timing calculations without a fresh edge.

This slice deliberately does not reorganize the mature caller workflows or introduce a new error code. Missing-edge, failed-read, and edge-crossed-read failures all use the existing `ERROR_CODE_RTC_NONRESPONSIVE` result.

## TDD characterization

The host test and runner integration were added before the guard existed. The red result was the expected compile failure:

```text
fatal error: 'rtc_sync_guard.h' file not found
```

The passing host contract now proves:

- a missing edge waits only while the timeout timer is active;
- a fresh edge completes the wait;
- an edge wins at the timeout boundary;
- a missing edge reaches an explicit timeout state;
- only a successful RTC read from an unchanged edge generation can commit;
- I2C failure, an intervening edge, and generation-counter wrap are rejected.

The existing edge-tracker tests now also verify that the exposed one-byte generation follows observed rising edges across counter wrap.

## Implementation

`waitForRTCSecondTransition()` clears the transition flag, resets the existing CPU-clock utility timer, and waits at most 1,500 ms. The timeout comfortably spans the worst normal wait for the next one-second rising edge while remaining independent of that RTC signal. It returns the existing RTC-nonresponsive error on timeout.

`syncSystemTimeToRTC()` uses that bounded wait, captures the counted RTC-edge generation, reads the DS3231, and captures the generation again. It calls `set_system_time()` only when the read succeeds and both one-byte generations match. `rtcEdgeGeneration()` temporarily masks only the Level-1 sampler, observes the live RTC pin, snapshots the one-byte generation, and restores the sampler state; pending interrupts remain pending.

The direct event-activation spin now calls the same bounded edge wait and returns its error before using `time()`.

## Exact-build evidence

All host tests and firmware contracts pass. The pinned AVR-GCC 7.3.0 / AVR-Dx DFP 1.9.103 Release build completed with zero warnings:

| Resource | R13 baseline | Candidate | Delta |
| --- | ---: | ---: | ---: |
| Flash text | 40,992 bytes | 41,154 bytes | +162 bytes |
| Initialized data | 1,112 bytes | 1,112 bytes | 0 |
| BSS | 1,572 bytes | 1,572 bytes | 0 |

The generated `PORTA_PORT_vect` and `TCB0_INT_vect` sizes remain unchanged at `0x216` and `0x95a` bytes. Generated symbols are 94 bytes for `syncSystemTimeToRTC()`, 68 bytes for the bounded wait, and 60 bytes for the race-safe edge-generation snapshot.

Candidate artifact SHA-256 values:

| Artifact | SHA-256 |
| --- | --- |
| ELF | `b3a6dfe95103a1d082fbf91a644a830b82b35f775b8e1eb3179baf6974f489eb` |
| HEX | `e37af24292275bcf4479cc317c8b07919b1b57a614fc555d65286cef8d6637d5` |
| EEP | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| MAP | `2a115d25961f14c019bb1166234c12487a9a33af9f2658910116ec3b17d0b3c3` |
| LSS | `c536cc5aa84fff020ea3e30d72d2d5297d19637c38e7050d330ac7f40cdfcc80` |
| SREC | `fd1e7fcb29d52f4d8cb2a0b57d23eba7b1ca597dc2198ef56b0be029d361db7c` |

## Connected-target gate

Use an isolated fault-injection image on the authorized test unit, with RF inhibited and EEPROM preserved:

1. suppress the DS3231 square wave after normal boot and prove the helper returns error 252 in approximately 1.5 seconds while WiFi/foreground processing recovers;
2. force a delay longer than one second between RTC read and commit check and prove the changed generation rejects the stale read;
3. restore the production candidate, then verify flash exactly and EEPROM/fuses byte-for-byte against their pre-test images;
4. re-run normal clock-set/readback and advancing-time probes.

No target image should be retained unless all restoration and normal-path gates pass.
