# Circular Buffer Zero-Capacity Hardening — 2026-07-12

## Status

The red-green host regression, repository checks, and deterministic exact Mac AVR Release builds are complete. The change is not yet programmed on the connected test unit. Routine Windows duplication is not required under the established parity policy.

## Confirmed defect

`CircularStringBuff(0)` constructed a buffer with zero capacity, but `full()` reported false and `put()` continued into the ordinary storage and index-advance path. The class then attempted both a zero-byte-buffer write and modulo by zero.

The deployed FlexFox constructs its only instance with the fixed, nonzero `TEXT_BUFF_SIZE` of 100 bytes. This defect therefore does not change or explain ordinary operation, but it made the class unsafe for an invalid configuration and complicated separate allocation-failure hardening.

## TDD evidence

A focused host regression was added first with the following zero-capacity contract:

- capacity and size remain zero;
- the buffer is empty and reports full, preventing producer loops from attempting writes;
- `put()` is a no-op;
- `get()` and `pop()` return the existing empty sentinel.

The pre-fix run failed red and UBSan reported division by zero in both `put()` and the subsequently corrupted `get()` path:

```text
expectation failed: buffer.full()
runtime error: division by zero
expectation failed: buffer.size() == 0
runtime error: division by zero
expectation failed: buffer.get() == '\0'
FAIL zero_capacity_buffer_rejects_input
```

The correction is deliberately limited to zero capacity:

- `full()` returns true when capacity is zero;
- `put()` returns before storage or index arithmetic when capacity is zero.

No constructor, allocation, nonzero-capacity, FIFO, LIFO, overwrite, reset, busy-state, or public-interface behavior changes. Allocation failure for a requested nonzero capacity remains a separate planned slice.

The focused regression and complete `just check` suite pass green under the host sanitizer build.

## Exact Mac AVR build

Two consecutive builds used AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103. Both completed with zero warnings and byte-identical hashes for all six artifacts:

```text
   text    data     bss     dec     hex
  40372    1106    1144   42622    a67e
```

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `70d770baef2e35013f1cc6f8a7191597e0c5224af5978cf576c5dab6d69778ef` |
| `FlexFox80.hex` | `20148bdb4c698e2563b2c9e8b0ae63d49155a9f7cd027517389b5843928a4383` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `6df20f58933e690cf212316d766ddafd73af2fbbdb4c34320d9a5ce09f100975` |
| `FlexFox80.lss` | `d84ff1ed36d5f327155c3ee24fe160893c16e23dc4f320b53f4f0009b8fb3cda` |
| `FlexFox80.srec` | `8768acc2c293c7ef9df8295ff4ae70f34d4c687ec8c2d6f88c1e7feff40f0fbd` |

Compared with the preceding collision-free-ID build, text grows by 22 bytes. Data and BSS are unchanged, and the EEPROM initializer remains byte-identical.

## Remaining verification

- Program the exact Release HEX on the dummy-loaded test unit using the established complete EEPROM preservation and verification workflow.
- Confirm the ordinary nonzero-capacity path remains healthy through the existing read-only WiFi probe.
