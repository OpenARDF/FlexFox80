# FlexFox80 v1.0.0 Maintenance Release Record

**Status:** Released and independently archive-verified on 2026-07-14

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Embedded versions:** AVR `0.201`, ESP `2.1`

**Candidate source commit:** `10dd7c200bc1d63a13b174b2833b452801ae57a8`

**Integrated main reproduction:** `2dd843859da4e10561704534779a62964374f803`

**Tagged release commit:** `b4b98165af2ece0ed442ce23311be07df57c0231`

**GitHub release:** [FlexFox80 v1.0.0](https://github.com/OpenARDF/FlexFox80/releases/tag/v1.0.0)

## Published release

- Annotated tag object: `1ce3b05909014ae8df50c0d246d042550ef47fb3`.
- Tag target and package-manifest commit: `b4b98165af2ece0ed442ce23311be07df57c0231`.
- Final ZIP: `FlexFox80-v1.0.0-AVR-0.201-ESP-2.1-Release-Files.zip`, 451,859 bytes, SHA-256 `17e126cefbc8ecc7281658057fc07278dceada6711e54fe80a4569b8424b655b`.
- All six GitHub assets were downloaded into a clean directory and matched the validated local files byte-for-byte.
- The downloaded ZIP passed checksums, manifest identity, AVR HEX parsing, ESP checksum validation, LittleFS inspection, and tag-target comparison.

See [GitHub release verification](../../Evidence/V1_0_0_GITHUB_RELEASE_VERIFICATION_2026-07-14.md) for the archived asset inventory and independent validation evidence.

## Candidate evidence

- `just check` passes at the clean candidate commit.
- Two pinned AVR-GCC 7.3.0 / AVR-Dx_DFP 1.9.103 Release builds are warning-free and byte-identical.
- Two pinned ESP8266 core 2.7.4 / WebSockets 2.3.6 builds are warning-free and byte-identical, including the selected LittleFS image.
- The candidate ZIP was assembled under `/private/tmp`, extracted into a separate clean directory, and checked for ZIP integrity, manifest identity, SHA-256 values, AVR Intel HEX parsing, ESP image checksum, and LittleFS contents.
- Candidate ZIP: `FlexFox80-v1.0.0-AVR-0.201-ESP-2.1-Release-Files.zip`, 451,743 bytes, SHA-256 `08166cf8105fcea0d589de54b09ddeb67ac883370a970647e8ac30d0717ab1ec`.
- The exact selected pair was independently programmed and verified on a representative Ver 2.1 master. Its preserved configuration, normal startup and telemetry, and live `SW_VERSIONS,2.1,0.201` response pass the final candidate hardware gate.

The candidate package is intentionally temporary. The final package is regenerated from the exact clean `main` tag commit so its manifest, tag target, branch, and archived hashes agree.

## Intended GitHub assets

- complete validated release ZIP;
- `FlexFox80-AVR-0.201.hex`;
- `FlexFox80-ESP-2.1.bin`;
- `FlexFox80-LittleFS-2.1.bin` labeled as recovery/factory content;
- release manifest and checksum list.

## Owner decisions recorded

On 2026-07-14, the release owner approved:

1. product release label `v1.0.0` for the first tagged maintenance baseline;
2. the proposed maintainer release notes;
3. the remaining broad A8 tests as explicit v1.0.0 deferrals rather than silent passes; and
4. the documented reference rollback set, with routine per-unit AVR EEPROM preservation and full-device backups only when specifically requested or separately justified; and
5. final rebuilding, annotated `v1.0.0` tag creation, and GitHub publication after the clean-main reproduction passed.

See [hardware disposition](hardware-disposition.md) and [rollback record](rollback.md).

## Candidate disposition

The candidate checklist passes and integration is approved. [Installed-pair verification](../../Evidence/V1_0_0_INSTALLED_PAIR_VERIFICATION_2026-07-14.md) records the exact flash, EEPROM, fuse, configuration, identity, telemetry, and combined-version evidence. The approved replacement-tree merge established the qualified current-product tree on `main` while preserving the former tip at `legacy-main-before-avr128da48-2026-07-14`; the redundant `AVR128DA48` branch was removed. [Clean-main reproduction](../../Evidence/V1_0_0_MAIN_RELEASE_REPRODUCTION_2026-07-14.md) records the final repository and deterministic-build gate. The release owner then authorized final rebuilding, tagging, and GitHub publication.

See [release checklist](release-checklist.json), [approved maintainer notes](release-notes.md), and the repository [release workflow](../../RELEASE_WORKFLOW.md).
