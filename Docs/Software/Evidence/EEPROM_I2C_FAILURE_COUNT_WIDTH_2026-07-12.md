# EEPROM I2C Failure Count Width Evidence — 2026-07-12

## Contract

The AVR EEPROM schema declares `i2c_failure_count` as `uint16_t`. Its global value is also `uint16_t`, and the normal load and change-detection paths use `eeprom_read_word`. Writes must therefore preserve the same 16-bit width without moving the deployed EEPROM offset.

## Red evidence

A source-contract regression checked both write paths against the declared field width. Before the correction, `just test` left all existing circular-buffer characterization green but failed both I2C failure-count checks:

```text
PASS g_fox declaration covers every Event_t value
Firmware contract check failed: I2C_failure_count update does not write its full uint16_t value
Firmware contract check failed: I2C_failure_count initialization does not write its uint16_t width
```

The ordinary update path compared a 16-bit value but wrote only its low byte. The first-time initialization path wrote a dword even though the schema reserves a 16-bit field.

## Minimal correction

- The update path now calls `avr_eeprom_write_word(I2C_failure_count, *(uint16_t*)val)`.
- The initialization path now calls `avr_eeprom_write_word(I2C_failure_count, g_i2c_failure_count)`.

No field type, enum offset, structure order, guard position, read path, default value, or save trigger changed. This correction makes both writes match the already deployed 16-bit schema.

## Green evidence

- The new source-contract regression passes.
- All existing AVR circular-buffer characterization remains green with AddressSanitizer and UndefinedBehaviorSanitizer enabled.
- `just check` passes on macOS.
- Two exact Windows AVR Release builds of the accumulated `3bc10a5` snapshot completed deterministically with zero warnings, the expected 274-byte EEPROM layout, matching Mac HEX/EEPROM outputs and resource totals, and a passing full host-contract suite. See [Windows accumulated hardening verification](WINDOWS_ACCUMULATED_HARDENING_VERIFICATION_2026-07-12.md).

This focused correction does not close the broader EEPROM-layout and guard-validation work tracked as R6.
