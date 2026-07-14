# FlexFox80 v1.0.0 Maintenance Release Record

**Status:** Candidate preparation; not tagged, published, or approved for fleet deployment

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Embedded versions:** AVR `0.201`, ESP `2.1`

**Candidate source commit:** `10dd7c200bc1d63a13b174b2833b452801ae57a8`

## Candidate evidence

- `just check` passes at the clean candidate commit.
- Two pinned AVR-GCC 7.3.0 / AVR-Dx_DFP 1.9.103 Release builds are warning-free and byte-identical.
- Two pinned ESP8266 core 2.7.4 / WebSockets 2.3.6 builds are warning-free and byte-identical, including the selected LittleFS image.
- The candidate ZIP was assembled under `/private/tmp`, extracted into a separate clean directory, and checked for ZIP integrity, manifest identity, SHA-256 values, AVR Intel HEX parsing, ESP image checksum, and LittleFS contents.
- Candidate ZIP: `FlexFox80-v1.0.0-AVR-0.201-ESP-2.1-Release-Files.zip`, 451,743 bytes, SHA-256 `08166cf8105fcea0d589de54b09ddeb67ac883370a970647e8ac30d0717ab1ec`.

The candidate package is intentionally temporary. The final package must be regenerated from the clean integrated `AVR128DA48` commit so its manifest, tag target, branch, and archived hashes agree.

## Intended GitHub assets

- complete validated release ZIP;
- `FlexFox80-AVR-0.201.hex`;
- `FlexFox80-ESP-2.1.bin`;
- `FlexFox80-LittleFS-2.1.bin` labeled as recovery/factory content;
- release manifest and checksum list.

## Decisions still required

1. Approve product release label `v1.0.0` for the first tagged maintenance baseline.
2. Approve or revise the proposed hardware-test deferrals recorded in the release hardware checklist.
3. Select and verify the retained AVR and ESP rollback archives.
4. Install or identify a representative paired unit that reports `SW_VERSIONS,2.1,0.201` and retains its expected configuration.
5. Approve fast-forward integration into `AVR128DA48`, followed by final clean-branch builds, packaging, tag, and GitHub release.

See [release checklist](release-checklist.json), [candidate notes](release-notes.md), and the repository [release workflow](../../RELEASE_WORKFLOW.md).
