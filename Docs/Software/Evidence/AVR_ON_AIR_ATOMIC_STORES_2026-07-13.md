# AVR On-Air State Atomic Stores

**Date:** 2026-07-13

**Scope:** Foreground writes to `g_on_the_air`

**Status:** Source and exact-build gates pass; connected-target start/stop gate pending

## Ownership and risk

`g_on_the_air` is a signed 32-bit countdown and RF-cycle state value on an 8-bit AVR. Its ownership is more complex than the previously protected event epochs and wake time:

- the one-second `PORTA` ISR increments negative off-air counts, decrements positive on-air counts, and resets state at event edges;
- the 300 Hz `TCB0` ISR reads the count to drive RF and ID transitions and writes the next cycle state;
- foreground manual-mode, event-launch, suspend, and immediate-start paths publish initial or stopped state.

`PORTA` and `TCB0` share normal interrupt priority and cannot preempt one another. Their reads, read-modify-writes, and assignments are therefore coherent with each other and should not acquire new ISR critical sections. A foreground four-byte assignment can be interrupted by either ISR, however, exposing a value composed from old and new bytes.

Foreground reads occurred only in `activateEventUsingCurrentSettings()` immediately after the function calculated and assigned a new value. That function can retain the calculated value locally for its ID-count arithmetic and publish the result once.

## Characterization-first regression

The firmware contract was added before implementation and failed with:

```text
Firmware contract check failed: foreground on-air state stores are not atomic to ISR owners
```

The contract now requires:

- exactly the existing eight direct ISR assignments plus one assignment in the foreground setter;
- preservation of the PORTA increment and decrement operations;
- preservation of representative TCB0 cycle assignments;
- interrupt masking around the foreground setter's four-byte store;
- no direct shared-state reference in the event-launch calculation;
- the four foreground publication sites for manual, calculated launch, suspend, and immediate-start state.

The passing result is:

```text
PASS foreground on-air state stores are atomic to ISR owners
```

## Implementation

`setOnTheAirFromForeground(value)` is private to `main.cpp`. Six foreground assignments now publish through it. `activateEventUsingCurrentSettings()` calculates `on_the_air` locally, uses that same local for the existing ID-count formulas, and publishes it once after the three mutually exclusive cycle-position branches.

No ISR source was changed. The on-air/off-air formulas, comparisons, event flags, RF calls, and ID-count arithmetic are unchanged.

## Exact-build evidence

All host tests and firmware contracts pass. The pinned AVR build completed with zero warnings and reduced flash use relative to R12:

| Resource | R12 baseline | Candidate | Delta |
| --- | ---: | ---: | ---: |
| Flash text | 41,044 bytes | 40,992 bytes | -52 bytes |
| Initialized data | 1,112 bytes | 1,112 bytes | 0 |
| BSS | 1,572 bytes | 1,572 bytes | 0 |

Disassembly shows a 28-byte setter that saves `SREG`, executes `cli`, performs four byte stores, restores `SREG`, and returns. The generated `PORTA_PORT_vect` and `TCB0_INT_vect` sizes remain unchanged at `0x216` and `0x95a` bytes.

Candidate artifact SHA-256 values:

| Artifact | SHA-256 |
| --- | --- |
| ELF | `97dad2855f30023850a659ffdbe17c28adda6528d064f6538d6dacb475c00d51` |
| HEX | `d0e8847f6cad005aa499447a501bee421aceb38b98cdd596c0dd65698fcfb229` |
| EEP | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| MAP | `e4012467d62f6863458d0a6c950ce23c6636a07908e15d03143704e58ffcb11d` |
| LSS | `44230700ea8c332d9ab977bd488ed7bc3fea804ca7fce209791782fbe978bde3` |
| SREC | `92592df734db9e3d8c7a1681af19afe9132ad64696ea735adb173834a8d84dda` |

## Verification boundary

The source and generated-code gates establish atomic foreground publication, unchanged ISR bodies, and equivalent local event-launch arithmetic. Because this shared value directly drives RF cycling, the focused target gate should include byte-verified programming and a controlled start/stop observation on the authorized dummy-loaded unit. Broader event patterns, ID timing, sleep/wake, and long-duration cycling remain in A8.
