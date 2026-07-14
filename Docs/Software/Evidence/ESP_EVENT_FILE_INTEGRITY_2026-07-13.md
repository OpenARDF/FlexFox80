# ESP Clone Event-File Integrity Correction

**Date:** 2026-07-13

**Scope:** ESP8266 event-file framing and the existing clone-transfer length checksum

**Status:** Red/green host test, source contract, compatibility review, and two exact candidate builds pass; target programming, normal installed regression, valid clone, and corrupt-transfer rejection gates remain

## Confirmed defect

The master already sends a decimal `CHECK,<sum>` after `EVENT_END`. The sum is the length of every trimmed line from `EVENT_START` through `EVENT_END`, inclusive. The clone target writes the transferred filename preamble, event lines, checksum line, and then closes `/Temp` when it receives `EOF`.

`Event::validEventFile()` did not enforce that transfer checksum:

- its local `int checksum` was never initialized;
- its validation loop included `!endFound` in the loop condition;
- the branch intended to parse `CHECK` was inside that loop but reachable only when `endFound` was already true;
- normal `writeEventFile()` output ends at `EVENT_END` and contains no stored checksum.

The checksum branch was therefore unreachable. A clone could pass the structural start/end check even when the transferred `CHECK` was missing or did not match.

## Compatibility boundary

The three checked-in event fixtures and mature ESP-written files contain `EVENT_START` through `EVENT_END` without `CHECK`. Rejecting every checksum-less file would break deployed configuration and the established web editor.

The correction therefore preserves two explicit policies:

1. ordinary stored and newly written event files remain valid without a checksum;
2. clone-received `/Temp` must contain one well-formed, matching `CHECK` after `EVENT_END` before it can replace an event file.

This slice retains the deployed length-sum algorithm. It detects dropped, truncated, added, and changed-length lines but is not collision-resistant and cannot detect every equal-length substitution. Replacing the wire checksum would require a versioned protocol change across master and target and is outside this conservative correction.

## TDD gate

The host regression and runner integration were added before the shared production helper. The recorded red failure was:

```text
fatal error: 'event_file_integrity.h' file not found
```

The green regression compiles the same dependency-free state tracker used by `Event::validEventFile()` and proves:

- a legacy checksum-less file remains valid when checksum is optional;
- that same file is rejected for a clone transfer;
- the filename preamble is excluded and the lengths from `EVENT_START` through `EVENT_END` are included;
- a matching checksum after `EVENT_END` is accepted;
- changed-length payload, malformed checksum, missing end marker, checksum before the end marker, duplicate checksum, and invalid arguments are rejected.

The source contract requires production to use the shared state tracker, requires the legacy one-argument validator to remain checksum-optional, and requires `clientUpdateEventFilesLoop()` to call the explicit checksum-required overload for `/Temp`.

## Implementation

`event_file_integrity.h` owns initialized checksum state, framing state, strict decimal parsing, duplicate detection, overflow checks, and the optional/required final decision. `Event::validEventFile()` now reads through the checksum line instead of stopping at `EVENT_END`, retains the existing 200-line bound, and extracts `FILENAME` only from the pre-event preamble.

The normal one- and two-argument validators delegate with `requireChecksum=false`. The clone receive path alone calls:

```text
Event::validEventFile(path, &updatedFileName, true)
```

No event fields, serialization order, WebSocket messages, checksum arithmetic, RF behavior, AVR messages, or LittleFS image inputs changed.

## Exact-build evidence

The source was compiled twice with the qualified profile: Arduino CLI 1.2.0, ESP8266 core 2.7.4, WebSockets 2.3.6, Adafruit HUZZAH at 80 MHz, 4 MB flash / 1 MB filesystem, lwIP v2 Lower Memory, and debug disabled. Both builds completed with zero warnings and produced the same sketch binary.

| Resource | R8 installed baseline | Integrity candidate | Delta |
| --- | ---: | ---: | ---: |
| Total sketch use | 499,264 bytes | 499,676 bytes | +412 bytes |
| IROM code | 462,420 bytes | 462,804 bytes | +384 bytes |
| IRAM | 27,612 bytes | 27,612 bytes | 0 |
| DATA | 1,344 bytes | 1,344 bytes | 0 |
| RODATA | 7,888 bytes | 7,916 bytes | +28 bytes |
| BSS | 30,472 bytes | 30,472 bytes | 0 |

The candidate retains 42,188 bytes of dynamic-memory headroom and 5,156 bytes of IRAM headroom.

Candidate sketch binary:

- 503,824 bytes;
- SHA-256 `8d079501ce39810818fa64664a70a7f21729eb234fa93a6da04cb6679dfb3911`;
- byte-identical across two exact builds.

## Remaining target gates

Before R4 is complete:

1. preserve the receiving HUZZAH's complete flash and program only the exact candidate sketch;
2. require normal standalone reset/SSID and installed HTTP/WebSocket/AVR telemetry;
3. perform a normal clone into the corrected receiver and require all files, checksum validation, role restoration, RTC readback, and cleanup to succeed;
4. inject a mismatched or missing transfer checksum through a controlled test master and require the receiver to retain the prior event file and report clone failure;
5. restore both units' reliable event, role, master/target, clock, and filesystem state.

Do not infer the corrupt-transfer target gate from the host test or a successful normal clone.
