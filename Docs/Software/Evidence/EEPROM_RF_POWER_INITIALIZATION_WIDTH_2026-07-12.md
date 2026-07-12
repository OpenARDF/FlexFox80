# EEPROM RF Power Initialization Width Characterization — 2026-07-12

## Status

Characterized as the next bounded R6 candidate. No firmware correction is included in this evidence slice because the preceding I2C failure-count correction is still awaiting exact Windows AVR verification.

## Observed contract

- `EE_prom::rf_power` is declared as `uint16_t`.
- `RF_Power` reserves two bytes before `Guard4_15`.
- The normal read path uses `eeprom_read_word`.
- The normal update path compares a `uint16_t` and uses `avr_eeprom_write_word`.
- `g_80m_power_level_mW` is `uint16_t`.

## Mismatch

First-time EEPROM initialization calls:

```text
avr_eeprom_write_dword(RF_Power, g_80m_power_level_mW);
```

That writer spans four bytes: the two-byte RF power field and the first two bytes of the following reserved guard. With ordinary power values, the extra bytes are likely zero, matching the current static guard initializer, but the write still exceeds the field contract and couples initialization to adjacent storage.

The line predates the AVR128DA48 directory reorganization; `git blame` traces it to commit `d004f26b` from 2021-11-30.

## Proposed TDD slice

1. Extend the EEPROM source-contract regression to require word-width initialization for the declared 16-bit RF power field.
2. Capture the failing result against the current implementation.
3. Change only the initialization call from the dword writer to the word writer.
4. Rerun the host and repository gates.
5. Obtain two deterministic Windows AVR Release builds and record warnings, size, and artifact deltas.

This candidate should remain separate from broader EEPROM layout, enum-width, and guard-validation work.
