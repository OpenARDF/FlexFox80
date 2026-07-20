# FlexFox80 v2.0.0 GitHub Release Verification — 2026-07-20

**Release:** <https://github.com/OpenARDF/FlexFox80/releases/tag/v2.0.0>

**Published:** 2026-07-20T18:41:37Z

**First-install asset amendment:** 2026-07-20T18:57:18Z

**Tag object:** `9d2117c2db893ff2a7170f39c11b4f5c47065e77`

**Peeled tag and manifest commit:** `b3a05fee2dcf47d2ec6b99e92b4b0d5fc36d5556`

## Release record

GitHub reports `FlexFox80 v2.0.0` as a stable release: `isDraft=false` and `isPrerelease=false`. The annotated `v2.0.0` tag was pushed before publication and peels to the exact full commit recorded by the release manifest.

The release was amended on the publication date to add a SignalSlinger-style combined first-install AVR image for convenient Atmel-ICE/UPDI provisioning of future FlexFoxes. The release tag was not moved. The combined image was generated with the existing qualified FlexFox boot-chain packager from the same release source and matching AVR 0.210 application.

## Published assets

| Asset | Bytes | SHA-256 |
| --- | ---: | --- |
| `FlexFox80-v2.0.0-AVR-0.210-ESP-2.27-Release-Files.zip` | 547,206 | `b9cad788d6f75ecc8d976e6f55c1f708a2b85c721247eab7463e4d73eafcf48e` |
| `FlexFox80-AVR-0.210.hex` | 122,426 | `552736db7a5104d9c508247b00bd63188aa29934ef6ca25afa3760ff6d483359` |
| `FlexFox80-AVR-First-Install-0.210.hex` | 135,180 | `6d3969af6d9cf9966219e76bd3a5e7760c60893b02edc6fd78713260c6586eea` |
| `FlexFox80-ESP-2.27.bin` | 568,912 | `bc4d757bfed672e2537e86e402505e1093f67d002ced94d858ed31b2b3f815fa` |
| `FlexFox80-LittleFS-2.27.bin` | 1,024,000 | `e13c5ab7cf00d5ee9db8daae639da8d4f35f9c056673770e0ccb13b18bece022` |
| `FlexFox80-Release-Info-v2.0.0.json` | 2,045 | `b2958a7e9540fb73f88117091f1df63c91685aeaa2a80c9d8e61f7b4c1bb8b91` |
| `FlexFox80-Checksums-v2.0.0.txt` | 572 | `07f93bf97455f6520f98b9aacb2ec8bbc4835037d7f022d05dd4c74826bdcf37` |

GitHub's asset metadata reports the same sizes and SHA-256 digests.

## Fresh-download verification

All seven assets were downloaded with `gh release download` into a new directory. Their SHA-256 digests exactly match the table above. The downloaded ZIP then passed:

- ZIP integrity and exact seven-file inventory;
- every internal SHA-256 check;
- manifest format, product, stable channel, exact hardware revision, embedded versions, pinned build profiles, 274-byte EEPROM schema, and `SW_VERSIONS,2.27,0.210` identity;
- AVR Intel HEX parsing with the pinned AVR objcopy;
- exact extraction of the 5,112-byte BL0.3 binary at address zero and the 43,520-byte relocated AVR 0.210 application at address `0x4000` from the combined first-install HEX;
- first-install manifest identity for BL0.3, protocol 2, 38,400 baud, application start `0x4000`, and required `CODESIZE=0x00` and `BOOTSIZE=0x20` fuse fields;
- ESP sketch image checksum validation with esptool.py v2.8;
- LittleFS parsing under the qualified 8192-byte block, 256-byte page, 1,024,000-byte layout; and
- equality between the manifest commit and annotated tag target.

The combined first-install build was run twice with pinned AVR-GCC 7.3.0 and AVR-Dx_DFP 1.9.103 inputs. Both warning-free runs were byte-identical. The HEX does not write fuses or EEPROM; the qualified provisioning workflow must preserve/verify unit EEPROM and set/verify `CODESIZE=0x00` and `BOOTSIZE=0x20` separately.

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

The GitHub release, seven assets, combined BL0.3 plus AVR 0.210 first-install image, annotated tag, and fresh-download verification all pass. This evidence is committed after the tag as required by the release workflow; the `v2.0.0` tag remains fixed at the manifest commit.
