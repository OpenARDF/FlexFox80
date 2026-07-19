# ESP Interrupted File-Upload Recovery — 2026-07-19

**Status:** ESP 2.23 source/build/contract gates pass; the recovered fleet unit passes exact flash, filesystem, standalone HTTP, reinstall, and combined ESP/AVR telemetry verification

## Failure and retained evidence

During the fleet upgrade of `Tx_C22DD117`, the ESP and AVR firmware had already reached ESP 2.22 / BL0.3 / AVR 0.208. The Moto connection then disappeared while `/test.html` was being installed through one long HTTP upload. After a whole-unit restart, the ESP advertised only its fallback SDK access point and never brought up the product HTTP service.

The ESP was placed in its serial bootloader and its complete 4 MiB flash was read twice: the retained image verified against the target and had SHA-256 `410db44789e8ff5703107c18828ba28bd88989235f5e5611e7d42f7d5a24cda1`. The 1,024,000-byte LittleFS partition at `0x300000` unpacked successfully with the qualified 8,192-byte block and 256-byte page geometry. It contained:

- `/test.html.__bak`, 86,826 bytes;
- `/test.html.__uploading`, 87,115 bytes, byte-for-byte equal to the repository `/test.html` with SHA-256 `be6fbcf7a019f06ae9424e4403b65498ed2d968149f431f3baea7f0160f85e6d`;
- no live `/test.html`;
- intact unit-specific `.event`, `.me`, and default files;
- repository-exact `/events.html` and `/radio.html`.

This is direct evidence of interruption between the two transactional rename steps. Startup recovery found the backup and staging artifacts, but its loop also synchronously deleted stale large files before starting HTTP. On this hardware that work could outlast the AVR's ESP-power keep-alive window and repeat on every boot, leaving the product service unreachable. The ordinary upload handler also depended on the browser WebSocket heartbeat even though one large HTTP request can prevent that socket from being serviced.

## ESP 2.23 correction

ESP 2.23 reuses the existing firmware-update keep-alive path during every ordinary file-upload phase and services Linkbus while receiving data. Startup recovery now performs at most one best-effort metadata rename: if a backup is the only complete live copy, it restores that file and returns; it does not reclaim staging or redundant backup files during startup. The next explicitly kept-awake upload transaction reclaims stale artifacts before opening a new staging file.

Upload-start cleanup is fail-safe:

- an orphan backup is restored when the live target is missing;
- a redundant backup is removed only when the live target already exists;
- stale staging and backup cleanup must report success before a new transaction begins;
- failure leaves the only complete copy intact and aborts the upload.

The host firmware contract checks require this bounded startup behavior, both cleanup failures, and ordinary-upload keep-alive coverage. This correction does not alter the resident AVR bootloader, wireless AVR protocol, event files, or `.me` files.

## Build and recovery verification

The pinned ESP 2.23 build completed without warnings:

- sketch: 563,952 bytes;
- source artifact SHA-256: `4602dfa8818bee730cf11c0f5c1d08d47010a3c146dd5d2352e8a01c9e9e5421`;
- dynamic memory: 51,032 bytes used, 30,888 bytes free;
- IRAM: 27,676 of 32,768 bytes used.

The recovered LittleFS image retained every unit-specific file, promoted the complete staged `/test.html`, and omitted both transaction artifacts. A pack/unpack round trip reproduced its source directory exactly. The sketch and repaired filesystem were written together through FTDI and independently verified. ESP8266 installation normalized only the sketch flash-mode header from mode 0 to DIO mode 2; the installed-image MD5 is `b6749081bfda4acefdcd9014e0284dfd`.

With the HUZZAH standalone, the Moto obtained `73.73.73.74`, reached `73.73.73.73`, received HTTP 200, and read ESP 2.23 with `filesystemProtected=true`. The downloaded files exactly matched the repository:

| File | Bytes | SHA-256 |
| --- | ---: | --- |
| `/test.html` | 87,115 | `be6fbcf7a019f06ae9424e4403b65498ed2d968149f431f3baea7f0160f85e6d` |
| `/events.html` | 116,029 | `40d679f930a1d636daefb11cc4410fc371165ec1e29110d2070a48c550d8ff14` |
| `/radio.html` | 70,432 | `42724169bb1fb4dcabfa1987ce99c62c7ece4495e77b7a59ab2f92a722ea92ca` |

After reinstalling the HUZZAH in the FlexFox, the read-only fleet preflight returned `SW_VERSIONS,2.23,0.208`, SSID `Tx_C22DD117`, normal 23 C temperature, 11.0 V battery, and live bidirectional Linkbus telemetry. The guarded full fleet workflow then:

1. skipped the exact installed ESP image;
2. independently verified and skipped the exact BL0.3 / AVR 0.208 boot-chain image and fuses;
3. hash-verified and skipped all three exact web files;
4. repeated the live combined probe at 25 C and 11.0 V;
5. reported `PASS: tx-c22dd117 is complete at ESP 2.23, BL0.3, AVR 0.208.`

The ignored workflow evidence directories are timestamped `2026-07-19T14-53-55.912Z` and `2026-07-19T14-54-19.279Z` under `Software/AVR128DA48/tmp/fleet-upgrade/`. The complete pre-recovery flash capture remains an ignored local recovery artifact and is not a distributable firmware image.

## Disposition

The failure was in the ordinary LittleFS file-upload/recovery path, not in either bootloader. ESP 2.23 closes the reproduced boot loop while preserving unit data and reusing the already-qualified keep-alive mechanism. The recovered unit passes the exact installed-artifact and live hardware gates, so ESP 2.23 supersedes ESP 2.22 for the remaining fleet rollout and should be installed wirelessly on units already provisioned with 2.22.
