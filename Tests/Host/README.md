# FlexFox80 Host Tests

These tests compile selected production sources with the native host C++ compiler. They provide fast characterization and regression feedback without requiring an AVR device pack, ESP8266 board package, or attached hardware.

The default compiler command is `c++`. Set `CXX` to another compatible compiler command when needed. AddressSanitizer and UndefinedBehaviorSanitizer are enabled by default; set `HOST_TEST_SANITIZERS=0` only when the available compiler does not support them.

Run all established host tests with:

```sh
just test
```

The runner writes executables only under the ignored `Software/AVR128DA48/tmp/host-tests/` directory.

`just test` also runs dependency-free source-contract checks for firmware declarations whose target-compiler interpretation is safety-relevant. These checks supplement compilation; they do not replace it.

## Current characterization boundary

The first slice compiles the production AVR `CircularStringBuff.cpp` unchanged and covers behavior already relied upon by its FIFO users:

- initial empty state and capacity;
- uppercase storage and FIFO retrieval;
- full-buffer state and oldest-entry overwrite;
- index wraparound;
- reset and busy-state behavior;
- documented LIFO `pop()` behavior, including wrap to the last occupied index.

Zero-capacity construction and allocation-failure behavior are intentionally not asserted yet. They are candidate red-green defect slices and must begin with recorded pre-fix evidence.

## Limits

Host tests do not prove AVR ABI, interrupt, timing, memory-footprint, peripheral, sleep/wake, or RF behavior. Any firmware change still requires the build and target gates in `Docs/Software/HARDENING_AND_BUG_PLAN.md`.
