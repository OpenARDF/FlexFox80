# EEPROM RF Power Initialization Width Characterization — 2026-07-12

## Status

Corrected by commit `4dbd90f` (`Correct RF power EEPROM initialization width`). Mac red-green, repository, exact AVR Release build, connected-target fault injection, and accumulated exact Windows verification are complete.

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

## Completed TDD slice

The source contract was extended first to require both the ordinary update path and first-time initialization to use the word writer for `RF_Power`. Against the pre-fix source, `just test` failed red for exactly one reason:

```text
Firmware contract check failed: RF_Power initialization does not write its uint16_t width
```

Commit `4dbd90f` changes only the initialization call from `avr_eeprom_write_dword` to `avr_eeprom_write_word`. The same test then passed green, along with the existing host characterization, I2C-width contract, fixed enum-width contract, WiFi probe safety contract, and all 65 fields of the 274-byte EEPROM layout.

No field, offset, default, normal read, normal update, RF-control path, or guard definition changed.

## Exact Mac AVR build

Two consecutive final-source builds used AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103. Both reported `reference-version-match`, zero warnings, and byte-identical hashes for all six artifacts:

```text
   text    data     bss     dec     hex
  40174    1106    1137   42417    a5b1
```

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `b5e98b4b5ccea6b5fa3012a5fef4b60730b00feb0e2a743e74f79be4202e930c` |
| `FlexFox80.hex` | `2b6df2dbf0cf577b5fb3dbc3b0f5e0e9222f6c402b312da6f1116f5f1d990e79` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `05ec49db11e480d232d25991abf2725cc10929a5c94769f7c18449b637b57e8f` |
| `FlexFox80.lss` | `29d68eb531b005fc35e9622df0ed7375d73b3e371622e7063a9e98ab4ba251f2` |
| `FlexFox80.srec` | `4fe43bb1b7d24771f82ea9c38fc45e0e881c8a3a1a0a432e77dffd9744dfd41c` |

Compared with the preceding fixed-enum-width Mac build, text decreased by four bytes; data and BSS are unchanged. The EEPROM initializer remains byte-identical, and the linker map still reports `.eeprom = 0x112` (274 bytes).

## Remaining verification

- Obtain exact Windows same-source build and host-contract evidence.

## Connected-target fault injection

The dummy-loaded AVR128DA48 test unit was programmed with the exact `4dbd90f` Release HEX through the proven erase/program/EEPROM-restore workflow. Independent readback before injection established:

- programmed flash binary SHA-256: `fc68d76d524cf5a8f0fa95c101ce7a1e1d3281708151e3c74727048c9e4c207e`;
- original 512-byte EEPROM SHA-256: `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401`;
- original 16-byte fuse SHA-256: `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126`.

An ignored copied EEPROM image was then prepared with only these deliberate changes:

- initialization flag at offsets 0–1: `00 00`;
- all four `Guard4_15` bytes at offsets 159–162: `A5 5A C3 3C`.

The existing RF value at offsets 157–158 remained 3000 mW (`B8 0B`). The injection image SHA-256 was `45b79b8b39bc11c8f2cb6c539a204ffcf88d2fc0db3c6aa67dcfc8b00345c036`.

After writing and verifying that complete image, the target was released to boot once. Complete EEPROM readback proved:

```text
initialization flag = 0x0108
RF power            = 500 mW
Guard4_15           = A5 5A C3 3C
```

The initialization routine therefore wrote the correct two-byte default and did not change any byte of the adjacent guard. This directly exercises the corrected target instruction path; the old dword writer would have overwritten the first two sentinel bytes.

The complete original EEPROM was then restored and verified. Final independent reads were byte-identical to the intended `4dbd90f` flash, the original 512-byte EEPROM, and the original 16 fuse bytes using the hashes above.

Two exact Windows builds of the accumulated `3bc10a5` snapshot completed deterministically with zero warnings and passed the RF-power source contract, matching layout, resource, HEX, and EEPROM evidence. See [Windows accumulated hardening verification](WINDOWS_ACCUMULATED_HARDENING_VERIFICATION_2026-07-12.md).
