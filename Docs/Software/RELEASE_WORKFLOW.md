# FlexFox80 Release and Branch Integration Workflow

## Current branch model

| Branch | Role |
| --- | --- |
| `Development_AVR128DA48` | Active hardening and bug-development branch |
| `AVR128DA48` | Integration and release baseline for current hardware |
| `main` | Preserved earlier lineage until the approved AVR128DA48 overwrite transition |
| `ATMEGA328p` | Historical hardware-generation branch |

The current branch names intentionally expose the hardware transition. They can be simplified after `main` becomes the AVR128DA48 product line.

## Release authority and phases

- `Development_AVR128DA48` may produce a release candidate or explicitly approved prerelease.
- A stable release must be integrated into and reproduced from `AVR128DA48` first.
- The later replacement-tree transition to `main` is a separate operation and must not be combined with a firmware release.
- Creating a tag, distributing release artifacts, or programming a fleet requires explicit owner approval; completion of local engineering gates does not imply deployment authority.

FlexFox releases support one hardware target only: **Ver 2.1 (Mar 2022)**. Unlike SignalSlinger, the FlexFox release process does not build or publish parallel board variants. Earlier prototypes and any future revision require a separate compatibility decision before they can use these release assets.

FlexFox is a mature, actively used legacy product rather than a new general-audience product. Its release process is therefore maintainer-oriented and proportional to operational risk. Reproducible firmware, exact evidence, rollback, and installed-device verification are essential. GitHub is the normal durable release archive, but public launch material and broad-audience packaging are unnecessary. An internal-only archive remains available when the owner explicitly requests one.

The standard release has three checklist phases:

1. `candidate` proves the frozen source, deterministic builds, package, hardware evidence, rollback, and release notes are ready for an integration decision.
2. `release` additionally proves explicit integration and release approval and reproduces the artifacts from the clean `AVR128DA48` release commit.
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

## Development integration checklist

Before integrating `Development_AVR128DA48` into `AVR128DA48`:

1. Confirm the roadmap checkpoint or bug closure being integrated.
2. Confirm all intended commits are pushed to `origin/Development_AVR128DA48`.
3. Confirm the worktree is clean.
4. Fetch `origin` and compare:

   ```text
   Development_AVR128DA48
   origin/Development_AVR128DA48
   AVR128DA48
   origin/AVR128DA48
   ```

5. Run the full verification available at that roadmap stage.
6. Review the commit list and diff from `AVR128DA48` to `Development_AVR128DA48`.
7. Fast-forward when `AVR128DA48` is exactly behind development. Do not manufacture a merge commit when histories are already linear.
8. If `AVR128DA48` has independent commits, stop and review the divergence before selecting a merge or rebase strategy.
9. Push the verified integration result to `origin/AVR128DA48`.
10. Return to `Development_AVR128DA48` and verify local/remote alignment.

## Firmware release gate

A firmware release from `AVR128DA48` requires:

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

1. Fast-forward or otherwise perform the separately reviewed integration into `AVR128DA48`.
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

Before replacing AVR firmware on a configured unit:

1. read flash, the complete EEPROM address space, and all fuse bytes through the selected programmer;
2. record hashes and repeat critical reads to establish a stable backup;
3. classify the captured EEPROM by validated field offsets; do not infer its ABI only from the installed flash image;
4. if and only if the image validates as the historical 268-byte Release layout, migrate it with `node scripts/migrate-eeprom-enum-layout.mjs --from legacy-268 --input <backup> --output <new-image>`;
5. confirm the selected image's 274-byte EEPROM ABI and inactive/safe boot configuration;
6. erase and program flash using a procedure that supports required 0-to-1 transitions;
7. restore the validated 274-byte EEPROM image when erase retention has not been independently proven;
8. verify the input flash and EEPROM through the programmer;
9. perform independent post-operation reads and compare flash, EEPROM, and fuses byte-for-byte;
10. retain the prior flash image and raw unit-specific backup outside Git until functional qualification is complete.

Do not rely on avrdude `-D` alone to preserve EEPROM while replacing arbitrary flash. Without an erase, required 0-to-1 transitions can leave a mixed image even though page writes were attempted. The first Mac procedure and recovery are recorded in [MAC_AVR_PROGRAMMING_2026-07-12.md](Evidence/MAC_AVR_PROGRAMMING_2026-07-12.md).

Do not force a layout conversion when the plausibility gate rejects the raw EEPROM. Preserve the original image and diagnose it separately; a rejected image may be mixed, damaged, or from another historical layout.

## Future AVR128DA48 overwrite transition to main

The transition is intentionally deferred until the current hardware line passes the agreed hardening and release gates.

Before the transition:

1. Confirm `AVR128DA48` is the approved product baseline and is fully pushed.
2. Preserve the existing `main` tip with an annotated tag and/or clearly named legacy branch.
3. Record the old and new commit IDs and complete tree hashes.
4. Verify that release artifacts can be reproduced from `AVR128DA48`.
5. Review the exact replacement-tree diff, including software, KiCad, documentation, and ignored/tracked artifact policy.
6. Prepare a rollback procedure that restores the preserved legacy ref.
7. Obtain explicit approval for the overwrite integration.

The overwrite operation must preserve history while making the resulting `main` tree exactly match the approved AVR128DA48 tree. The exact Git commands will be scripted and dry-run in a temporary worktree before they are used on `main`.

After the transition:

- `main` becomes the stable current-product branch;
- a development branch remains the active checkout;
- future integrations prefer fast-forward merges when history is linear;
- branch protection and required checks should be configured to match the new workflow;
- legacy hardware remains reachable through its preserved refs.

## Prohibited shortcuts

- Do not force-push `main` or `AVR128DA48` without a separately reviewed recovery plan.
- Do not overwrite `main` merely to simplify branch names.
- Do not publish Debug artifacts as a release substitute.
- Do not flash an AVR Release image over retained deployed EEPROM if its `.eeprom` object is not exactly 274 bytes (`0x112`).
- Do not combine branch migration, artifact cleanup, and firmware behavior changes in one commit or integration.
- Do not skip hardware safety checks silently.
