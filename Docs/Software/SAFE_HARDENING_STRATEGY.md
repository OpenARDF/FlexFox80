# Safe Hardening Strategy

## Goal

Improve FlexFox80's tolerance of malformed data, peripheral faults, power interruption, and long-running operation while preserving its proven event timing, RF behavior, user workflow, and deployed-file compatibility.

The default rule is: reproduce first, capture expected behavior, then make the smallest independently verifiable change.

## Risk model

Changes have different regression costs:

| Area | Regression risk | Required evidence |
| --- | --- | --- |
| Documentation and portable build metadata | Low | Clean diff and successful build |
| Pure parsing/validation helpers | Moderate | Boundary and compatibility tests |
| Linkbus framing and command semantics | High | Cross-processor tests and malformed-input tests |
| EEPROM layout or initialization | High | Real EEPROM image, migration test, interrupted-write test |
| Event-cycle arithmetic | Very high | Timeline simulation plus hardware observation |
| Morse timing or TCB0 ISR | Very high | Golden timing traces and on-target measurement |
| Antenna inhibit, final power, or Si5351 keying | Safety critical | Bench instrumentation and fault injection |
| Sleep/wake and RTC behavior | Safety critical | Repeated power-cycle and clock-fault tests |

## Phase 1: Establish reproducible baselines

Before changing behavior:

1. Record the exact Atmel Studio/device-pack/compiler versions that produced the deployed AVR image.
2. Add a portable AVR build path or a versioned container/toolchain definition while retaining the known Atmel Studio path.
3. Add an `arduino-cli` or PlatformIO definition with pinned ESP8266 board core and library versions.
4. Produce source-derived version metadata for both processors.
5. Record flash, SRAM, EEPROM, and warning counts for a clean build.
6. Preserve known-good `.hex` and ESP `.bin` artifacts with checksums outside ordinary source diffs or as an explicit release artifact.

A tooling-only baseline change should not alter generated machine code without explanation.

## Phase 2: Add host-side characterization tests

Start with logic that can be compiled or adapted without hardware:

- Linkbus frame parsing and resynchronization;
- event-file parsing and semantic validation;
- role/slot assignment parsing;
- epoch and event start/finish comparisons;
- cycle-position calculations;
- Morse pattern expansion and duration;
- power-table rounding;
- EEPROM offset and width checks.

Tests should include current valid examples from `Software/Huzzah/ARDF_Transmitter/data/` as golden fixtures.

For mature behavior, characterization tests can initially assert what the firmware currently does. A bug fix should then change only the deliberately affected expectation.

## Phase 3: Build a protocol compatibility harness

Treat Linkbus as a public interface between processors.

The harness should verify:

- every command emitted by the ESP is accepted by the AVR;
- every AVR reply is understood by the ESP;
- maximum valid callsign and pattern lengths;
- empty optional fields;
- escaped separators;
- partial, repeated, and concatenated frames;
- excess fields and overlong fields;
- invalid numeric text, signs, and overflow;
- ACK, NAK, timeout, and retry behavior;
- power interruption midway through an event transfer.

Malformed input must be rejected without modifying the last known-good active configuration.

## Phase 4: Timeline simulation

Extract or wrap the event-cycle calculation so it can be evaluated at arbitrary epochs. Cover:

- event not yet started;
- exact start second;
- every fox delay slot;
- exact on-to-off and off-to-on boundaries;
- callsign insertion near a boundary;
- always-on foxoring/beacon operation;
- event finish during on-air and off-air periods;
- restart or wake in the middle of a cycle;
- leap across multiple cycles;
- zero, maximum, and rejected invalid intervals;
- system-time correction forward and backward.

Expected traces should include RF power requested, RF clock keyed, pattern/ID selection, countdown value, status code, and sleep request.

## Phase 5: Hardware-in-the-loop fault matrix

Run repeatable bench tests on a current board. Record firmware identity and instrumentation for each run.

| Fault or transition | Expected safe behavior |
| --- | --- |
| Antenna removed while keyed | Final-stage power is inhibited promptly and remains off |
| Antenna reattached | Output resumes only according to explicit current event state |
| Si5351 NACK or bus failure | RF remains off; error is reported; foreground remains responsive |
| DS3231 absent at boot | Event does not run from invalid time; error is visible |
| DS3231 square wave lost after boot | No infinite hang; defined degraded behavior occurs |
| ESP reset during configuration | AVR retains last complete configuration |
| ESP powered down during event | Event timing and RF cycle continue correctly |
| AVR reset during event | Recovery behavior matches the documented policy |
| Power removed during AVR EEPROM save | Prior or new configuration is recoverable, never a mixed unsafe state |
| Power removed during LittleFS event save | Prior or new event file remains valid |
| Supply voltage out of range | RF response and error indication match policy |
| Temperature threshold crossed | Fan hysteresis works without rapid cycling |
| Long on/off event | No counter overflow, heap degradation, or timing drift |

Use a dummy load and appropriate RF measurement equipment for transmit tests.

## Phase 6: Apply small hardening changes

Recommended initial slices are:

1. Bounds-check Linkbus receive fields and explicitly discard malformed frames.
2. Replace format-string copies with bounded text copies.
3. Validate all numeric event fields before changing global state.
4. Add a timeout and explicit error result to RTC synchronization.
5. Add reset-cause reporting, then implement a watchdog policy.
6. Correct and test EEPROM access widths without changing deployed offsets.
7. Define atomic snapshots for multi-byte ISR-shared values.
8. Make ESP event saves transactional using a temporary file and rename, subject to LittleFS support and compatibility tests.

Each slice should have its own focused commit and evidence. Avoid combining parser, timing, persistence, and RF-control changes.

## Change-review checklist

For every firmware change:

- Identify which processor and subsystem owns the behavior.
- Link the change to an observed symptom or reproducible test.
- State which existing behavior must remain unchanged.
- Check whether Linkbus or stored-file compatibility is affected.
- Check ISR execution time and interrupt-disabled duration.
- Check stack, static SRAM, heap, flash, and EEPROM impact.
- Confirm all RF-off paths still dominate RF-on requests.
- Exercise startup, active event, finish, suspend, and sleep/wake paths.
- Test failure before success where a peripheral is involved.
- Keep unrelated generated artifacts out of source-review commits.
- Record compiler, board, and firmware versions used for validation.

## Refactoring policy

Refactoring is useful only after behavior is characterized.

Good early refactoring targets are pure calculations and parsers that can be tested independently. Avoid initially reorganizing the TCB0 ISR, event global state, sleep/wake sequence, or RF control simply for style. Those areas should move only when a demonstrated fix requires it or when tests provide strong equivalence evidence.

When extracting code:

- preserve integer widths and signedness;
- preserve exact boundary comparisons;
- preserve operation ordering around RF power and keying;
- preserve protocol strings and stored-file formats;
- compare generated code or timing where interrupt behavior is involved.

## Suggested first investigation

The best first hardening slice is Linkbus input safety. It is locally bounded, directly testable without RF hardware, and protects the AVR from corrupted or unexpected data originating from the ESP, serial noise, or future software changes. It can establish the test infrastructure and narrow-change discipline needed for higher-risk timing and watchdog work.
