# ESP Role-Assignment Prefix Correction

**Date:** 2026-07-13

**Scope:** `Event::setTxAssignment()` cached role name, power, and frequency selection

**Status:** Complete — red/green host test, firmware contract, exact comparison/builds, verified flash, standalone startup, installed role/restoration gate, clone-control regression, and final communication probe pass

## Confirmed defect

The event assignment format is documented as `"role:slot"`. `Event::setTxAssignment()` found the colon but extracted the role using `substring(0, c - 1)`. Arduino `String::substring()` excludes its end index, so:

- `"1:0"` produced an empty role prefix and `toInt()` selected role zero;
- a longer prefix such as `"10:2"` produced `"1"` instead of retaining the complete prefix;
- only role zero happened to resolve as intended because its truncated empty prefix also converted to zero.

The full assignment and descriptive name were parsed through separate correct paths. The defect was limited to the cached role power and frequency populated when the assignment changed.

A post-fix call-site trace found no active production consumer for `tx_role_pwr`, and the only accessor for `tx_role_freq`, `getTxFrequency()`, has no call site outside `Event`. AVR event programming instead calls `getTxRoleIndex()` on the intact stored assignment, then looks up power and frequency directly with `getPowerlevelForRole(role)` and `getFrequencyForRole(role)`. The correction therefore fixes inconsistent internal state and removes a latent future hazard; current evidence does **not** show that this defect caused an incorrect AVR configuration in released behavior.

## TDD gate

The host test and runner integration were added before the production bounds helper existed. The red result was the expected compile failure:

```text
fatal error: 'role_assignment_bounds.h' file not found
```

The green test compiles the exact dependency-free first-colon decision used by production and proves:

- assignments `"0:0"`, `"1:0"`, and `"10:2"` extract complete prefixes `"0"`, `"1"`, and `"10"`;
- null, missing-colon, and empty-role inputs remain rejected;
- empty slots and additional colons retain their legacy acceptance, so this slice does not silently broaden validation behavior.

The source contract requires `setTxAssignment()` to obtain bounds from the production helper, use the complete `[roleBegin, roleEnd)` substring, and never restore the `c - 1` expression.

## Implementation

`role_assignment_bounds.h` identifies the first colon and returns the role and slot boundaries. `Event::setTxAssignment()` uses those bounds to extract the full role prefix before looking up cached power and frequency. Assignment storage, descriptive-name parsing, change tracking, event-file persistence, slot parsing, and AVR message generation are unchanged.

This is intentionally not a numeric-validation change. Inputs that the mature code previously accepted after a non-empty role and colon remain accepted. The present event model supports at most six role types, so the multi-digit case is a parser-boundary regression rather than a currently valid event assignment. Numeric and range validation belongs to its separately planned hardening slice.

## Exact-build evidence

The committed pre-fix source and candidate were compiled against the same qualified development profile:

- Arduino CLI 1.2.0;
- ESP8266 core 2.7.4;
- WebSockets 2.3.6;
- Adafruit Feather HUZZAH ESP8266 at 80 MHz;
- 4 MB flash / 1 MB filesystem;
- lwIP v2 Lower Memory, debug disabled.

| Resource | Immediate pre-fix baseline | Candidate | Delta |
| --- | ---: | ---: | ---: |
| Total sketch use | 499,232 bytes | 499,264 bytes | +32 bytes |
| IROM code | 462,388 bytes | 462,420 bytes | +32 bytes |
| IRAM | 27,612 bytes | 27,612 bytes | 0 |
| DATA | 1,344 bytes | 1,344 bytes | 0 |
| RODATA | 7,888 bytes | 7,888 bytes | 0 |
| BSS | 30,472 bytes | 30,472 bytes | 0 |

The candidate retains 42,216 bytes of dynamic-memory headroom and 5,156 bytes of IRAM headroom. Both builds completed with zero warnings.

Immediate baseline sketch binary:

- 503,392 bytes;
- SHA-256 `3b6b5ad8e20d9662c9ee833f9c8072b955b27f61d895cc9dde95a3d13f4a796e`.

Candidate sketch binary:

- 503,424 bytes;
- SHA-256 `1910afd3993d691bf353091cb499f6e17ec2ee7f456974a21e58b0450e6d3221`;
- byte-identical across two candidate builds.

The checked-in LittleFS inputs did not change. The build wrapper recreates that image, whose hash is not used as source-correction evidence because filesystem image metadata can vary across invocations.

## Standalone programming gate

The exact candidate sketch was programmed at address `0x0` on removable HUZZAH MAC `44:17:93:0f:09:3e` without replacing its LittleFS region. Before programming, the complete 4 MiB flash was preserved and independently verified:

- backup: `huzzah-4417930f093e-20260713-full.bin`;
- size: 4,194,304 bytes;
- SHA-256: `ec05ea3f65b0f28be571c3c58e17b0272125b6e4d238b9114401d13ba74a81bf`.

An independent post-write verification matched all 503,424 candidate bytes at address `0x0`. After a normal reset, the user observed the established LED behavior and normal FlexFox SSID advertisement.

## Connected-target gate

The HUZZAH was installed on the dummy-loaded FlexFox with its preserved LittleFS contents. The initial read-only probe passed HTTP, WebSocket, temperature, battery, identity, master state, software versions `2.0,0.200`, and advancing AVR clock reports.

Before mutation, an explicit event read recorded the restoration point:

- event: `Classic 80m Set 1-1`;
- assignment: `1:0` (`Finish - MO`);
- role 0: 3,520,000 Hz and 3000 mW;
- role 1: 3,600,000 Hz and 3000 mW.

The guarded `just wifi-role-assignment-test` requires explicit opt-in plus the expected event and restoration role. It sends only heartbeat, identity, explicit event selection, role assignment, and role-value queries. It does not send `EXECUTE`, `PASS`, `SYNC`, `CLEAR`, RF commands, or direct AVR frames. Cleanup writes and reloads the expected assignment even after a handled failure or signal.

The live gate passed:

1. `0:0` produced `SAVED_EVENT`, reloaded as `TX_ROLE,0:0`, and returned 3,520,000 Hz/3000 mW;
2. `1:0` produced `SAVED_EVENT`, reloaded as `TX_ROLE,1:0`, and returned 3,600,000 Hz/3000 mW;
3. final cleanup saved and independently reloaded `Classic 80m Set 1-1 / 1:0`.

No event was executed because the preserved event had already ended. This is appropriate for this correction: the focused host test and source contract prove the internal cache decision, while the call-site trace establishes that the cache does not currently drive AVR programming.

The first harness used a ten-second event-read deadline and timed out before any role command was sent. The ESP later completed the event-list operation. The final harness sends the explicit, idempotent event request once and allows 30 seconds, avoiding duplicate state transitions while preserving the no-mutation-before-baseline rule.

After restoration, `just wifi-clone-control-test` passed ordinary reports, quiet suppression, exactly one next-edge report, one-shot retention, and explicit resume. A final `just wifi-probe` again passed HTTP, WebSocket, `TEMP,30.0C`, `BAT,11.3V`, `SSID,Tx_Master`, MAC `22:C8:8E:CF:AB:84`, software `2.0,0.200`, `MASTER,1`, and advancing clock reports. R8 is complete.
