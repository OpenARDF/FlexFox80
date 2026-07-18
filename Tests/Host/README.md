# FlexFox80 Host Tests

These tests compile selected production sources with the native host C++ compiler. They provide fast characterization and regression feedback without requiring an AVR device pack, ESP8266 board package, or attached hardware.

The default compiler command is `c++`. Set `CXX` to another compatible compiler command when needed. AddressSanitizer and UndefinedBehaviorSanitizer are enabled by default; set `HOST_TEST_SANITIZERS=0` only when the available compiler does not support them.

Run all established host tests with:

```sh
just test
```

The runner writes executables only under the ignored `Software/AVR128DA48/tmp/host-tests/` directory.

`just test` also runs dependency-free source-contract checks for firmware declarations whose target-compiler interpretation is safety-relevant. These checks supplement compilation; they do not replace it.

The source contracts also require the release-pair definitions and their reporting paths to remain aligned. The current candidate identity is ESP `2.13` plus AVR `0.204`, reported together as `SW_VERSIONS,2.13,0.204`.

The embedded firmware-update page regression also parses both browser scripts and requires the AVR path to retain the unique MAC-derived SSID-suffix authorization prompt and display measured LittleFS headroom.

The fleet soak generator regression creates a deterministic twelve-event bundle for ten units and verifies the alternating Sprint/Classic schedule, quiet intervals, zero-based assignments, role definitions, frequencies, file limits, hashes, and clone-transfer checksums. It also covers overwrite and short-setup-time rejection.

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

The RTC edge-tracker regression compiles the same counter decisions used by the AVR's one-second interrupt and covers:

- a PORT interrupt before or after the high-priority sampler observes an edge;
- recovery of multiple observed edges and eight-bit counter wrap;
- repeated one-second PORT wakes while the sampler is deliberately stopped for standby sleep; and
- counter realignment followed by the first physical edge after the sampler restarts.

The event-schedule state regression compiles the same pure decision used by the foreground schedule helpers and covers:

- invalid clock and event-window inputs;
- `start-1`, `start`, and `start+1`;
- `finish-1`, `finish`, and `finish+1`;
- agreement between future, active, and overall-scheduled predicates using one supplied `now` value.

The source contracts additionally require both the RTC pre-start wake and the
scheduled-start transition to retire `SLEEP_UNTIL_START_TIME`. This preserves
correct cycle timing when a connected Wi-Fi programming client intentionally
keeps the ESP powered across the event start and disconnects later.

The ESP role-assignment bounds regression compiles the same first-colon decision used by `Event::setTxAssignment()` and covers:

- complete extraction of role prefixes `0`, `1`, and `10`;
- rejection of null, missing-colon, and empty-role assignments;
- preservation of the existing permissive handling for empty slots and additional colons.

The ESP event-file integrity regression compiles the same framing/checksum state tracker used by `Event::validEventFile()` and covers:

- continued acceptance of mature checksum-less event files;
- required matching checksums for cloned temporary files;
- observation of the deployed `CHECK` line after `EVENT_END`;
- rejection of changed-length payloads, malformed or duplicate checksums, missing end framing, and checksums placed before the end marker.

The ESP event-data read guard regression compiles the same reset and count-bound helpers used by `Event::readEventFile()` and covers:

- removal of prior event identity, internal set label, assignment, role, and transmit-pattern data before each read;
- rejection of zero, negative, and oversized event/role counts before indexed access;
- safe handling of null event and role/tx storage while clearing a partially allocated object.

The ESP clone keep-alive schedule regression compiles the same bounded schedule used during target cloning and covers:

- an immediate heartbeat when cloning begins;
- a 20-second cadence that remains due until it can be queued;
- correct interval arithmetic across `millis()` wraparound; and
- immediate heartbeat cancellation when clone work ends.

The ESP Linkbus command-transaction regression compiles the same outcome decision used by event programming and covers:

- no success while a command remains queued or ACK-pending;
- immediate failure on a NAK, even if other work is queued;
- conservative failure when an ACK arrives only after a retry;
- a bounded 12-second local deadline; and
- correct deadline arithmetic across `millis()` wraparound.

The ESP firmware-update integrity regression compiles the exact guards used by the sketch-only WiFi updater and covers strict size bounds, strict CRC32 parsing, the standard CRC32 test vector, and rejection of filesystem or compressed-image filenames.

The Fleet Soak regressions compile the exact reserved-name and assignment boundaries, exercise deterministic generation of the twelve alternating Sprint/Classic events, and verify that the control page remains an optional asset using only soak-scoped activation and cleanup operations.

Zero-capacity construction and allocation-failure behavior are intentionally not asserted yet. They are candidate red-green defect slices and must begin with recorded pre-fix evidence.

## Limits

Host tests do not prove AVR ABI, interrupt, timing, memory-footprint, peripheral, sleep/wake, or RF behavior. Any firmware change still requires the build and target gates in `Docs/Software/HARDENING_AND_BUG_PLAN.md`.
