# FlexFox80 Release and Branch Integration Workflow

## Current branch model

| Branch | Role |
| --- | --- |
| `Development_AVR128DA48` | Active hardening and bug-development branch |
| `AVR128DA48` | Integration and release baseline for current hardware |
| `main` | Preserved earlier lineage until the approved AVR128DA48 overwrite transition |
| `ATMEGA328p` | Historical hardware-generation branch |

The current branch names intentionally expose the hardware transition. They can be simplified after `main` becomes the AVR128DA48 product line.

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
- recorded AVR and ESP source versions;
- pinned build tools and dependencies;
- Release builds of both processors;
- build warnings and resource usage recorded;
- SHA-256 hashes for release artifacts;
- protocol compatibility verification;
- the applicable automated regression suite;
- an AVR linker map showing the deployed 274-byte (`0x112`) EEPROM schema, with the fixed-width layout contract and AVR compile-time assertion passing;
- the versioned hardware checklist, including approved skips;
- rollback artifacts and flashing instructions;
- user-readable release notes.

Checked-in Debug artifacts remain historical evidence, not proof of a current release build. Use the pinned AVR and hardware-compatible ESP development wrappers for current build evidence, and preserve the exact mature ESP rollback image outside Git. Full release status still requires two-unit clone and broader hardware qualification.

## Release identity

The release process must align:

- AVR `SW_REVISION` and its source commit;
- ESP8266 firmware version/date and its source commit;
- event-file format compatibility;
- board/hardware revision;
- AVR image hash;
- ESP image and LittleFS image hashes.

Do not identify a release by a checked-in binary timestamp alone.

## Artifact policy

- Build products belong in a temporary output directory or release package, not ordinary source commits.
- Release images should be published as release assets with checksums.
- Existing tracked AVR Debug artifacts remain in place until a dedicated cleanup evaluates and preserves their historical evidence value.
- Manufacturing outputs such as intentional Gerber packages are not firmware build products and must not be hidden by overly broad ignore rules.

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
