# AVR Wake-Time Atomic Stores

**Date:** 2026-07-13

**Scope:** Foreground writes to `g_time_to_wake_up`

**Status:** Source and exact-build gates pass; connected-target gate pending

## Ownership and risk

`g_time_to_wake_up` is a 32-bit `time_t` on an 8-bit AVR. Its ownership differs from the event start and finish epochs:

- foreground `eventEnabled()` writes one of three wake-time values while configuring event sleep behavior;
- the 300 Hz `TCB0` ISR writes the next wake time when it prepares an off-air sleep interval;
- the one-second `PORTA` ISR reads the value while the unit is sleeping.

`TCB0` and `PORTA` use the same normal interrupt priority, so they cannot preempt each other. The TCB0-to-PORTA transfer is already coherent and does not need another critical section. Foreground stores can be interrupted by `PORTA` between bytes, however, so a sleeping unit could compare a torn wake epoch.

The main loop cannot execute `eventEnabled()` while the CPU is asleep. Its wake-time configuration is complete before foreground later sets `g_sleeping`, so this slice needs to make only the four-byte foreground store atomic. It does not need to combine the adjacent sleep type or go-to-sleep flags into a larger transaction.

## Characterization-first regression

The firmware contract was added before implementation and failed with:

```text
Firmware contract check failed: foreground wake-time stores are not atomic to the RTC ISR
```

The contract distinguishes the existing TCB0 store from foreground stores. It requires:

- one direct definition, one direct TCB0 assignment, and one assignment inside the foreground setter;
- all three `eventEnabled()` branches to call the setter;
- no direct wake-time assignment inside `eventEnabled()`;
- interrupt masking around the setter's four-byte store;
- preservation of TCB0's existing `temp_time + seconds_to_sleep` assignment.

The passing result is:

```text
PASS foreground wake-time stores are atomic to the RTC ISR
```

## Implementation

`setWakeTimeFromForeground(value)` is private to `main.cpp`. The three `eventEnabled()` assignments now call it. The TCB0 ISR remains unchanged, as do the PORTA comparisons, sleep type assignments, and go-to-sleep sequencing.

## Exact-build evidence

All host tests and firmware contracts pass. The pinned AVR build completed with zero warnings and no resource growth relative to the event-epoch baseline:

| Resource | Baseline | Candidate | Delta |
| --- | ---: | ---: | ---: |
| Flash text | 41,044 bytes | 41,044 bytes | 0 |
| Initialized data | 1,112 bytes | 1,112 bytes | 0 |
| BSS | 1,572 bytes | 1,572 bytes | 0 |

Disassembly shows a 28-byte setter that saves `SREG`, executes `cli`, performs four byte stores, restores `SREG`, and returns. The generated `PORTA_PORT_vect` and `TCB0_INT_vect` sizes remain unchanged at `0x216` and `0x95a` bytes.

Candidate artifact SHA-256 values:

| Artifact | SHA-256 |
| --- | --- |
| ELF | `76655fd89bade3487d092c92e20fe9d92daddd5b60361b4cabefb01e45c15f21` |
| HEX | `a9d2a3b447ee350c482ec4a94338322e8e84d0e1defe1a6dbda81f97c77be0b0` |
| EEP | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| MAP | `317fa0164dce6c603206c47edcc8d259939e55d45f0cc17a8a560017a4d56a06` |
| LSS | `1fe13a6ce45e381239d682cb1c1b574b560ec8b1be2e61afda690cbfeabd9427` |
| SREC | `815d64043bdfcf6e85ce09f65d99e3466ff95ab23c210020596cbd50b2cf8f1f` |

## Verification boundary

The source and generated-code gates establish that foreground wake-time stores cannot be interrupted mid-value and that neither consuming ISR changed. Connected-target programming, state preservation, and startup observation remain the next focused gate. A complete scheduled sleep/wake cycle remains part of A8 rather than this narrow atomic-transfer check.
