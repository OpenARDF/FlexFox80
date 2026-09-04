# FlexFox80 v2.0.3 GitHub Release Verification — 2026-09-03

**Release:** <https://github.com/OpenARDF/FlexFox80/releases/tag/v2.0.3>

**Published:** 2026-09-04T01:16:59Z

**Tag object:** `2dbaac444e55c7eaba3aebb203fd66d5d3dc7f6f`

**Peeled tag and manifest commit:** `332d9a37fceeea1da192118616b803096090d704`

## Release record

GitHub reports `FlexFox80 v2.0.3` as its latest stable release with `isDraft=false` and `isPrerelease=false`. The remote annotated tag peels to the exact verified source and manifest commit.

## Published assets

| Asset | Bytes | SHA-256 |
| --- | ---: | --- |
| `FlexFox80-v2.0.3-AVR-0.210-ESP-2.35-Release-Files.zip` | 555,845 | `f28e95f19dcfff510bb486e2d073c0458f6e822ef6fc386567d38ef14f6a3b63` |
| `FlexFox80-AVR-0.210.hex` | 122,905 | `ae482773e00cfd87039586b3e1890e595e9e501b9179ecb4df1ac698655f3092` |
| `FlexFox80-AVR-First-Install-0.210.hex` | 135,648 | `71991b95664b1fdf6b7e30d0f45547ecdd6642c154c7e1943e4234c240cdb95c` |
| `FlexFox80-ESP-2.35.bin` | 581,120 | `3e3817c2b60e2f8b94d4cb40ff2fcb6360cf5db80404e295af1114d634749acb` |
| `FlexFox80-LittleFS-2.35.bin` | 1,024,000 | `3da1bfb553c5feb4d7a0ed47ef4dc420fd0ab83fb0c5944cc4856490f24ec892` |
| `FlexFox80-Release-Info-v2.0.3.json` | 2,131 | `cea8968803c1c4d1c68f6d09ba3e0020c8b1d1fb076db057d336e4e9ee30a19b` |
| `FlexFox80-Checksums-v2.0.3.txt` | 572 | `55927a64f97cc1cfbbad0d0c6dc04cbc33a9d5a79f146b905a9f5ca45bfa6866` |

GitHub asset metadata reports the same sizes and SHA-256 digests.

## Fresh-download verification

All seven assets were downloaded with `gh release download` into a new directory. Each was byte-identical to the locally validated release set. A clean extraction of the downloaded ZIP passed:

- ZIP integrity and exact seven-file inventory;
- every internal SHA-256 check;
- manifest format, product, stable channel, exact hardware revision, embedded versions, pinned build profiles, 274-byte EEPROM schema, and `SW_VERSIONS,2.35,0.210`;
- AVR Intel HEX parsing and equality to the two warning-free, byte-identical pinned builds;
- exact equality of the first-install image to the validated BL0.3 and relocated-application package;
- first-install fuse metadata `CODESIZE=0x00` and `BOOTSIZE=0x20`;
- ESP sketch checksum validation with esptool.py v2.8;
- LittleFS parsing with the qualified 8,192-byte block, 256-byte page, and 1,024,000-byte layout, including `radio.html` 0.86; and
- equality between the manifest commit, remote annotated-tag target, `main`, and the frozen source commit.

Two complete local release-build passes were byte-identical and warning-free. The first-install HEX does not write fuses or EEPROM.

## Disposition

The stable GitHub release, seven assets, annotated tag, and fresh-download verification pass. This evidence is committed after the tag; `v2.0.3` remains fixed at `332d9a3`.
