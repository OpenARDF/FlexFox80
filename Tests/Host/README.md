# FlexFox80 Host Tests

These tests compile selected production sources with the native host C++ compiler. They provide fast characterization and regression feedback without requiring an AVR device pack, ESP8266 board package, or attached hardware.

The default compiler command is `c++`. Set `CXX` to another compatible compiler command when needed. AddressSanitizer and UndefinedBehaviorSanitizer are enabled by default; set `HOST_TEST_SANITIZERS=0` only when the available compiler does not support them.

Run all established host tests with:

```sh
just test
```

The runner writes executables only under the ignored `Software/AVR128DA48/tmp/host-tests/` directory.

`just test` also runs dependency-free source-contract checks for firmware declarations whose target-compiler interpretation is safety-relevant. These checks supplement compilation; they do not replace it.

The source contracts also require the release-pair definitions and their reporting paths to remain aligned. The current release identity is ESP `2.1` plus AVR `0.201`, reported together as `SW_VERSIONS,2.1,0.201`.

## Current characterization boundary

The first slice compiles the production AVR `CircularStringBuff.cpp` unchanged and covers behavior already relied upon by its FIFO users:

- initial empty state and capacity;
- uppercase storage and FIFO retrieval;
- full-buffer state and oldest-entry overwrite;
- index wraparound;
- reset and busy-state behavior;
- documented LIFO `pop()` behavior, including wrap to the last occupied index.

The bounded text-copy regression compiles the same inline helper used by Linkbus and Serialbus and covers:

- percent characters copied as literal data;
- exact maximum payload length with a terminating NUL;
- rejection of oversized input without emitting a partial protocol frame;
- rejection of null pointers and zero-capacity destinations.

The Linkbus receive-boundary regression compiles the same guards used by the USART receive parser and covers:

- the one-to-three-character message-ID boundary;
- the final payload byte in the first and third fields;
- reserved space for each field's terminating NUL;
- rejection of writes to the message-ID position or a fourth field;
- safe termination of a maximum-length third field;
- rejection of a fourth comma-delimited field and zero-capacity buffers.

The event-schedule state regression compiles the same pure decision used by the foreground schedule helpers and covers:

- invalid clock and event-window inputs;
- `start-1`, `start`, and `start+1`;
- `finish-1`, `finish`, and `finish+1`;
- agreement between future, active, and overall-scheduled predicates using one supplied `now` value.

The ESP role-assignment bounds regression compiles the same first-colon decision used by `Event::setTxAssignment()` and covers:

- complete extraction of role prefixes `0`, `1`, and `10`;
- rejection of null, missing-colon, and empty-role assignments;
- preservation of the existing permissive handling for empty slots and additional colons.

The ESP event-file integrity regression compiles the same framing/checksum state tracker used by `Event::validEventFile()` and covers:

- continued acceptance of mature checksum-less event files;
- required matching checksums for cloned temporary files;
- observation of the deployed `CHECK` line after `EVENT_END`;
- rejection of changed-length payloads, malformed or duplicate checksums, missing end framing, and checksums placed before the end marker.

Zero-capacity construction and allocation-failure behavior are intentionally not asserted yet. They are candidate red-green defect slices and must begin with recorded pre-fix evidence.

## Limits

Host tests do not prove AVR ABI, interrupt, timing, memory-footprint, peripheral, sleep/wake, or RF behavior. Any firmware change still requires the build and target gates in `Docs/Software/HARDENING_AND_BUG_PLAN.md`.
