# Linkbus Message-ID Length Characterization — 2026-07-12

## Status

Mac red-green, direct boundary, repository, exact AVR Release build, connected-target programming, and overlength-ID rejection evidence is complete. Exact Windows verification remains open.

## Confirmed defect

Every defined Linkbus message ID contains one to three characters, but `linkbus_Rx()` previously accumulated an unlimited number of ID bytes in a `uint32_t` and later cast the result to the 16-bit `LBMessageID` enum.

The encoding multiplies the current value by ten and adds each uppercase character value. A four-character label can therefore exceed 16 bits and truncate to a valid command ID. Exhaustive uppercase-label checking found concrete aliases:

| Four-character input | 32-bit accumulator | Truncated 16-bit value | Aliased ID |
| --- | ---: | ---: | --- |
| `AOSU` | `0x12057` | `0x2057` / 8279 | `KEY` |
| `AWVZ` | `0x1239A` | `0x239A` / 9114 | `RST` |
| `AZRX` | `0x1249C` | `0x249C` / 9372 | `VER` |

The first two aliases reach state-changing handlers. The third provides a read-only target reproducer: before correction, `$AZRX?` is interpreted as the version query even though `AZRX` is not a defined ID.

## Completed TDD slice

The firmware source contract was extended first to require an ID-length guard in the receive parser. Against the pre-fix source, `just test` failed red with:

```text
Firmware contract check failed: Linkbus receive parser lacks linkbus_rx_id_can_append bounds guard(s)
```

The production correction:

- defines `LINKBUS_MAX_MSG_ID_LENGTH` as 3 alongside the existing Linkbus dimensions;
- tracks the current ID length independently from field and aggregate message lengths;
- accepts the existing one-to-three-character ID forms unchanged;
- rejects a fourth ID byte before it is incorporated into the accumulator;
- leaves the receive buffer unpublished and relies on the existing next-frame start marker for resynchronization.

The direct host regression compiles the same inline predicate used in `linkbus_Rx()` and covers lengths zero through three. Existing field-width, field-count, literal text-copy, EEPROM, and circular-buffer regressions remain green.

The constrained target test now includes `$AZRX?`. It also requires rejected frames to remain unanswered for at least six seconds before the queued read-only recovery query succeeds. This distinguishes rejection plus the ESP retry policy from the immediate version reply that the pre-fix alias would produce.

## Exact Mac AVR build

Two consecutive final-source builds used AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103. Both completed with zero warnings and byte-identical hashes for all six artifacts:

```text
   text    data     bss     dec     hex
  40278    1106    1138   42522    a61a
```

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `ff8f03587cc341623cc408a6dcfc8e8452bfc2c7b93d523c64241d7fa99ab7f6` |
| `FlexFox80.hex` | `44254d519cf8e3bc2dae82765ea9827672485d70470467b20db612fa5a8eb36c` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `f446633a51037ff9c88a1b8eaaa940ba5161067d0e6e664860b790c07ff7e4cb` |
| `FlexFox80.lss` | `f883b7780081842892b49705f9f6f2ad0f5b61b66e033f251a0bb06693936b36` |
| `FlexFox80.srec` | `a6def97511e186c3577de7c9a80522057c06ed56d3d8c175ccfc30c81ad64be7` |

Compared with the preceding field-boundary build, text grows by 22 bytes and BSS by one byte for the parser's ID-length state. Data is unchanged, and the EEPROM initializer remains byte-identical.

## Connected-target programming

The dummy-loaded AVR128DA48 test unit was programmed with the exact `912d24b` Release HEX through the proven chip-erase, flash-write/verify, and complete EEPROM-restore/verify workflow.

Fresh pre-write captures matched the preserved unit baseline:

- EEPROM, 512 bytes: `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401`;
- fuses, 16 bytes: `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126`.

Avrdude verified all 41,110 input flash bytes twice. Independent post-operation reads then proved:

| Memory | Expected SHA-256 | Post-read SHA-256 | Result |
| --- | --- | --- | --- |
| Programmed flash bytes | `19f0ccb3f9b7ff14554327f2aafb8da2696970e1bd1efa9e5be5bab539356574` | `19f0ccb3f9b7ff14554327f2aafb8da2696970e1bd1efa9e5be5bab539356574` | byte-identical |
| Restored EEPROM | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

The first expanded Linkbus test attempt timed out at HTTP after programming, before its WebSocket opened or any test frame was sent. This matches the known Moto/DroidTether association loss after extended Atmel-ICE activity and is not counted as a firmware result.

After restoring the Moto/DroidTether route, the expanded target test passed:

1. HTTP 200 and WebSocket connection succeeded.
2. Initial `TEMP,33.0C` and `BAT,12.2V` replies established the live AVR path.
3. A baseline raw `PASS,$TEM?` produced a fresh `TEMP,33.0C` reply.
4. The existing oversized-field and fourth-field cases remained unanswered through the ESP retry cycle, then each following `$TEM?` produced a fresh reply.
5. Overlength ID `$AZRX?` remained unanswered through the same retry cycle instead of producing the immediate `VER` response that its pre-fix 16-bit alias would generate.
6. The following `$TEM?` produced a fresh `TEMP,34.0C` reply, proving next-frame resynchronization after rejecting the overlength ID.

The harness requires at least six seconds of unanswered delay for each malformed frame. This directly distinguishes rejection plus the ESP's existing retry policy from an acknowledged aliased command or query.

## Remaining verification

- Obtain exact Windows same-source builds and the full Windows host-contract run.
