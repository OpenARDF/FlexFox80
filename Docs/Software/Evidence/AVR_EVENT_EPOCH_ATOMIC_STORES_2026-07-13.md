# AVR Event-Epoch Atomic Stores

**Date:** 2026-07-13

**Scope:** Foreground writes to `g_event_start_epoch` and `g_event_finish_epoch`

**Status:** Source, exact-build, byte-verified target-programming, and focused runtime gates pass

## Risk addressed

The AVR128DA48 is an 8-bit processor, while `time_t` is 32 bits in the pinned build. The event start and finish epochs are written by foreground configuration paths and read by the one-second and 300 Hz interrupt paths. `volatile` preserves the accesses but does not make a four-byte store atomic. An interrupt could therefore observe bytes from both the old and new epoch and make an incorrect start, finish, or transmit-state decision.

This slice changes only the foreground stores. It does not reorganize either interrupt, alter event comparisons, change persisted values, or change the Linkbus protocol.

## Characterization-first regression

The firmware contract was added before the implementation and failed with:

```text
Firmware contract check failed: foreground event epoch stores are not atomic to ISR readers
```

The contract now checks that:

- all application assignments to the two epochs are confined to their definitions and the three update helpers;
- the EEPROM manager does not assign either shared epoch directly;
- the individual start and finish helpers mask interrupts around their four-byte stores;
- the paired helper masks interrupts across both stores, so a start/finish configuration becomes visible as one coherent transaction.

The passing result is:

```text
PASS event epoch stores are atomic to ISR readers
```

## Implementation

`event_time_state.h` exposes three deliberately small operations:

- `setEventStartEpoch(value)`;
- `setEventFinishEpoch(value)`;
- `setEventEpochs(start, finish)`.

The manual-mode configuration, start-now path, Linkbus start/finish updates, EEPROM restore, and EEPROM-default initialization use those helpers. Paths that logically establish both bounds use the paired helper. The start-now path retains its previous rule: if the new start is later than the existing finish, finish becomes one day after start.

No pointer alias writes were found. `validateTimeString()` compares the epoch pointer only to identify the field, and the EEPROM update helper reads the globals when persisting them.

## Exact-build evidence

The pinned AVR build completed with zero warnings. Relative to the source baseline immediately after RTC edge recovery:

| Resource | Baseline | Candidate | Delta |
| --- | ---: | ---: | ---: |
| Flash text | 41,084 bytes | 41,044 bytes | -40 bytes |
| Initialized data | 1,112 bytes | 1,112 bytes | 0 |
| BSS | 1,572 bytes | 1,572 bytes | 0 |

The compiler consolidated the repeated critical sections, so the change reduces rather than increases flash use. The generated helper sizes are 28 bytes for each individual setter and 44 bytes for the paired setter. Disassembly shows each helper saving `SREG`, executing `cli`, performing four or eight byte stores, restoring `SREG`, and returning. The `PORTA_PORT_vect` and `TCB0_INT_vect` sizes are unchanged at `0x216` and `0x95a` bytes respectively.

Candidate artifact SHA-256 values:

| Artifact | SHA-256 |
| --- | --- |
| ELF | `9d8b12084409487d4459828bd581855e6cc1e7e567fe78f6a683dc883854d2de` |
| HEX | `5580882e48d68fa1f39263dc2ae1fb3719f9af9301086400002cc4395c8ad6e7` |
| EEP | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| MAP | `194a9ff88081d6cbb16155d2f9352238725b9b95c5c4eda8b991a2f39e4d3bbf` |
| LSS | `f339e565700340a64480e37d744d9053d85d4c02c66eabb111a2b39ee3879930` |
| SREC | `1e2db28032c81ac781167e53de637578ca158268568bf0cf28c252352838acc6` |

## Connected-target verification

The committed candidate at `87c7332` was programmed on the authorized dummy-loaded test unit through Atmel-ICE `J41800053674`. The target identified as AVR128DA48 signature `1E 97 08`, silicon revision 1.7, at 3.27 V.

Two independent pre-program reads matched byte-for-byte and retained the previously qualified production state:

| Region | Bytes read | Pre-program SHA-256 |
| --- | ---: | --- |
| Programmed flash span | 41,922 | `b8d7bb66de33a59ce57777734ef27cbf0e2ceda302f9b41fb3dacfffc499c3f8` |
| Complete EEPROM | 512 | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` |
| Complete fuse memory | 16 | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` |

The target was explicitly erased, the committed HEX was written and verified twice in the programming session, and the captured 512-byte EEPROM was restored. Fuses were not written. The candidate binary programmed span is 41,882 bytes with SHA-256 `6b623e216d0aabb1eca72ad71b6e0b3d01517e1dc32b4fdd48bd4a3345a269d1`.

Two new post-program sessions independently read flash, EEPROM, and fuses. Both reads matched each other. They also established:

- target flash exactly matches the committed candidate binary;
- all 512 EEPROM bytes exactly match the pre-program capture;
- all 16 fuse bytes exactly match the pre-program capture.

After the Moto route was restored, a read-only runtime probe received HTTP 200, opened the WebSocket, and returned AVR temperature `26.0C`, battery `11.5V`, advancing clock reports, SSID `Tx_Master`, software versions `2.0,0.200`, and master role `1`. The tunnel disconnected after the functional assertions and several additional clock reports, so the harness exited nonzero; an immediate retry timed out before contact. This is recorded as a transport tail rather than a clean harness pass. The successful HTTP, AVR telemetry, and advancing clock reports establish ordinary startup and ESP-to-AVR communication for this focused gate.

## Verification boundary

Host tests, the firmware contract, the exact build, byte-verified target programming, and focused startup observation establish that foreground stores cannot be interrupted mid-value, generated interrupt bodies did not change, device state was preserved, and the firmware boots with ordinary ESP-to-AVR telemetry. This focused gate does not exercise a complete event or substitute for Classic, Sprint, foxoring, beacon, ID, sleep/wake, and event-boundary regression retained in A8.
