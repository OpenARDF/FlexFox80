# FlexFox80 v1.0.0 GitHub Release Verification

**Date:** 2026-07-14

**Release:** [FlexFox80 v1.0.0](https://github.com/OpenARDF/FlexFox80/releases/tag/v1.0.0)

**Published UTC:** `2026-07-14T23:01:50Z`

**Result:** Pass — the stable, non-draft, non-prerelease GitHub release resolves through the annotated tag to the verified release commit, contains the six required assets, and passes independent download and package validation.

## Tag identity

- annotated tag: `v1.0.0`;
- tag object: `1ce3b05909014ae8df50c0d246d042550ef47fb3`;
- peeled release commit: `b4b98165af2ece0ed442ce23311be07df57c0231`;
- package-manifest commit: `b4b98165af2ece0ed442ce23311be07df57c0231`.

The remote tag and the downloaded manifest agree exactly. The tag was not moved during final documentation updates.

## Published assets

| Asset | Bytes | SHA-256 |
| --- | ---: | --- |
| `FlexFox80-v1.0.0-AVR-0.201-ESP-2.1-Release-Files.zip` | 451,859 | `17e126cefbc8ecc7281658057fc07278dceada6711e54fe80a4569b8424b655b` |
| `FlexFox80-AVR-0.201.hex` | 117,717 | `897076608a3473d48c0226dfd8800f5ac058e553286a2f97450bfd03a77f1ba4` |
| `FlexFox80-ESP-2.1.bin` | 503,824 | `0749b0e493254d526cb7a5e8afd938bd65b04c22bfffaa6c5150614f6315bad8` |
| `FlexFox80-LittleFS-2.1.bin` | 1,024,000 | `0b45a6ad86ea7774fc6f964a2325417f6d8978f7d2213305d3786b9ee77f0c41` |
| `FlexFox80-Release-Info-v1.0.0.json` | 1,505 | `fe017e9f796327230ae5ce38a5796b19bc3a80b3aec0d23ad4cd6926dae201b9` |
| `FlexFox80-Checksums-v1.0.0.txt` | 466 | `d58195cfed22da8aece6a6b2aad927c68b3d80ba68eb33d6dc05f918ef18874b` |

The GitHub asset metadata reported the same sizes and SHA-256 digests. The LittleFS asset is labeled `Recovery/factory LittleFS image; not a routine update`.

## Independent archive validation

All six assets were downloaded from GitHub into a new temporary directory and compared byte-for-byte with the locally validated release set. The ZIP passed `unzip -t`, was extracted into another new directory, and then passed:

- every checksum in `FlexFox80-Checksums-v1.0.0.txt`;
- exact manifest format, product, stable channel, hardware, version-pair, branch, and tag-commit checks;
- AVR Intel HEX parsing with the pinned AVR toolchain;
- ESP image parsing and a valid embedded checksum with esptool 2.8;
- LittleFS parsing with the qualified 8,192-byte block, 256-byte page, and 1,024,000-byte image geometry;
- inspection of all eight expected LittleFS files; and
- equality between the manifest commit and the peeled `v1.0.0` tag commit.

This closes the durable-archive and archived-release verification gates for v1.0.0.
