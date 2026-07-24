# FlexFox80 v2.0.1 GitHub Release Verification — 2026-07-24

**Release:** <https://github.com/OpenARDF/FlexFox80/releases/tag/v2.0.1>

**Published:** 2026-07-24T21:20:32Z

**Tag object:** `aec57b6511cf689a67d4409918fd49a32dc8c15c`

**Peeled tag and manifest commit:** `8a23b565821bbd27dca414b8b93809bd9ccc044c`

## Release record

GitHub reports `FlexFox80 v2.0.1` as the latest stable release with `isDraft=false` and `isPrerelease=false`. The remote annotated tag peels to the exact requested source and manifest commit.

## Published assets

| Asset | Bytes | SHA-256 |
| --- | ---: | --- |
| `FlexFox80-v2.0.1-AVR-0.210-ESP-2.27-Release-Files.zip` | 548,810 | `4e83e79c8fa7e8ea0f37cd2f61f06cce81697439fe45588c59a1c55668e60342` |
| `FlexFox80-AVR-0.210.hex` | 122,905 | `ae482773e00cfd87039586b3e1890e595e9e501b9179ecb4df1ac698655f3092` |
| `FlexFox80-AVR-First-Install-0.210.hex` | 135,648 | `71991b95664b1fdf6b7e30d0f45547ecdd6642c154c7e1943e4234c240cdb95c` |
| `FlexFox80-ESP-2.27.bin` | 569,152 | `abb161062fc3a8f04c436bc851fddc8c0fa84f69f1c32248e5d0073912466148` |
| `FlexFox80-LittleFS-2.27.bin` | 1,024,000 | `380d9de6e85a76fb5905dc7555961a2cbb75bdd52a111707a932ff531fd48e75` |
| `FlexFox80-Release-Info-v2.0.1.json` | 2,034 | `d9cf9ca7c6d264beeee5aa99aeb2d404fe3e5afc8312b133b8ccefbcacb4b911` |
| `FlexFox80-Checksums-v2.0.1.txt` | 572 | `f0c9205e961469ae7b19ed3112c77adb480449a7d6a63e2d187c02c09a03bf6c` |

GitHub asset metadata reports the same sizes and SHA-256 digests.

## Fresh-download verification

All seven assets were downloaded with `gh release download` into a new directory. Each was byte-identical to the locally validated release set. A clean extraction of the downloaded ZIP passed:

- ZIP integrity and exact seven-file inventory;
- every internal SHA-256 check;
- manifest format, product, stable channel, exact hardware revision, embedded versions, build profiles, 274-byte EEPROM schema, and `SW_VERSIONS,2.27,0.210`;
- AVR Intel HEX parsing;
- exact extraction of 5,112 mapped BL0.3 bytes, 43,688 mapped relocated-application bytes, and the 512-byte protocol-2 trailer;
- first-install fuse metadata `CODESIZE=0x00` and `BOOTSIZE=0x20`;
- ESP sketch checksum validation with esptool.py v2.8;
- LittleFS parsing with the qualified 8,192-byte block, 256-byte page, and 1,024,000-byte layout; and
- equality between the manifest commit, remote annotated-tag target, `main`, and the requested source commit.

Two complete local release-build passes were byte-identical and warning-free. The first-install HEX does not write fuses or EEPROM.

## Disposition

The latest GitHub release, seven assets, annotated tag, and fresh-download verification pass. This evidence is committed after the tag; `v2.0.1` remains fixed at `8a23b56`.
