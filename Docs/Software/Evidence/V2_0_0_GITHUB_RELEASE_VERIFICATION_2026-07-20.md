# FlexFox80 v2.0.0 GitHub Release Verification — 2026-07-20

**Release:** <https://github.com/OpenARDF/FlexFox80/releases/tag/v2.0.0>

**Published:** 2026-07-20T18:41:37Z

**Tag object:** `9d2117c2db893ff2a7170f39c11b4f5c47065e77`

**Peeled tag and manifest commit:** `b3a05fee2dcf47d2ec6b99e92b4b0d5fc36d5556`

## Release record

GitHub reports `FlexFox80 v2.0.0` as a stable release: `isDraft=false` and `isPrerelease=false`. The annotated `v2.0.0` tag was pushed before publication and peels to the exact full commit recorded by the release manifest.

## Published assets

| Asset | Bytes | SHA-256 |
| --- | ---: | --- |
| `FlexFox80-v2.0.0-AVR-0.210-ESP-2.27-Release-Files.zip` | 495,969 | `c1c9f0a341046beebe4752108fa7d7756d0a2de99fd3044d7645d7a167e2f3cd` |
| `FlexFox80-AVR-0.210.hex` | 122,426 | `552736db7a5104d9c508247b00bd63188aa29934ef6ca25afa3760ff6d483359` |
| `FlexFox80-ESP-2.27.bin` | 568,912 | `bc4d757bfed672e2537e86e402505e1093f67d002ced94d858ed31b2b3f815fa` |
| `FlexFox80-LittleFS-2.27.bin` | 1,024,000 | `e13c5ab7cf00d5ee9db8daae639da8d4f35f9c056673770e0ccb13b18bece022` |
| `FlexFox80-Release-Info-v2.0.0.json` | 1,445 | `5c30980aa7b43ceed0ff8a06ae65bd6f89c8e323a9aae509052b230c929e7c21` |
| `FlexFox80-Checksums-v2.0.0.txt` | 468 | `6702b92e2d87d5cba07c539ed085cee832203719ce0383d50b583a63d1c48206` |

GitHub's asset metadata reports the same sizes and SHA-256 digests.

## Fresh-download verification

All six assets were downloaded with `gh release download` into a new directory. Each downloaded asset compared byte-for-byte with the previously validated local asset. The downloaded ZIP then passed:

- ZIP integrity and exact six-file inventory;
- every internal SHA-256 check;
- manifest format, product, stable channel, exact hardware revision, embedded versions, pinned build profiles, 274-byte EEPROM schema, and `SW_VERSIONS,2.27,0.210` identity;
- AVR Intel HEX parsing with the pinned AVR objcopy;
- ESP sketch image checksum validation with esptool.py v2.8;
- LittleFS parsing under the qualified 8192-byte block, 256-byte page, 1,024,000-byte layout; and
- equality between the manifest commit and annotated tag target.

The downloaded LittleFS image lists exactly:

- `Classic80m.event` (1,104 bytes);
- `FoxO80m.event` (1,153 bytes);
- `Sprint80m.event` (1,934 bytes);
- `defaults.txt` (495 bytes);
- `events.html` (116,373 bytes);
- `favicon.ico` (1,150 bytes);
- `radio.html` (70,887 bytes); and
- `test.html` (87,569 bytes).

The final LittleFS binary hash differs from the earlier development-candidate image because mklittlefs stores input modification timestamps and the clean branch transition refreshed the three web-file timestamps. Two consecutive final-tag builds produced the published hash, the stored file names and sizes are exact, and the individually published ESP sketch and AVR application hashes remain unchanged. LittleFS is explicitly factory/recovery material rather than a routine update.

## Disposition

The GitHub release, six assets, annotated tag, and fresh-download verification all pass. This evidence is committed after the tag as required by the release workflow; the `v2.0.0` tag remains fixed at the manifest commit.
