# ESP Role-Assignment Prefix Correction

**Date:** 2026-07-13

**Scope:** `Event::setTxAssignment()` cached role name, power, and frequency selection

**Status:** Red/green host test, firmware contract, immediate-baseline comparison, and two exact candidate sketch builds pass; connected-target gate pending

## Confirmed defect

The event assignment format is documented as `"role:slot"`. `Event::setTxAssignment()` found the colon but extracted the role using `substring(0, c - 1)`. Arduino `String::substring()` excludes its end index, so:

- `"1:0"` produced an empty role prefix and `toInt()` selected role zero;
- `"10:2"` produced `"1"` and selected role one;
- only role zero happened to resolve as intended because its truncated empty prefix also converted to zero.

The full assignment and descriptive name were parsed through separate correct paths. The defect was limited to the cached role power and frequency populated when the assignment changed; later values sent to the AVR could therefore come from the wrong role even though the displayed assignment remained correct.

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

This is intentionally not a numeric-validation change. Inputs that the mature code previously accepted after a non-empty role and colon remain accepted. Numeric validation belongs to its separately planned hardening slice.

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

## Connected-target gate

After committing the source/build checkpoint:

1. preserve or identify rollback-compatible ESP sketch and filesystem state;
2. program the exact candidate sketch without erasing or replacing LittleFS;
3. require normal standalone reset/SSID startup before installation if a removable HUZZAH is used;
4. on a complete FlexFox, load an event fixture whose roles have distinct power and frequency values;
5. assign role zero, role one, and the highest valid configured role, and require the selected role's cached values and AVR configuration messages each time;
6. restore the unit's reliable event/role configuration and repeat normal HTTP, WebSocket, AVR telemetry, and clone-control probes.

Do not treat build success alone as target qualification. The cached-value consequence must be observed with distinct role values before R8 is complete.
