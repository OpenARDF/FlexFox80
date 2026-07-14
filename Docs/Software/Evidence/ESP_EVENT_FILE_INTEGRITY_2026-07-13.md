# ESP Clone Event-File Integrity Correction

**Date:** 2026-07-13

**Scope:** ESP8266 event-file framing and the existing clone-transfer length checksum

**Status:** Complete; host, deterministic-build, installed, normal-clone, corrupt-transfer rejection, prior-file retention, and restoration gates pass

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

- production source commit `c2e4e989f8ce5f1bcb4124bcea72ac88c7381446`;
- 503,824 bytes;
- SHA-256 `8d079501ce39810818fa64664a70a7f21729eb234fa93a6da04cb6679dfb3911`;
- byte-identical across two exact builds.

## Target qualification — 2026-07-14

The corrected production sketch was installed on the candidate HUZZAH with ESP chip MAC `44:17:93:0f:09:3e`. The WebSocket `MAC` command reports the connected client's MAC address, not the ESP chip MAC, so hardware identity was established through `esptool` rather than that command. Existing known-good flash archives were retained as rollback images; at the operator's explicit direction, this checkpoint did not create another full-flash archive.

Installed startup and communication passed through the supported path:

- the ESP advertised its expected SSID after reset;
- HTTP returned 200 and WebSocket communication opened normally;
- live AVR temperature, battery, software-version, master-state, and advancing `SYNC` reports were received;
- the exact production sketch is 503,824 bytes with SHA-256 `8d079501ce39810818fa64664a70a7f21729eb234fa93a6da04cb6679dfb3911`.

### Normal clone

The candidate was temporarily changed from `MASTER,1` to `MASTER,0` and used as the receiver. Its recorded pre-test state was `Classic 80m Set 1-1`, assignment `1:0`, callsign `NZ0I<<`, six role definitions, 3,520,000/3,600,000 Hz, and 3000/3000 mW.

The first normal attempt exhibited the already tracked `B-CLONE-02` stall. Resetting only the target did not restore ordinary AVR reports; the existing `$ESP,R;` cleanup command did. A clean retry then showed the established successful-clone LED sequence. Direct post-clone readback proved:

- `MASTER,0` and assignment `1:0` were preserved;
- all nine expected `Classic 80m Set` event files loaded;
- the active event and callsign remained valid;
- all six roles and both frequency/power values matched the baseline;
- HTTP, WebSocket, telemetry, and advancing clock reports resumed normally.

The production receive path cannot install a clone `/Temp` file unless the explicit checksum-required validator passes. The successful file set therefore exercised the corrected validator while retaining compatibility with the ordinary checksum-less files already stored in LittleFS. The clone event stream was not independently captured, so the evidence does not claim a frame-by-frame timing trace.

### Controlled mismatched-checksum clone

The temporary source/master HUZZAH was independently identified by `esptool` as `a4:e5:7c:2d:69:ed`. A test-only copy changed only the master event-file send expression from `String(checksum)` to `String(checksum + 1)`. Its sketch binary was 503,808 bytes with SHA-256 `ad68541b0f04c228d48a936f7b37d6de80b830b8acf6fa25359e769c3c98894e`. This was test equipment and was never committed as production source.

The source active event was given the temporary, persisted callsign `R4BAD`; the receiver baseline remained `NZ0I<<`. During the clone, the ESP LED sequence indicated that the transfer aborted. Direct receiver readback then proved:

- the active `Classic 80m Set 1-1` still contained `NZ0I<<`, not `R4BAD`;
- all nine prior event files remained readable;
- `MASTER,0`, assignment `1:0`, every role, both frequencies, and both powers were unchanged;
- HTTP, WebSocket, temperature, battery, and advancing `SYNC` reports remained healthy.

This distinguishes rejection with prior-file retention from a transfer that merely appeared to fail at the user interface.

### Restoration

The temporary source ESP was reflashed at address `0x0` with the exact production R4 binary; LittleFS was not rewritten. Its callsign was restored from `R4BAD` to `NZ0I<<` with `SAVED_EVENT` confirmation, and it was restored to `MASTER,0`, SSID `Tx_7C2D69ED`, and assignment `0:4`. A complete readback confirmed all nine events, six roles, both frequencies, both powers, and live telemetry.

The R4 candidate was restored to `MASTER,1`, SSID `Tx_Master`, and assignment `1:0`. Final readback confirmed all nine event files with `NZ0I<<`, six roles, 3,520,000/3,600,000 Hz, 3000/3000 mW, battery and temperature reports, and advancing `SYNC` epochs.

R4 is complete. The intermittent normal-clone stall remains deferred as `B-CLONE-02`; it did not invalidate the successful retry or the independently proven corrupt-transfer rejection and retention behavior.
