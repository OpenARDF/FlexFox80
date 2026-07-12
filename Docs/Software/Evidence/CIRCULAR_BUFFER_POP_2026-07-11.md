# Circular Buffer `pop()` Evidence — 2026-07-11

## Contract

`CircularStringBuff::pop()` is documented as returning and removing the last inserted item. This is LIFO behavior, distinct from the buffer's FIFO `get()` operation.

## Red evidence

The host test inserted `A`, `B`, and `C`, then expected `pop()` to return `C`, `B`, `A`, and finally the empty sentinel. Against the pre-fix production implementation, `just test` failed all three value expectations while every existing FIFO, wraparound, overwrite, reset, and busy-state characterization continued to pass:

```text
expectation failed: buffer.pop() == 'C'
expectation failed: buffer.pop() == 'B'
expectation failed: buffer.pop() == 'A'
3 expectation(s) failed
```

The implementation read `buf_[head_]` before moving `head_` backward. `head_` denotes the next write position, not the last occupied position.

## Minimal correction

`pop()` now moves `head_` backward with the existing wraparound logic and then reads `buf_[head_]`. No allocation, FIFO `get()`, overwrite, reset, busy-state, or public-interface logic changed.

## Green evidence

- The LIFO regression passes with AddressSanitizer and UndefinedBehaviorSanitizer enabled.
- All prior circular-buffer characterization remains green.
- `just check` passes on macOS.
- Two clean exact Windows AVR Release builds completed with zero warnings, unchanged resource usage, and deterministic matching artifacts between runs.
- The Windows `just check` run passed, including the LIFO regression and all prior circular-buffer characterization.
- Exact target-build results and hashes are recorded in [WINDOWS_CIRCULAR_BUFFER_POP_VERIFICATION_2026-07-12.md](WINDOWS_CIRCULAR_BUFFER_POP_VERIFICATION_2026-07-12.md).
