# EEPROM `g_fox` Declaration Evidence — 2026-07-11

## Trigger

Two deterministic Windows AVR Release builds at source commit `4d17bab` produced the same three AVR-GCC 7.3.0 array-bounds warnings in `src/eeprommanager.cpp`. The complete build environment, warning locations, resource usage, and baseline hashes are recorded in [WINDOWS_BUILD_ENVIRONMENT_2026-07-12.md](WINDOWS_BUILD_ENVIRONMENT_2026-07-12.md).

The warnings occurred when reading or initializing `g_fox[EVENT_BLIND_ARDF]`.

## Root cause

`main.cpp` defines five `g_fox` entries for none, classic, sprint, foxoring, and blind ARDF. `Event_t` assigns `EVENT_BLIND_ARDF` index 4 and `EVENT_NUMBER_OF_EVENTS` value 5.

The external declaration in `eeprommanager.cpp` incorrectly stated:

```cpp
extern volatile Fox_t g_fox[EVENT_NUMBER_OF_EVENTS-1];
```

That declaration told the compiler the external array had only four elements even though the linked definition contains five. All three warnings were consequences of the same declaration mismatch. No EEPROM address, stored field, enum value, or array definition required a change.

## Red evidence

A dependency-free source-contract test was added before the correction. Against the pre-fix declaration it failed as expected:

```text
Firmware contract check failed: g_fox extent is EVENT_NUMBER_OF_EVENTS-1; expected EVENT_NUMBER_OF_EVENTS
```

## Minimal correction

The external declaration now uses `EVENT_NUMBER_OF_EVENTS`. The production array contents, indexing, EEPROM schema, and runtime control flow are unchanged.

## Green evidence and remaining gate

- `node scripts/check-firmware-contracts.mjs` passes.
- `just test` passes with sanitizers enabled.
- `just check` passes on macOS.
- An exact Windows AVR Release rebuild is still required to confirm that the three warnings disappear and to record post-fix size and artifact hashes before this slice is considered target-verified.
