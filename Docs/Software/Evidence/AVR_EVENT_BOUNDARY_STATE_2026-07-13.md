# AVR Event-Boundary State

**Date:** 2026-07-13

**Scope:** Foreground event-window predicates and `clockConfigurationCheck()`

**Status:** Red/green host tests, firmware contract, generated-code review, and two exact builds pass; connected-target gate pending

## Confirmed defect

The RTC interrupt starts and finishes scheduled events with `now >= boundary`. The foreground status helpers did not share that contract:

- `eventScheduledForNow()` required `start < now` and `finish > now`;
- `eventScheduledForTheFuture()` required `start > now`;
- both therefore returned false at the exact start second;
- `clockConfigurationCheck()` used strict `now > start` and `now > finish`, treating the exact start as waiting and the exact finish as potentially active;
- `eventScheduled()` read `time()` itself and then called helpers that each read `time()` again, so one decision could span a second transition.

This is a deterministic one-second status/control boundary defect. It does not move the RTC or system clock and is not evidence of a persistent phase shift, but its inconsistent results can influence foreground sleep, button, startup, and reporting decisions at an exact event boundary.

## TDD gate

The boundary test and host-runner integration were added before the production helper existed. The red result was the expected compile failure:

```text
fatal error: 'event_schedule_state.h' file not found
```

The green test uses one supplied `now` value and covers:

- invalid current time, invalid start, empty window, and reversed window;
- `start-1` is future;
- `start` and `start+1` are active;
- `finish-1` is active;
- `finish` and `finish+1` are finished;
- the future, active, and overall-scheduled predicates agree with the same position.

## Implementation

The dependency-free `event_schedule_state.h` classifies one event window as invalid, future, active, or finished. Its comparisons mirror the RTC ISR: the active interval is `start <= now < finish`.

Each legacy foreground wrapper remains available, captures `time()` exactly once, and delegates its decision using that value. `eventScheduled()` no longer nests either other wrapper. `clockConfigurationCheck()` captures one `now`, maps invalid and finished to the existing configuration-error result, and maps active/future through its existing enabled and RTC-use decisions. No ISR, epoch storage, event setup, RF, or persistence code changed.

A source contract requires the ISR-aligned comparisons, exactly one `time()` call in every wrapper, direct use of the shared decision, and no nested schedule-helper calls.

## Exact-build evidence

Two consecutive pinned AVR-GCC 7.3.0 / AVR-Dx DFP 1.9.103 Release builds completed with zero warnings and byte-identical evidence:

| Resource | R14 baseline | Candidate | Delta |
| --- | ---: | ---: | ---: |
| Flash text | 41,154 bytes | 41,010 bytes | -144 bytes |
| Initialized data | 1,112 bytes | 1,112 bytes | 0 |
| BSS | 1,572 bytes | 1,572 bytes | 0 |

The compiler inlined and consolidated the shared decision. The retained `eventScheduled()` symbol is 170 bytes and `clockConfigurationCheck()` is 180 bytes. The generated `PORTA_PORT_vect` and `TCB0_INT_vect` sizes remain unchanged at `0x216` and `0x95a` bytes.

Candidate artifact SHA-256 values:

| Artifact | SHA-256 |
| --- | --- |
| ELF | `735fe2b8cb3f29046a8ba6172ca76f0c13a9104127400f2c2ee0f430c0e38a3b` |
| HEX | `db2c4125fb96b80e2fe9b11204ef28bcb2661451895591f60c43896df04cdcda` |
| EEP | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| MAP | `6f24be84f2a0cc868ab1835d8a0664de7295939274a0309b7ba3ea79788d0df8` |
| LSS | `2dbc7a33d41fc12c9eb894730ae18d2124aa00122bb5d11b47faa0486f378108` |
| SREC | `d5f074f967d67963c35b2ce01176a095df731b3b09899b96b2336250a5effac6` |

The 41,848-byte raw candidate flash image has SHA-256 `a02b6b92bbb5a5d5f4284e53553688b680ab9cf37720a376a40f7f4ed013a26d`. The EEPROM artifact is unchanged from R14.

## Connected-target gate

After committing the source/build checkpoint:

1. independently read and hash production flash, EEPROM, and fuses twice;
2. program the exact candidate while preserving EEPROM and fuses;
3. independently read the installed regions twice and require exact candidate flash plus byte-for-byte preserved EEPROM/fuses;
4. require ordinary HTTP, WebSocket, temperature, battery, version, role, and advancing-clock telemetry;
5. retain exact scheduled start/finish runtime behavior for the broader A8 event-mode regression, because exercising a real transmit event is outside this predicate-only slice.

Do not retain the candidate if any exact restoration or normal-path gate fails.
