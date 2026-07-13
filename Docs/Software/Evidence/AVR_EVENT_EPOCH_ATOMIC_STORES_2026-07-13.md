# AVR Event-Epoch Atomic Stores

**Date:** 2026-07-13

**Scope:** Foreground writes to `g_event_start_epoch` and `g_event_finish_epoch`

**Status:** Source and exact-build gates pass; connected-target gate pending

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

## Verification boundary

Host tests, the firmware contract, and the exact build establish that foreground stores cannot be interrupted mid-value and that the generated interrupt bodies did not change. They do not yet establish connected-target programming or broader event behavior. The next focused gate is byte-verified programming with EEPROM and fuse preservation, followed by ordinary startup/WiFi observation. Classic, Sprint, foxoring, beacon, ID, sleep/wake, and event-boundary regression remain in A8.
