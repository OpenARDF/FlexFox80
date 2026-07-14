# AVR On-Air State Atomic Stores

**Date:** 2026-07-13

**Scope:** Foreground writes to `g_on_the_air`

**Status:** Focused source, exact-build, byte-verified programming, and connected-target gates pass

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

## Connected-target verification

The candidate was installed on the authorized AVR128DA48 test unit through Atmel-ICE `J41800053674` at 3.27 V. Two pre-program reads agreed. Programming used an explicit chip erase, candidate flash, and restoration of the captured EEPROM. Two independent post-program reads then agreed with these SHA-256 values:

| Memory | Before | Installed candidate |
| --- | --- | --- |
| Flash | `a095ac4d9d46553d9291002c16cd6c501a41d9a2410f524d4fe93d246eccd27d` | `d7f2c11755ea4b17232c7d1e9b4bfb3c00db048c5e944fd956927f435a9b2d1b` |
| EEPROM | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | unchanged |
| Fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | unchanged |

The installed image passed HTTP, WebSocket, identity, version, temperature, battery, and advancing-clock probes before the state-transition test.

The dummy load did not satisfy the firmware's antenna-detect input, so the first ordinary start request was safely rejected with error 247. The focused test therefore used the existing zero-power path:

1. `$POW,M,0;` returned `POWER,0`. Error 246 was expected because antenna-detect remained open; the implementation nevertheless establishes `g_tx_power_is_zero` and inhibits RF output.
2. `$GO,1;` returned the expected antenna warning 247 and then `STATUS,253`. The status is emitted only after the immediate-start handler publishes `9999` through the new atomic setter and enables the event.
3. After four seconds, `$KEY,^;` ran `suspendEvent()`, publishing zero through the same setter. Fresh `$TEM?` and `$BAT?` responses queued behind the stop frame prove that the ESP received the AVR acknowledgment and advanced the Linkbus queue only after the stop handler completed.
4. Two additional stop frames ran from the test's fail-safe cleanup path.

Atmel-ICE then reset the AVR so it reloaded the preserved EEPROM configuration. A final flash, EEPROM, and fuse read matched the installed candidate and both pre-test nonvolatile images byte-for-byte.

## Verification boundary

The focused gates establish atomic foreground publication, unchanged ISR bodies, equivalent local event-launch arithmetic, exact installation, and acknowledged immediate-start/stop execution without RF output. The test does not qualify antenna-detect behavior, RF amplitude, every scheduled-event branch, ID timing, sleep/wake, or long-duration cycling; those remain in A8.
