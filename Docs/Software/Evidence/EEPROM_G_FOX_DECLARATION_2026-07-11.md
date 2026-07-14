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

## Green and target evidence

- `node scripts/check-firmware-contracts.mjs` passes.
- `just test` passes with sanitizers enabled.
- `just check` passes on macOS.
- Two clean exact Windows AVR Release rebuilds using AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103 completed with zero warnings; the three array-bounds warnings disappeared and no new warning appeared.
- AVR size remained `text=40170`, `data=1106`, and `bss=1137`.
- Both post-fix builds were deterministic, and their ELF, HEX, EEP, LSS, MAP, and SREC hashes all matched the pre-fix baseline. This confirms that correcting the declaration removed compiler diagnostics without changing generated firmware artifacts.

The exact target-build results are recorded in [WINDOWS_G_FOX_VERIFICATION_2026-07-12.md](WINDOWS_G_FOX_VERIFICATION_2026-07-12.md). This defect slice is target-verified.
