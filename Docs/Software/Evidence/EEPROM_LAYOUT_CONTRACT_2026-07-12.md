# EEPROM Layout Contract Evidence — 2026-07-12

## Purpose

R6 requires EEPROM width corrections without moving deployed offsets. A host-side characterization now compares the structure declaration with every address in `EE_var_t` using the AVR-GCC 7.3.0 target widths that define the existing schema.

## Covered invariant

`scripts/check-eeprom-layout.mjs`:

- reads the 65 fields in `EE_prom` in declaration order;
- applies the AVR target widths for scalar, enum, time, floating-point, and array fields;
- evaluates every `EE_var_t` offset expression;
- requires each enum address to equal its corresponding structure-member offset; and
- requires the aggregate layout to remain 274 bytes (`0x112`), matching the checked-in reference map's `.eeprom` size for `EepromManager::ee_vars`.

The checked-in AVR ELF's DWARF independently records the same 274-byte structure and target member offsets, including `rf_power` at `0x9D`, `guard4_15` at `0x9F`, `guard4_32` at `0x10C`, and `i2c_failure_count` at `0x110`. Run the checker with `--show-offsets` when a human-readable field/size/offset table is needed.

The check runs through `just test` and therefore through `just check`.

## Result

```text
PASS all 65 EEPROM field offsets match the 274-byte AVR layout
```

This is a passing characterization test, not a defect correction. It creates a migration tripwire before later edits to field types, order, array lengths, enum widths, or target ABI assumptions.

## Explicit limits

- It does not validate the contents of guard words; current firmware does not check them.
- It does not replace an exact AVR build or a captured EEPROM image from deployed hardware.
- The checked-in `.eep` file is build output for the static initializer, not evidence of a unit's persisted runtime state. A real image has now been captured separately, but it is intentionally ignored because it may contain unit-specific configuration.
- Any intentional schema migration must update this contract only alongside a separately reviewed compatibility and recovery plan.
