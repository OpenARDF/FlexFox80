# FlexFox80 v2.0.2 GitHub Release Verification — 2026-07-25

**Release:** <https://github.com/OpenARDF/FlexFox80/releases/tag/v2.0.2>

**Published:** 2026-07-25T14:49:35Z

**Tag object:** `1759a65ba3b59d52b165fbe747788ab8a85e9e43`

**Peeled tag and manifest commit:** `1120ee63804bf6b69dcea89c1c17772236081556`

## Release record

GitHub reports `FlexFox80 v2.0.2` as a stable release with `isDraft=false` and `isPrerelease=false`. The remote annotated tag peels to the exact verified source and manifest commit.

## Published assets

| Asset | Bytes | SHA-256 |
| --- | ---: | --- |
| `FlexFox80-v2.0.2-AVR-0.210-ESP-2.28-Release-Files.zip` | 549,378 | `ef86a4ef7ae9390f2da236d7d59a310b5b8a800034bfff00351b11380947538f` |
| `FlexFox80-AVR-0.210.hex` | 122,905 | `ae482773e00cfd87039586b3e1890e595e9e501b9179ecb4df1ac698655f3092` |
| `FlexFox80-AVR-First-Install-0.210.hex` | 135,648 | `71991b95664b1fdf6b7e30d0f45547ecdd6642c154c7e1943e4234c240cdb95c` |
| `FlexFox80-ESP-2.28.bin` | 569,168 | `b851c87004302c908783f6746c91de91a82a6a0dadb8f53546de8ea27feef226` |
| `FlexFox80-LittleFS-2.28.bin` | 1,024,000 | `287936d2f922844c454797adec249965a7d1b0ca80ba48639da21e11deadd8f5` |
| `FlexFox80-Release-Info-v2.0.2.json` | 2,034 | `1f8cfb575f29ca00839a7286d113baf8c72dc19518763f4c0b69a7e5abc5abb5` |
| `FlexFox80-Checksums-v2.0.2.txt` | 572 | `a14e1c89c6f1e5607bf2d04ecab650a0e2ae7effcf54778aa8096d8613cedb1c` |

GitHub asset metadata reports the same sizes and SHA-256 digests.

## Fresh-download verification

All seven assets were downloaded with `gh release download` into a new directory. Each was byte-identical to the locally validated release set. A clean extraction of the downloaded ZIP passed:

- ZIP integrity and exact seven-file inventory;
- every internal SHA-256 check;
- manifest format, product, stable channel, exact hardware revision, embedded versions, build profiles, 274-byte EEPROM schema, and `SW_VERSIONS,2.28,0.210`;
- AVR Intel HEX parsing with no EEPROM, fuse, signature, or lock-byte address records;
- exact extraction of the 5,112 mapped BL0.3 bytes, relocated AVR `0.210` application, and 512-byte protocol-2 trailer;
- first-install fuse metadata `CODESIZE=0x00` and `BOOTSIZE=0x20`;
- ESP sketch checksum validation with esptool.py v2.8;
- LittleFS parsing with the qualified 8,192-byte block, 256-byte page, and 1,024,000-byte layout, including `radio.html` 0.86; and
- equality between the manifest commit, remote annotated-tag target, `main`, and the frozen source commit.

Two complete local release-build passes were byte-identical and warning-free. The first-install HEX does not write fuses or EEPROM.

## Disposition

The stable GitHub release, seven assets, annotated tag, and fresh-download verification pass. This evidence is committed after the tag; `v2.0.2` remains fixed at `1120ee6`.
