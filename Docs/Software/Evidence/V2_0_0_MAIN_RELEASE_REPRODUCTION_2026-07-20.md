# FlexFox80 v2.0.0 Clean-Main Reproduction — 2026-07-20

**Integrated commit verified:** `558c841d8f79d8f6c628b6a55da301108b170e66`

**Branch:** `main`

**Selected pair:** ESP `2.27`, AVR `0.210`

## Integration

`main` was clean and at prior public tip `06dd9312836c2c16a5ffd4940fb9bc65fd704c8d`. It was fast-forwarded to the qualified `Development_AVR128DA48` candidate without a merge commit, rewritten history, or main-only divergence.

## Repository gate

`just check` passed from the clean integrated branch. The run covered repository/document policy, Markdown links, source headers, host sanitizer/regression tests, firmware source and protocol contracts, release-checklist validation, and the exact 274-byte EEPROM layout.

## Reproducible firmware

Two fresh AVR builds used AVR-GCC 7.3.0 and AVR-Dx_DFP 1.9.103. Both were warning-free and byte-identical across all seven recorded artifacts. The selected application HEX remained:

`552736db7a5104d9c508247b00bd63188aa29934ef6ca25afa3760ff6d483359`

Two fresh ESP builds used ESP8266 core 2.7.4, WebSockets 2.3.6, Adafruit Feather HUZZAH at 80 MHz, and the 4M1M layout. Both were warning-free and byte-identical across sketch, ELF, map, and LittleFS artifacts. The selected images remained:

- sketch: `bc4d757bfed672e2537e86e402505e1093f67d002ced94d858ed31b2b3f815fa`;
- LittleFS: `4199c8b87b3c77272939e2c400aa77e04d5f52b1125d8c689b35f87b86b5b476`.

These hashes also exactly match the development-branch candidate qualification.

## Disposition

The integrated branch passes the release gate. The following release-record commit changes documentation only. The selected firmware is rebuilt once more from that exact tag candidate before the manifest is generated, after which the annotated tag target and manifest commit must agree.
