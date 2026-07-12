# Goertzel Fixed Sample Storage — 2026-07-12

## Status

The red-green source contract, repository checks, deterministic exact Mac AVR Release builds, and connected-target programming evidence are complete. The post-program WiFi probe remains open after the known Moto/DroidTether association loss. Routine Windows duplication is not required under the established parity policy.

## Confirmed defect

The global Goertzel detector clamps its sample count to 100 through `MAXN` (209), then allocated that fixed-range sample buffer from the heap during static initialization. `Flush()`, `DataPoint()`, and `Magnitude2()` dereferenced the pointer without any allocation-failure path.

The deployed instance requests 201 samples, or 402 AVR bytes. A transient or fragmented-heap failure at startup would therefore cause a null write in ADC sample handling instead of a defined degraded state.

## TDD evidence and correction

A source contract was added first to require fixed `MAXN` sample storage and reject `malloc()`/`free()` in `Goertzel.cpp`. It failed red against the heap-backed implementation:

```text
Firmware contract check failed: fixed-size Goertzel samples still depend on fallible heap allocation
```

The correction replaces the global pointer with `int testData[MAXN]`, removes the constructor allocation, and leaves the destructor empty. The existing global-state/single-instance design is retained. The following behavior is unchanged:

- `_N` remains clamped to 100–209 samples;
- the deployed instance still processes 201 samples;
- indexing, readiness, flush, target-frequency setup, coefficient calculation, high-value counting, and magnitude calculation are unchanged;
- no protocol, EEPROM, RF-setting, event, or ESP source changes.

The fixed buffer removes the startup failure and heap fragmentation possibility. `just check` passes green.

## Exact Mac AVR build and SRAM interpretation

Two consecutive builds used AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103. Both completed with zero warnings and byte-identical hashes for all six artifacts:

```text
   text    data     bss     dec     hex
  40338    1106    1560   43004    a7fc
```

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `61610e5c2229c9d65a0ff9de25e3318328d5ef9384fa2306b31a73dc0c2f9d1c` |
| `FlexFox80.hex` | `3215699d2f00ac14ba1347e595e90bfe0426bf05075865317060f629a28cd68e` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `b1077acb8ab201a0b03e9f328d7c5e5fc894ed0364022cda26369d7532811f9b` |
| `FlexFox80.lss` | `22a057f8fa36de5d1e54774f73b9a156d707c2b66bbb91f00c95aea0acb4da88` |
| `FlexFox80.srec` | `29b8ad0648e22a2433693d26185216e44ccddd1029360b5e60ac1410cfb18049` |

Compared with the preceding build, text decreases by 42 bytes and BSS grows by 416 bytes: the prior two-byte pointer is replaced by a 418-byte maximum array. This does not represent a 416-byte increase in live runtime usage. The deployed heap path already reserved 402 bytes for 201 samples, plus allocator metadata; fixed storage reserves 418 bytes and removes that heap allocation. The conservative live-SRAM increase is therefore at most 16 bytes before accounting for removed allocator metadata.

Static data plus BSS is 2,666 bytes of the AVR128DA48's 16 KiB SRAM. EEPROM output remains byte-identical.

## Connected-target programming

The dummy-loaded AVR128DA48 test unit was programmed with the exact `ffed775` Release HEX through the proven chip-erase, flash-write/verify, and complete EEPROM-restore/verify workflow.

Fresh pre-write captures matched the preserved unit baseline:

- EEPROM, 512 bytes: `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401`;
- fuses, 16 bytes: `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126`.

Avrdude verified all 41,170 input flash bytes twice. Independent post-operation reads then proved:

| Memory | Expected SHA-256 | Post-read SHA-256 | Result |
| --- | --- | --- | --- |
| Programmed flash bytes | `03f268fcbba8f8cd388b9418be04acc1ea7fcbbb550e32b363a64ec90618a2d7` | `03f268fcbba8f8cd388b9418be04acc1ea7fcbbb550e32b363a64ec90618a2d7` | byte-identical |
| Restored EEPROM | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

The first read-only WiFi probe after programming timed out at HTTP before opening a WebSocket or sending any message. This matches the known Moto/DroidTether association loss after Atmel-ICE activity and is not counted as a firmware result.

## Remaining verification

- Reconnect the Moto/DroidTether route.
- Confirm normal startup and WiFi-to-AVR communications with the read-only probe.
- A future controlled ADC/tone-input test remains part of the broader functional hardware gate; this slice does not claim frequency-detection qualification.
