# FlexFox80 Release and Branch Integration Workflow

## Current branch model

| Branch | Role |
| --- | --- |
| `Development_AVR128DA48` | Active hardening and bug-development branch |
| `main` | Stable release branch and default GitHub branch for current Ver 2.1 hardware |
| `ATMEGA328p` | Historical hardware-generation branch |

The transitional `AVR128DA48` branch was removed after the approved 2026-07-14 replacement-tree merge established the current product line on `main`. Its history remains reachable through `main`, `Development_AVR128DA48`, and release history.

## Release authority and phases

- `Development_AVR128DA48` may produce a release candidate or explicitly approved prerelease.
- A stable release must be integrated into and reproduced from a clean `main` release commit.
- After a release integration, fast-forward `Development_AVR128DA48` through the resulting `main` commit before resuming development. This keeps later release integrations linear when no independent release-only commit intervenes.
- Creating a tag, distributing release artifacts, or programming a fleet requires explicit owner approval; completion of local engineering gates does not imply deployment authority.

FlexFox releases support one hardware target only: **Ver 2.1 (Mar 2022)**. Unlike SignalSlinger, the FlexFox release process does not build or publish parallel board variants. Earlier prototypes and any future revision require a separate compatibility decision before they can use these release assets.

FlexFox is a mature, actively used legacy product rather than a new general-audience product. Its release process is therefore maintainer-oriented and proportional to operational risk. Reproducible firmware, exact evidence, rollback, and installed-device verification are essential. GitHub is the normal durable release archive, but public launch material and broad-audience packaging are unnecessary. An internal-only archive remains available when the owner explicitly requests one.

The standard release has three checklist phases:

1. `candidate` proves the frozen source, deterministic builds, package, hardware evidence, rollback, and release notes are ready for an integration decision.
2. `release` additionally proves explicit integration and release approval and reproduces the artifacts from the clean `main` release commit.
3. `final` additionally proves the annotated tag, durable release record, archived artifacts, and independent archive verification.

## Release-specific checklist

Adapted from SignalSlinger, every release uses a machine-readable checklist copied from [release-checklist-template.json](release-checklist-template.json). Store the release-specific copy under `Docs/Software/Releases/<release>/release-checklist.json` so future maintainers can distinguish reusable policy from completed evidence.

Each required item must be either:

- `done`, with non-empty `evidence`; or
- `skipped`, with both `skipReason` and `skipRequestedBy` recording the owner's explicit decision.

Never convert missing evidence into a silent pass. Validate the checklist before integration review, before declaring the maintenance release, and after archive verification:

```text
just release-checklist Docs/Software/Releases/<release>/release-checklist.json candidate
just release-checklist Docs/Software/Releases/<release>/release-checklist.json release
just release-checklist Docs/Software/Releases/<release>/release-checklist.json final
```

The final checklist update may be a post-release documentation commit. It must not move or recreate the annotated release tag.

## Development-to-main release integration checklist

Before integrating `Development_AVR128DA48` into `main`:

1. Confirm the roadmap checkpoint or bug closure being integrated.
2. Confirm all intended commits are pushed to `origin/Development_AVR128DA48`.
3. Confirm the worktree is clean.
4. Fetch `origin` and compare:

   ```text
   Development_AVR128DA48
   origin/Development_AVR128DA48
   main
   origin/main
   ```

5. Run the full verification available at that roadmap stage.
6. Review the commit list and diff from `main` to `Development_AVR128DA48`.
7. Fast-forward when `main` is exactly behind development. Do not manufacture a merge commit when histories are already linear.
8. If `main` has independent commits, stop and review the divergence before selecting a merge strategy; never rebase or force-push published release history.
9. Push the verified integration result to `origin/main`.
10. Fast-forward `Development_AVR128DA48` through the resulting `main` commit when necessary, return the checkout to development, and verify both local/remote alignments.

## Firmware release gate

A firmware release from `main` requires:

- a clean checkout at the intended commit;
- an approved product release label and stable/prerelease channel;
- the sole supported hardware target recorded exactly as `Ver 2.1 (Mar 2022)`;
- recorded AVR and ESP source versions;
- pinned build tools and dependencies;
- Release builds of both processors;
- build warnings and resource usage recorded;
- SHA-256 hashes for release artifacts;
- a machine-readable manifest, checksum list, plain-language package README, validated release ZIP, and independent package-validation result;
- protocol compatibility verification;
- the applicable automated regression suite;
- an AVR linker map showing the deployed 274-byte (`0x112`) EEPROM schema, with the fixed-width layout contract and AVR compile-time assertion passing;
- the versioned hardware checklist, including approved skips;
- rollback artifacts and flashing instructions;
- concise maintainer-readable release notes.
- a representative installed pair reporting the expected `SW_VERSIONS,<ESP>,<AVR>` value;
- an annotated tag that points to the verified release commit;
- archived assets that pass the same hashes and package validation as the verified local release set.

Checked-in Debug artifacts remain historical evidence, not proof of a current release build. Use the pinned AVR and hardware-compatible ESP development wrappers for current build evidence, and preserve the exact mature ESP rollback image outside Git. Full release status still requires two-unit clone and broader hardware qualification.

## Release identity

The release process must align:

- the owner-approved product release label and annotated tag;
- AVR `SW_REVISION` and its source commit;
- ESP8266 firmware version/date and its source commit;
- event-file format compatibility;
- hardware target `Ver 2.1 (Mar 2022)`;
- AVR image hash;
- ESP image and LittleFS image hashes.

The product release label is independent of the two embedded version strings. Do not infer the Git tag from only the AVR or ESP version, and do not reuse or move a published tag. The manifest, package filenames, release notes, and live combined version report must all identify the same pair.

Do not identify a release by a checked-in binary timestamp alone.

## Artifact policy

- Build products belong in a temporary output directory or release package, not ordinary source commits.
- Release images should be published as release assets with checksums.
- Package release assets according to [release package format](RELEASE_PACKAGE_FORMAT.md); do not publish a hand-selected build directory.
- Existing tracked AVR Debug artifacts remain in place until a dedicated cleanup evaluates and preserves their historical evidence value.
- Manufacturing outputs such as intentional Gerber packages are not firmware build products and must not be hidden by overly broad ignore rules.

## Package, tag, and archive sequence

After the `candidate` checklist passes and integration is explicitly approved:

1. Fast-forward or otherwise perform the separately reviewed integration into `main`.
2. From a clean checkout of the exact integrated commit, rerun `just check` and both deterministic Release builds.
3. Assemble the release package, manifest, checksums, and package README defined by [release package format](RELEASE_PACKAGE_FORMAT.md), manually or with a qualified helper.
4. Validate the package independently rather than trusting the assembly step's success.
5. Complete the `release` checklist and obtain explicit release approval.
6. Create and push an annotated `vX.Y.Z` tag pointing to the verified release commit. Record the tag object and peeled commit IDs.
7. Create a durable release record containing the approved maintainer notes, hardware target, embedded version pair, verification summary, and approved deferrals.
8. Store the validated ZIP, AVR HEX, ESP sketch BIN, ESP LittleFS BIN, manifest, and checksums in the GitHub release unless an internal-only archive was explicitly approved. Label the LittleFS image as recovery/factory content rather than a routine update.
9. Verify the archived tag target, record, asset names, byte sizes, and hashes.
10. Copy the archived package into a clean temporary directory and rerun package validation.
11. Mark the final checklist with archive locations and verification evidence, then commit that final record without moving the release tag.

The current repository has deterministic AVR and ESP build wrappers but does not yet have a package builder or independent package validator. A builder is optional convenience for this legacy product. Step 4 still requires an independent, recorded structure-and-hash check; that may initially be a carefully documented manual check, with automation added when its repeat value justifies the work.

## AVR programming and rollback

Routine programming does not require a full archival backup of each AVR or ESP device. Create a full-device backup only when the owner requests one or when a separately documented recovery risk makes it necessary. Normal rollback uses the versioned release firmware and the established reference recovery set.

AVR EEPROM is different from a full-device backup: it contains unit-specific configuration and may need to be restored after the required chip erase. Before replacing AVR firmware on a configured unit:

1. read and verify the complete EEPROM address space; read fuses for a before/after comparison, but do not write them during a routine firmware update;
2. record hashes and repeat the EEPROM read to establish a stable unit-specific configuration image;
3. classify the captured EEPROM by validated field offsets; do not infer its ABI only from the installed flash image;
4. if and only if the image validates as the historical 268-byte Release layout, migrate it with `node scripts/migrate-eeprom-enum-layout.mjs --from legacy-268 --input <backup> --output <new-image>`;
5. confirm the selected image's 274-byte EEPROM ABI and inactive/safe boot configuration;
6. erase and program flash using a procedure that supports required 0-to-1 transitions;
7. restore the validated 274-byte EEPROM image when erase retention has not been independently proven;
8. verify the input flash and EEPROM through the programmer;
9. perform independent post-operation reads and compare the installed release flash, restored EEPROM, and unchanged fuses byte-for-byte;
10. retain the raw unit-specific EEPROM outside Git until functional qualification is complete. Preserve prior AVR flash or a complete device image only when specifically requested or separately justified.

Do not rely on avrdude `-D` alone to preserve EEPROM while replacing arbitrary flash. Without an erase, required 0-to-1 transitions can leave a mixed image even though page writes were attempted. The first Mac procedure and recovery are recorded in [MAC_AVR_PROGRAMMING_2026-07-12.md](Evidence/MAC_AVR_PROGRAMMING_2026-07-12.md).

Do not force a layout conversion when the plausibility gate rejects the raw EEPROM. Preserve the original image and diagnose it separately; a rejected image may be mixed, damaged, or from another historical layout.

For a routine ESP sketch update, write and verify only the release sketch at `0x0`; do not erase or replace LittleFS. A full 4 MiB ESP backup is optional and is made only when specifically requested or separately justified. A LittleFS replacement is a distinct recovery/factory operation and requires explicit authorization.

## Completed transition to main

On 2026-07-14, the owner approved the one-time replacement-tree merge that established the qualified AVR128DA48 product line on `main`:

- old `main`: `acf7b586b6cccd28fa7aa0e8ddeeb3079d6e5880`;
- preserved annotated tag: `legacy-main-before-avr128da48-2026-07-14`;
- qualified development parent: `56d1f479358bf4163e98a1d0abe7c9970857b926`;
- replacement merge: `17b31195e77a3617f6f0e5aa66b3ed54f4de745f`;
- resulting tree: `3b792e14ad2ea7b57c07a9dd42dcca2c390a6f73`, exactly matching the qualified development tree.

The merge was created and checked in a temporary detached worktree, passed `just check` and the v1.0.0 candidate checklist, preserved both parent histories, and was pushed normally without force. `Development_AVR128DA48` was then fast-forwarded through the merge. The redundant `AVR128DA48` branch was deleted locally and remotely at the owner's direction.

## Prohibited shortcuts

- Do not force-push `main` or rewrite published release history.
- Do not overwrite `main` merely to simplify branch names.
- Do not publish Debug artifacts as a release substitute.
- Do not flash an AVR Release image over retained deployed EEPROM if its `.eeprom` object is not exactly 274 bytes (`0x112`).
- Do not combine branch migration, artifact cleanup, and firmware behavior changes in one commit or integration.
- Do not skip hardware safety checks silently.
