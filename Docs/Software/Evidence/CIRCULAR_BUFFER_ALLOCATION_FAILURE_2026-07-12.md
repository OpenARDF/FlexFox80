# Circular Buffer Allocation-Failure Hardening — 2026-07-12

## Status

The red-green host regression, repository checks, and deterministic exact Mac AVR Release builds are complete. Connected-target programming remains open. Routine Windows duplication is not required under the established parity policy.

## Confirmed defect

`CircularStringBuff` stored the requested capacity even when `malloc()` returned null. The object therefore appeared usable and `put()` wrote through the null pointer.

This is a low-memory fault path rather than expected operation: the deployed FlexFox constructs one fixed 100-byte buffer during static initialization. Hardening the failure is nevertheless important because a memory shortage should disable optional text buffering rather than cause immediate undefined behavior.

## TDD evidence

The host sanitizer harness now permits allocator failure and requests `SIZE_MAX`, which AddressSanitizer deterministically refuses. The pre-fix test failed red:

```text
WARNING: AddressSanitizer failed to allocate 0xffffffffffffffff bytes
expectation failed: buffer.capacity() == 0
expectation failed: buffer.full()
runtime error: store to null pointer of type 'char'
ERROR: AddressSanitizer: SEGV on unknown address 0x000000000000
```

The correction changes one constructor assignment: capacity is set to the requested size only when allocation succeeds, and otherwise becomes zero. The prior zero-capacity hardening then supplies the safe disabled behavior:

- `capacity()` and `size()` return zero;
- `empty()` and `full()` both report true, preventing producer attempts;
- `put()` is a no-op;
- `get()` and `pop()` return the existing empty sentinel.

Successful nonzero allocation, buffer layout, FIFO/LIFO order, overwrite, reset, busy state, and public interfaces are unchanged. The complete `just check` suite passes green, including the sanitizer-forced failure.

`scripts/run-host-tests.sh` sets only AddressSanitizer's `allocator_may_return_null=1` option for the circular-buffer executable. Shellcheck and shfmt validation pass.

## Exact Mac AVR build

Two consecutive builds used AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103. Both completed with zero warnings and byte-identical hashes for all six artifacts:

```text
   text    data     bss     dec     hex
  40380    1106    1144   42630    a686
```

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `cf1a0449e78677ffa110e07932d7b3af80944dada3c5eb791b6511d3e5ea67ef` |
| `FlexFox80.hex` | `09cce546875cdaadcf0edec823b903a8278f7f70572390974001cc4dd968455b` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `5b5307e5141f160724ba9501c236e4ea292b4615466744c654ac6cea061047da` |
| `FlexFox80.lss` | `e6a945197aafa39a80c01d91d6cc1d8dd570d3f4cadc9ea4e964f41f5379894c` |
| `FlexFox80.srec` | `f4eec261bafacc0a95f197401959a1b6cc50fd69870b143c339c09b7e9bde552` |

Compared with the preceding zero-capacity build, text grows by eight bytes. Data and BSS are unchanged, and the EEPROM initializer remains byte-identical.

## Remaining verification

- Program the exact Release HEX on the dummy-loaded test unit using the established complete EEPROM preservation and verification workflow.
- Confirm normal startup and the successful 100-byte allocation path through the existing read-only WiFi probe.
