# Linkbus Message-ID Length Characterization — 2026-07-12

## Status

Mac red-green, direct boundary, repository, and exact AVR Release build evidence is complete. Exact Windows and connected-target overlength-ID verification remain open.

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

## Remaining verification

- Obtain exact Windows same-source builds and the full Windows host-contract run.
- Program the connected dummy-loaded test unit through the proven flash/EEPROM-preservation workflow.
- Prove `$AZRX?` remains unanswered through the ESP retry cycle and that the following read-only temperature query succeeds.
