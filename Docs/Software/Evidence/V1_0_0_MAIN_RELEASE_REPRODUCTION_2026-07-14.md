# FlexFox80 v1.0.0 Main-Branch Release Reproduction

**Date:** 2026-07-14

**Branch:** `main`

**Integrated commit verified:** `2dd843859da4e10561704534779a62964374f803`

**Hardware target:** FlexFox Ver 2.1 (Mar 2022) only

**Result:** Pass — the integrated release tree passes the complete repository check and independently reproduces the selected AVR `0.201`, ESP `2.1`, and LittleFS artifacts with the qualified pinned build profiles.

## Repository gate

The worktree was clean and aligned with `origin/main`. `just check` passed, covering repository policy, documentation links, standardized firmware headers, host regression tests, firmware contracts, release-checklist validation, and the fixed 274-byte EEPROM layout.

## AVR deterministic reproduction

Two Release builds used AVR-GCC 7.3.0 and AVR-Dx_DFP 1.9.103. Both builds completed with zero warnings. Their ELF, HEX, EEPROM object, map, listing, and S-record files were byte-identical.

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `32f1d263ab4ed1d7080a80f0a49d771b2a6061cf579ed67382f988b3f5c6f1eb` |
| `FlexFox80.hex` | `897076608a3473d48c0226dfd8800f5ac058e553286a2f97450bfd03a77f1ba4` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `6f24be84f2a0cc868ab1835d8a0664de7295939274a0309b7ba3ea79788d0df8` |
| `FlexFox80.lss` | `2dbc7a33d41fc12c9eb894730ae18d2124aa00122bb5d11b47faa0486f378108` |
| `FlexFox80.srec` | `dc5600fbecefa5482a9a22effed011033e0bca2e4966b6e0a1a89c787f7cd753` |

Resource use was 41,010 bytes text, 1,112 bytes initialized data, and 1,572 bytes BSS. The deployed EEPROM schema remained exactly 274 bytes.

## ESP and LittleFS deterministic reproduction

Two builds used ESP8266 core 2.7.4, WebSockets 2.3.6, 80 MHz CPU, and the qualified 4M1M profile. Both completed with zero warnings. The published sketch, ELF, map, LittleFS image, and build-evidence files were byte-identical. Timestamp-bearing transient library archives under the temporary work directory are not release artifacts and were excluded from the equality claim.

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| `ARDF_Transmitter.ino.bin` | 503,824 | `0749b0e493254d526cb7a5e8afd938bd65b04c22bfffaa6c5150614f6315bad8` |
| `ARDF_Transmitter.ino.elf` | 5,408,656 | `ebb8e71bcd03319a815c145940744354da910c35ea56314a8d4584289539cdac` |
| `ARDF_Transmitter.ino.map` | 3,953,481 | `e5917fff3f67a13a84206ac39fde507225c00e1dfd1f227a0c86457354be4c0f` |
| `ARDF_Transmitter.littlefs.bin` | 1,024,000 | `0b45a6ad86ea7774fc6f964a2325417f6d8978f7d2213305d3786b9ee77f0c41` |

The sketch used 499,676 bytes of program storage and 39,732 bytes of dynamic memory; reported IRAM use was 27,612 bytes.

## Release interpretation

This evidence closes the clean integrated-branch reproduction gate. The final annotated tag points to a documentation-only descendant that records this gate and the owner's final release authorization. Both firmware targets are rebuilt once more from that exact tag commit before the tag is created; their selected hashes must remain identical to the values above. The GitHub package manifest records the exact tag commit and is validated independently before and after publication.
