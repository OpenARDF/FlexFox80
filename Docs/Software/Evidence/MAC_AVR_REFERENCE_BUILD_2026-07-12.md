# Mac AVR Reference Build Evidence — 2026-07-12

## Source and scope

- Branch: `Development_AVR128DA48`
- Source commit: `0429c2a` (`Make generated AVR include path portable`)
- Configuration: repository `scripts/build-avr-release.mjs` Release wrapper
- Host: Apple silicon Mac using Rosetta for the archived x86_64 toolchain

This build includes the I2C failure-count width correction from `6c0f4fa`. It does not include a later RF power width correction.

> **Follow-up:** This historical baseline predates the fixed-width persisted-enum correction in `57d70a7`. Its Release configuration could produce a 268-byte EEPROM object and must not be flashed over retained deployed EEPROM. Use the corrected 274-byte evidence in [EEPROM_ENUM_WIDTH_ABI_2026-07-12.md](EEPROM_ENUM_WIDTH_ABI_2026-07-12.md) for programming decisions.

## Exact build inputs

### AVR-GCC

- Official archive: [Microchip AVR 8-bit Toolchain 3.7.0 for macOS](https://ww1.microchip.com/downloads/aemDocuments/documents/DEV/ProductDocuments/SoftwareTools/avr8-gnu-toolchain-osx-3.7.0.518-darwin.any.x86_64.tar.gz)
- Filename: `avr8-gnu-toolchain-osx-3.7.0.518-darwin.any.x86_64.tar.gz`
- Size: `52,111,217` bytes
- SHA-256: `378b210cc82dc06599b5a45dede0ed188a9e87f2de045f12cf3547554edd6ec8`
- Compiler identity: `AVR_8_bit_GNU_Toolchain_3.7.0_518`, GCC `7.3.0`

### Device family pack

- Historical official archive: [Atmel AVR-Dx_DFP 1.9.103](http://packs.download.atmel.com/Atmel.AVR-Dx_DFP.1.9.103.atpack)
- Filename: `Atmel.AVR-Dx_DFP.1.9.103.atpack`
- Size: `6,237,158` bytes
- SHA-256: `c7f95e43a0640068720da826c0e9f8f12c76109eb8bd6bd2b762868704d7bbf4`
- PDSC identity: vendor `Atmel`, name `AVR-Dx_DFP`, release `1.9.103`, dated `2021-07-20`

Both archives and their extracted directories remain under the ignored `Software/AVR128DA48/tmp/` tree and were not committed.

## Red portability evidence

The first exact wrapper attempt stopped at the first translation unit:

```text
include/driver_init.h:28:10: fatal error: utils\compiler.h: No such file or directory
```

Repository inspection found exactly one backslash include path. Commit `0429c2a` changes only that generated-derived include spelling to `utils/compiler.h`, documents the regeneration risk, and adds a source-contract check that rejects future Windows-only include separators.

## Two-run result

Two consecutive clean wrapper runs reported:

- status: `reference-version-match`
- warnings: `0`
- byte-identical `build-evidence.json`
- matching SHA-256 for all six requested artifacts

Both runs produced:

```text
   text    data     bss     dec     hex
  40170    1106    1137   42413    a5ad
```

| Artifact | Run 1 and Run 2 SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `88201fd09db6f1c490167880bb68813d97532cc3410e5b6b289c9a9907e26c82` |
| `FlexFox80.hex` | `de31cf9feffec15ddf401c0e0b1131da233a283c04dfe41540bb9fb07dfa1aaf` |
| `FlexFox80.eep` | `905808d6fb3c3d7e490386b6a5c613afb9c49c488c6047d9c94eb84bb8efde09` |
| `FlexFox80.map` | `7f67f419c3ee56ae148824edc7f21748695c235d6b6dacb623abd93d9c643670` |
| `FlexFox80.lss` | `f958419e44da500d44c0a7e3bae725d101fc77ed9b71496b5ef547d5b02bebeb` |
| `FlexFox80.srec` | `3565f9076c84d762ca20ea6c6aa6e8f1eef399add9b7416e909e72580f946d5b` |

The memory totals exactly match the established Windows reference totals. The `.eep` hash also matches the prior Windows verification. Executable and listing hashes are not compared as same-source proof until Windows returns the pending `6c0f4fa` verification; the Mac build additionally contains the compile-only include portability change.

## Repository verification

`just check` passes, including the sanitizer-enabled host suite, firmware source contracts, and the 65-field AVR EEPROM layout contract.

## Conclusion

An exact-version AVR Release build is now reproducible and deterministic on the primary Mac development environment. This closes the missing Mac DFP/toolchain boundary for AVR builds. It does not yet establish the ESP8266 build or qualify an AVR artifact through functional transmitter hardware tests.
