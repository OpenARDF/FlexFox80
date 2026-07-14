# AVR Wake-Time Atomic Stores

**Date:** 2026-07-13

**Scope:** Foreground writes to `g_time_to_wake_up`

**Status:** Source, exact-build, byte-verified target-programming, and focused runtime gates pass

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

## Connected-target verification

The committed candidate at `4afd355` was programmed on the authorized dummy-loaded test unit through Atmel-ICE `J41800053674`. The target identified as AVR128DA48 signature `1E 97 08`, silicon revision 1.7, at 3.27 V.

Two independent pre-program reads matched byte-for-byte. They confirmed that the target still carried the exact R11 image and preserved state:

| Region | Bytes read | Pre-program SHA-256 |
| --- | ---: | --- |
| Programmed flash span | 41,882 | `6b623e216d0aabb1eca72ad71b6e0b3d01517e1dc32b4fdd48bd4a3345a269d1` |
| Complete EEPROM | 512 | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` |
| Complete fuse memory | 16 | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` |

The target was explicitly erased, the committed HEX was written and verified twice in the programming session, and the captured 512-byte EEPROM was restored. Fuses were not written. The R12 candidate binary programmed span is 41,882 bytes with SHA-256 `a095ac4d9d46553d9291002c16cd6c501a41d9a2410f524d4fe93d246eccd27d`.

Two new post-program sessions independently read flash, EEPROM, and fuses. Both reads matched each other and established:

- target flash exactly matches the committed R12 candidate binary;
- all 512 EEPROM bytes exactly match the pre-program capture;
- all 16 fuse bytes exactly match the pre-program capture.

After the Moto route was restored, the read-only runtime probe passed cleanly. It received HTTP 200, opened the WebSocket, returned AVR temperature `28.0C` and battery `11.4V`, identified SSID `Tx_Master`, software versions `2.0,0.200`, and master role `1`, and observed advancing clock reports from epoch `1783987788` through `1783987798`.

## Verification boundary

The source and generated-code gates establish that foreground wake-time stores cannot be interrupted mid-value and that neither consuming ISR changed. Byte-verified target programming establishes exact flash installation with complete EEPROM and fuse preservation, while the runtime probe establishes ordinary startup and ESP-to-AVR telemetry. A complete scheduled sleep/wake cycle remains part of A8 rather than this narrow atomic-transfer check.
