# Linkbus Receive Bounds Characterization — 2026-07-12

## Status

Mac red-green, direct boundary, repository, exact AVR Release build, and connected-target programming evidence is complete. Exact Windows and connected-target malformed-frame verification remain open.

## Confirmed defect

`linkbus_Rx()` receives bytes in a USART interrupt and stores up to three fields in:

```text
char fields[3][21]
```

The prior parser incremented `field_len` and `field_index` without checking either array boundary. Two independent malformed inputs could therefore write outside the receive buffer:

- more than 20 payload bytes in any field, because byte 21 occupies the NUL terminator position and later bytes exceed the 21-byte row;
- a fourth comma-delimited field, because its first byte addresses `fields[3]` beyond the three-row table.

The existing aggregate 50-character message limit did not prevent either case. A single field can exceed 21 bytes before the aggregate limit, and several short fields can reach a fourth row.

## Completed TDD slice

The firmware source contract was extended first. Against the pre-fix source, `just test` failed red with:

```text
Firmware contract check failed: Linkbus receive parser lacks linkbus_rx_field_can_terminate, linkbus_rx_can_start_next_field, linkbus_rx_field_can_append bounds guard(s)
```

The production correction adds small inline boundary predicates used directly at the three array-access decisions in `linkbus_Rx()`. Direct host tests compile those same predicates and prove:

- fields 1 through 3 accept 20 payload bytes plus their terminating NUL;
- a 21st payload byte is rejected;
- the third field can terminate safely at maximum length;
- a comma after the third field is rejected before a fourth field can start;
- message-ID position zero, indices beyond field 3, and zero-capacity buffers cannot be used for field storage.

When a frame exceeds either declared dimension, the parser leaves its buffer marked empty, stops receiving that frame, and waits for the existing `$` or `!` start-marker logic to begin a new frame. It does not publish a partial message ID. Valid field lengths, one-to-three-field messages, empty fields, command/reply/query terminators, escaped delimiters, uppercasing, and the existing aggregate message limit are unchanged.

`just check` passes with AddressSanitizer and UndefinedBehaviorSanitizer enabled for the host tests.

## Exact Mac AVR build

Two consecutive final-source builds used AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103. Both completed with zero warnings and byte-identical hashes for all six artifacts:

```text
   text    data     bss     dec     hex
  40256    1106    1137   42499    a603
```

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `21aa2ff7628b7a2fd60bbcdf65af7f1a81f08e58bbb83be3f51ce87135263306` |
| `FlexFox80.hex` | `e02ea26c5182f0b272e5c8888fb73b4cb93fdad3fbee6670d27ea4a9ae8fd3e5` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `05ba9bcd83e5e9ff3bd3f6681486a68265082a12cb4140d619e9c37b7a34c0b2` |
| `FlexFox80.lss` | `2203fb65df77962b0c26f7ad5ebe2e17e0b3d5e70fda53d1fed1b21a86890e68` |
| `FlexFox80.srec` | `ce801cc55bea3ac366d3643ca90a2cbeb4722f84b494d5dd716bbc4ad548ebb0` |

Compared with the preceding bounded text-send build, text grows by 52 bytes; data and BSS are unchanged. The EEPROM initializer remains byte-identical.

## Connected-target programming

The dummy-loaded AVR128DA48 test unit was programmed with the exact `3bc10a5` Release HEX through the proven chip-erase, flash-write/verify, and complete EEPROM-restore/verify workflow.

Fresh pre-write captures matched the preserved unit baseline:

- EEPROM, 512 bytes: `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401`;
- fuses, 16 bytes: `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126`.

Avrdude verified all 41,088 input flash bytes twice. Independent post-operation reads then proved:

| Memory | Expected SHA-256 | Post-read SHA-256 | Result |
| --- | --- | --- | --- |
| Programmed flash bytes | `fd81309f8beb34abc49122c0c8777760f167f818366c9f950b6fb48d8a77cb1d` | `fd81309f8beb34abc49122c0c8777760f167f818366c9f950b6fb48d8a77cb1d` | byte-identical |
| Restored EEPROM | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

The first constrained Linkbus test attempt timed out at HTTP after the programming session, before its WebSocket opened or any malformed frame was sent. This matches the known Moto/DroidTether association loss after extended Atmel-ICE activity and is not counted as a firmware result.

## Remaining verification

- Obtain exact Windows same-source builds and the full Windows host-contract run.
- Send controlled oversized-field and fourth-field frames that do not map to a configuration or RF command, then prove a following read-only temperature query succeeds.
