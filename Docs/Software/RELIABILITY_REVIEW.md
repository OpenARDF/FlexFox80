# FlexFox80 Software Reliability Review

## Purpose

FlexFox80 is a mature and reportedly reliable product. This review identifies why the design has worked well, where the implementation is vulnerable, and which areas need evidence before modification.

The review covers the ESP8266 and AVR128DA48 source on the `AVR128DA48` branch at commit `33e64e0`. It is based on static source inspection and existing build artifacts. No hardware reproduction or fresh firmware build was performed as part of the review.

## Existing reliability strengths

### Real-time isolation

The AVR owns event timing and RF control, so web traffic, filesystem operations, and ESP8266 resets are separated from the time-critical transmit loop. The ESP8266 can be powered off after configuration.

### Fail-safe RF controls

- The RF clock is disabled during transmitter initialization.
- Antenna removal forces final drain voltage off.
- RF power and frequency are restricted to configured 80 m limits.
- Sleep and event suspension explicitly unkey and depower the transmitter.
- Si5351 keying operations use bounded retries and attempt subsystem restart.

### Peripheral timeouts and retries

RTC, Si5351, and I2C access use retries or timeout counters in many paths. Startup records missing major peripherals as hardware errors and uses LED indications.

### Persistence controls

Most EEPROM writes occur only when a value changes. Numeric EEPROM values are generally clamped when loaded. The ESP event parser applies maximum role, transmitter, and line counts.

### Operational recovery behavior

The ESP8266 frequently calls `yield()` in long loops, allowing its built-in watchdog and network stack to operate. The AVR can power-cycle or reset the ESP8266 independently.

## Priority findings

### R1: Linkbus receive fields are not bounds checked

**Classification:** Confirmed code issue<br>
**Area:** AVR communications ISR<br>
**Potential impact:** SRAM corruption, reset, incorrect settings, or unpredictable RF behavior after malformed serial input

`linkbus_Rx()` limits the total character count but does not limit `field_index` before indexing `fields[field_index - 1]`, and does not limit `field_len` before writing a character. The receive structure allows three fields of 21 bytes each, while a single message field can exceed that size before the total-message limit is reached. Extra delimiters can also move beyond the three-field array.

The parser runs in a USART ISR, so memory corruption can affect foreground and other interrupt state.

**Recommended first step:** Build a host-side copy or thin adapter around the parser and feed overlong fields, extra delimiters, escape characters, truncated frames, and back-to-back start markers. Record current behavior before adding rejection and resynchronization rules.

### R2: RTC synchronization can wait forever

**Classification:** Confirmed code issue and failure candidate<br>
**Area:** AVR timekeeping<br>
**Potential impact:** Complete foreground hang after a runtime RTC or square-wave failure

`syncSystemTimeToRTC()` clears `g_seconds_transition` and spins until the PORTA RTC interrupt sets it again. There is no timeout. This helper is called from event setup, configuration reporting, clock commands, and other normal paths.

The RTC is probed during startup, but a later RTC failure, broken square-wave connection, disabled interrupt, or corrupted port configuration can make this wait permanent.

**Recommended first step:** Reproduce with the square-wave input disconnected or suppressed after startup. Determine the desired degraded behavior: continue from system time, refuse a schedule change, report an RTC error, or reset.

### R3: AVR watchdog support is absent

**Classification:** Confirmed code issue<br>
**Area:** AVR recovery<br>
**Potential impact:** No automatic recovery from hangs; remote reset command does not reset

The watchdog API and reset modes are declared, but `wdt_init()` is empty. The Linkbus reset command calls `wdt_init(WD_FORCE_RESET)`, which currently has no effect. A hardware-watchdog enable flag is also unused.

This compounds R2 and any other infinite-wait defect.

**Recommended first step:** Before enabling a watchdog globally, inventory the longest legitimate blocking operations, sleep behavior, EEPROM writes, and startup retries. Add reset-cause reporting and a controlled watchdog-reset test first.

### R4: Event-file checksum validation is ineffective

**Classification:** Confirmed code issue<br>
**Area:** ESP8266 LittleFS event files<br>
**Potential impact:** Corrupt or partially written event files can be accepted as structurally valid

In `Event::validEventFile()`:

- `checksum` is used without initialization;
- the loop terminates when `EVENT_END` is found;
- the branch intended to read `CHECK` after `EVENT_END` cannot execute;
- `writeEventFile()` does not currently write a `CHECK` line.

The effective validation is therefore based mainly on file existence, start/end markers, and a line limit.

**Recommended first step:** Decide whether checksums remain part of the supported file format. If yes, document the exact covered bytes and compatibility behavior for existing files. If no, remove the misleading mechanism and use atomic-file replacement plus semantic validation.

### R5: Transmit interval input is insufficiently validated

**Classification:** Confirmed code issue and failure candidate<br>
**Area:** AVR event configuration<br>
**Potential impact:** Invalid cycle arithmetic, signed overflow, unexpected always-on/off behavior, or division by zero

Linkbus interval fields are parsed with `atol()` and directly narrowed to `int16_t`. Negative values and out-of-range positive values are not rejected. Activation rejects a zero on-air value but does not explicitly reject negative intervals. When starting an event already in progress, the code computes:

```text
cyclePeriod = on_air_seconds + off_air_seconds
secondsIntoCycle = elapsed % cyclePeriod
```

Certain invalid combinations can make the cycle period zero or negative.

The ESP event validation mostly verifies that interval strings are nonempty, not that they are numeric and in the AVR-supported range.

**Recommended first step:** Define protocol-level ranges and test them on both processors. Reject invalid values before mutating active state, and retain the prior valid configuration after a rejected event transfer.

### R6: EEPROM field widths are inconsistent

**Classification:** Confirmed code issue<br>
**Area:** AVR persistence<br>
**Potential impact:** Adjacent EEPROM guard corruption, incomplete counter persistence, and fragile migration behavior

Examples include:

- the two-byte RF power field is initialized with a four-byte write;
- the two-byte I2C failure counter is initialized with a four-byte write;
- updates to the I2C failure counter write only one byte;
- guard words are defined throughout the layout but never checked.

The initialization flag represents the entire layout but has no schema version or migration path. A layout change that retains the same flag can reinterpret existing EEPROM silently.

**Recommended first step:** Create a table-driven EEPROM layout test that checks every enum offset against the structure member offset and expected width. Capture a real deployed EEPROM image before changing layout behavior.

### R7: Text send helpers use input as a format string

**Classification:** Confirmed code issue<br>
**Area:** AVR Linkbus and Serialbus transmit buffers<br>
**Potential impact:** Buffer overrun or unintended stack reads when transmitted text contains `%`

`lb_send_text()` and `serialbus_send_text()` use `sprintf(*buff, text)`. This interprets `text` as a format string and does not constrain output to the destination buffer. Other Linkbus formatting helpers also use unbounded `sprintf()` into fixed arrays.

**Recommended first step:** Inventory maximum message lengths and all callers. Add tests for maximum-length callsigns, patterns, labels, `%` characters, and error messages before replacing formatting with length-aware copies.

### R8: Role index extraction is off by one

**Classification:** Confirmed internal-state issue; correction and all focused gates complete<br>
**Area:** ESP8266 per-transmitter role assignment<br>
**Potential impact:** Cached role power/frequency can be taken from the wrong role

`Event::setTxAssignment()` locates `:` and extracts the role with `substring(0, c - 1)`. For `1:2`, where the colon is at index 1, this yields an empty string rather than `1`. Conversion then selects role zero for cached power and frequency.

The full assignment string is still stored and the AVR event-send path parses it correctly. A completed call-site trace found no active consumer for cached power and no external caller of the cached-frequency accessor. Current evidence therefore identifies inconsistent internal state and a latent future hazard, but not an incorrect AVR configuration in released behavior.

**Implementation status:** Complete. The shared production bounds helper and red/green host test prove complete role prefixes for roles 0, 1, and a multi-digit parser boundary while preserving the legacy input boundary. A source contract and two exact zero-warning candidate builds pass with 32 additional flash bytes and no RAM/IRAM change. The exact sketch passed independent flash verification, standalone reset/SSID startup, guarded installed role-persistence checks for roles 0 and 1, restoration of the original event assignment, clone controls, and final HTTP/WebSocket/AVR telemetry; see [ESP role-assignment prefix correction](Evidence/ESP_ROLE_ASSIGNMENT_2026-07-13.md).

## Additional hardening candidates

### Shared ISR and foreground state

Many 16- and 32-bit values are shared between AVR interrupts and foreground code. `volatile` prevents compiler caching but does not make access atomic on an 8-bit processor. Event epochs, transmit countdowns, status fields, ADC results, and related state should be audited for torn reads and multi-step invariants.

Do not add broad interrupt disabling without measuring ISR latency requirements. Prefer small atomic snapshots and ownership rules.

### Oversized real-time ISR

TCB0 performs Morse generation, RF keying, cycle transitions, button and antenna debouncing, ADC scheduling, and some sleep preparation. This has worked in practice, but it increases worst-case interrupt latency and makes timing changes difficult to validate.

The first step should be timing instrumentation, not refactoring. Measure normal and worst-case ISR duration relative to its period.

### Dynamic allocation on the ESP8266

The ESP code makes extensive use of Arduino `String` and dynamically allocated Event/role/slot objects. Repeated event loads and browser edits may fragment heap over long sessions. The ESP8266 is reset and powered down in normal operation, which reduces exposure, but long configuration or clone-distribution sessions should be stress tested.

### Dynamic allocation on the AVR

The AVR circular string buffer and Goertzel support allocate heap memory. The checked-in map shows comfortable static SRAM usage, but allocation failure is not consistently checked. Prefer fixed storage for newly introduced AVR components.

### Persistence is not transactional

ESP event files are rewritten in place, and AVR settings are written as independent EEPROM values. Power loss during a save can produce a partially updated configuration. The existing initialization flag does not detect a partially completed later save.

### Hardware error coverage is incomplete

The hardware-error enum includes missing 12 V and FET bias, but startup currently records RTC, Si5351, and Wi-Fi failures. ADC readings are collected, yet supply-related hardware error bits are not visibly integrated into the same startup/error path.

### Intentional fall-through is widespread

Fox setup and parts of the ESP state machine intentionally fall through switch cases. Some instances are documented; others are less explicit. Compiler fall-through diagnostics should be enabled and every intentional case annotated before structural changes.

## Verification and maintenance gaps

- No automated unit or integration tests are checked in.
- No CI workflow builds either processor.
- The AVR build depends on Windows Atmel Studio paths in a generated Makefile.
- The ESP8266 library versions and board configuration are not pinned in the repository.
- Generated binaries, object files, map/listing files, and IDE state are tracked, obscuring source-only changes.
- The root README provides little programming, flashing, hardware-test, or release guidance.
- The AVR revision string, ESP displayed date, source commit, and built artifacts do not form a documented release identity.

## Resource snapshot from checked-in AVR artifact

The checked-in debug map reports approximately:

- `.text`: `0xABB8`, about 44 KiB;
- `.data`: `0x33C`, 828 bytes;
- `.bss`: `0x47C`, 1,148 bytes;
- static SRAM total: about 1,976 bytes, excluding stack and dynamic allocations.

This suggests useful headroom on an AVR128DA48, but a fresh reproducible release build should be used before relying on these figures.

## Overall assessment

The product-level architecture is sound and appropriately isolates RF timing from the network interface. The most important immediate risks are narrow, testable implementation defects rather than a need for wholesale redesign.

The first hardening work should focus on:

1. protocol memory safety;
2. recovery from loss of RTC timing;
3. watchdog design and reset diagnostics;
4. event and timing input validation;
5. EEPROM layout verification;
6. establishing repeatable builds and behavioral tests.
