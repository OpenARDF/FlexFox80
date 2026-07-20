# FlexFox80 v2.0.0 Candidate Qualification — 2026-07-20

**Candidate source:** `6976f80e908526ccbbec44a19e897f72d82ac887`

**Branch:** `Development_AVR128DA48`

**Selected pair:** ESP `2.27`, AVR `0.210`, resident bootloader BL0.3

## Repository gate

The candidate working tree was clean and aligned with `origin/Development_AVR128DA48`. `just check` passed, including documentation and policy checks, host regression tests, firmware protocol/source contracts, release-checklist validation, and the fixed 274-byte AVR EEPROM schema.

## AVR reproducibility

Two fresh Release builds used AVR-GCC 7.3.0 and AVR-Dx_DFP 1.9.103. Both reported zero warnings and matched byte-for-byte across ELF, HEX, EEP, map, listing, S-record, and binary artifacts.

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `45d77bbe78b1e79e12bd5d656b165545bbfdd8558395826f0f3eda7a305b340e` |
| `FlexFox80.hex` | `552736db7a5104d9c508247b00bd63188aa29934ef6ca25afa3760ff6d483359` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `1f5aadea82352d95229d0ddeb6af60fb3363d693af565d3280c5fe9f0e773756` |
| `FlexFox80.lss` | `3cde9fd0b34cf53aea4700f0e30255860a052c792f9e47e46885db3af64f44ae` |
| `FlexFox80.srec` | `440e06984530b96f79f14f459c151cd4f05fbf2335cf90b77bf0e2f3dac9cf9e` |
| `FlexFox80.bin` | `9a0c311e1e94f1f6eaa9001090267617aad306d0c8c46f15afc3b970ac2768d2` |

Resource totals are `text=42638`, `data=1156`, `bss=1585`, `dec=45379`.

## ESP reproducibility

Two fresh builds used ESP8266 core 2.7.4, WebSockets 2.3.6, Adafruit Feather HUZZAH, 80 MHz, 115200 baud, and the 4M1M flash layout. Both reported zero warnings and matched byte-for-byte across sketch, ELF, map, and LittleFS outputs.

| Artifact | Bytes | SHA-256 |
| --- | ---: | --- |
| `ARDF_Transmitter.ino.bin` | 568,912 | `bc4d757bfed672e2537e86e402505e1093f67d002ced94d858ed31b2b3f815fa` |
| `ARDF_Transmitter.ino.elf` | 5,975,944 | `99ea939706e9d9b1cc2aaabca5043e34f551ea2bde80ff95009d9a666e8e3f79` |
| `ARDF_Transmitter.ino.map` | 4,075,330 | `1b1acc6cc8dc7ccbde4916f9bed6ccdbcb4e265fb8cece888acdee9eab11d001` |
| `ARDF_Transmitter.littlefs.bin` | 1,024,000 | `4199c8b87b3c77272939e2c400aa77e04d5f52b1125d8c689b35f87b86b5b476` |

The sketch uses 564,760 bytes (54%) of program storage. Globals use 51,248 bytes (62%), leaving 30,672 bytes. The LittleFS image contains the expected eight files: `Sprint80m.event`, `Classic80m.event`, `FoxO80m.event`, `favicon.ico`, `radio.html`, `defaults.txt`, `test.html`, and `events.html`.

## Fleet evidence

Fifteen uniquely named foxes retain machine-readable final summaries dated 2026-07-20 with `result: pass`, exact expected SSID, selected artifact identities, final ESP `2.27`, AVR `0.210`, BL0.3, and combined `versionsAfter: 2.27,0.210`. The beacon and master/spectator use `Tx_Master` and were upgraded and verified interactively. The release owner confirmed that these 17 units constitute the complete FlexFox fleet, that the update sequence is complete, and that hardware testing is complete.

## Disposition

The candidate package contains exactly the six files required by the release contract. A separate clean extraction passed ZIP integrity, internal SHA-256 checks, manifest identity/profile checks, AVR Intel HEX parsing, ESP esptool checksum validation, and LittleFS image inspection. The candidate ZIP is 495,923 bytes with SHA-256 `4ac04a1a739afaa3dab533c488a19de71038484ecbf8ecb85006452eaceec1f1`.

The candidate passes the development-branch release gate. The stable GitHub release still requires clean-main integration, clean-main reproduction, final package regeneration and validation, annotated tagging, publication, and fresh-download archive verification.
