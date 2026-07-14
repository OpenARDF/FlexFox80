# FlexFox80 Development Workflow

## Branch roles

- `Development_AVR128DA48` is the active development branch. Routine hardening and bug work starts here and returns here after release integration.
- `main` is the stable release branch and default GitHub branch for the current Ver 2.1 product line.
- The transitional `AVR128DA48` branch was removed after the approved 2026-07-14 replacement-tree merge. Do not recreate it as an integration layer.
- `ATMEGA328p` and any other legacy hardware branches are historical product lines. Do not merge them into current firmware work without a specific compatibility objective.

## Before editing

1. State the current branch in the first work update.
2. Run `git status --short --branch` and identify pre-existing changes.
3. Confirm that the task belongs on `Development_AVR128DA48` unless it is an explicitly authorized integration, release, or legacy-maintenance operation.
4. Read [GENERATED_FILES.md](GENERATED_FILES.md) before changing IDE-generated, vendor-managed, build-output, or CAD files.
5. Identify the behavior that must remain unchanged and the evidence required for the proposed change.

Pre-existing changes belong to the user. Do not alter, discard, stage, or publish them unless the user explicitly includes them in the task.

## Mac and Windows coordination

Use the repository-root [Codex mailbox](../../CODEX_MAILBOX.md) for cross-machine requests and replies. Both agents communicate by fetching, editing that file, committing the mailbox/evidence update, and pushing `Development_AVR128DA48`.

- Preserve unacknowledged messages from the other agent.
- Put durable results under `Docs/Software/Evidence/`; the mailbox is the coordination channel, not the only record.
- Transfer toolchains, device packs, firmware images, and other large binaries outside Git. Record filenames, byte sizes, and SHA-256 hashes so the transfer can be verified.
- Never resolve a mailbox push conflict with a force-push. Fetch and preserve both agents' messages.
- Polling the mailbox means fetching `origin` and reviewing new commits on `origin/Development_AVR128DA48`; do not assume the local file is current.

Exact same-source Windows parity has now been demonstrated for the accumulated `3bc10a5` hardening snapshot and the adjacent `912d24b` Linkbus message-ID correction. In both cases Windows and Mac produced matching resource totals, Intel HEX, EEPROM output, and EEPROM layout; host-sensitive ELF, map, listing, and S-record representations were deterministic on each host and the differences were explained.

Routine hardening slices therefore do not require a separate Windows mailbox round trip when the pinned Mac wrapper reports the exact AVR-GCC 7.3.0 and `AVR-Dx_DFP` 1.9.103 versions, produces deterministic warning-free builds, passes the full repository suite, and the applicable Mac target test passes. Request fresh Windows verification when:

- the AVR compiler, device pack, build flags, linker behavior, or wrapper changes;
- Mac output is nondeterministic, warning-bearing, or differs unexpectedly from the established resource/layout baseline;
- a change is sensitive to host paths, generated project behavior, Microchip Studio, or Windows-only tooling;
- a hardening release candidate reaches the cross-platform release gate;
- a specific failure can be reproduced only in the preserved Windows environment.

## Commit and push policy

- Commit and push only when the user requests it or grants continuing authorization that clearly includes repository follow-through.
- Stage files explicitly. Do not use a command that stages the full worktree when unrelated changes are present.
- Before every commit, run:
  - `git diff --check`;
  - the checks appropriate to the change class below;
  - `git diff --cached --check`;
  - `git diff --cached --stat`;
  - `git diff --cached --name-only`.
- Keep commits narrow. Workflow, parser safety, RTC recovery, watchdog behavior, EEPROM layout, and RF behavior should not be mixed into one commit.
- Push the current development branch after a successful authorized commit, then verify that local `HEAD` and the remote-tracking ref match.
- Never force-push an integration or release branch as part of ordinary work.

## Verification by change class

### Documentation-only changes

- Resolve all local Markdown links.
- Run `git diff --check`.
- Run a focused secret scan when documents include command output, paths, logs, or configuration examples.
- A firmware build is optional.

### Workflow, ignore, or text-policy changes

- Show the exact affected-file set.
- Check for unexpected tracked files that would match new ignore patterns.
- Use `git check-attr` on representative source, project, CAD, and binary files after `.gitattributes` changes.
- Confirm that line-ending rules do not create mass diffs.
- A firmware build is optional when no build input changes.

### AVR firmware changes

- Use the repository AVR build wrapper once Step A2 supplies it.
- Build the Release configuration unless the task explicitly targets Debug behavior.
- Record compiler warnings, flash, SRAM, and EEPROM usage.
- Run all available host tests and subsystem checks related to the change.
- For timing, ISR, sleep/wake, EEPROM, RTC, antenna, power, or RF changes, complete the applicable hardware test before integration.

### ESP8266 firmware or web-asset changes

- Use the pinned ESP build wrapper once Step A2 supplies it.
- Run event-file and Linkbus compatibility tests.
- Record flash and LittleFS usage.
- Exercise the changed HTTP/WebSocket flow when user-visible behavior changes.

### Cross-processor protocol changes

- Update the protocol compatibility table and fixtures.
- Verify old valid messages remain valid unless a compatibility break is explicitly approved.
- Verify malformed or rejected input leaves the last valid active configuration intact.
- Test both processors or their approved simulators together before integration.

## Source and generated-file policy

- Prefer proven existing functions and shared helpers before adding parallel behavior.
- Preserve exact integer widths, signedness, boundary comparisons, and RF operation ordering during refactors.
- Treat `atmel_start*`, Microchip START configuration/modules/utilities, generated Makefiles, and IDE project metadata as generated or vendor-managed surfaces.
- `driver_isr.cpp` is generated-derived but contains product-specific communications behavior. Change it only in a focused commit and document the regeneration risk.
- Do not reformat generated or vendor-managed files as collateral work.
- Do not untrack historical build artifacts until the reproducible build baseline and artifact policy in Step A2 are complete.

See [GENERATED_FILES.md](GENERATED_FILES.md) for the detailed inventory.

## Release integration into main

Integration requires explicit user authorization.

1. Finish the applicable hardening or bug checkpoint on `Development_AVR128DA48`.
2. Verify both the development branch and `main` remote state.
3. Require a clean worktree.
4. Prefer a fast-forward integration when `main` is exactly behind the development branch.
5. If histories diverge, stop and review the commits and merge strategy before merging.
6. Push `main` only after the integration result is verified.
7. Fast-forward the development branch through the resulting `main` commit when necessary, return the checkout to `Development_AVR128DA48`, and verify it tracks its remote.

The one-time legacy-to-current transition is complete. Its preservation tag, exact parents, tree hash, and verification are recorded in [RELEASE_WORKFLOW.md](RELEASE_WORKFLOW.md).

## Work handoff

Every handoff should state:

- current branch and remote alignment;
- commits created and pushed;
- files intentionally left modified or untracked;
- checks run and checks unavailable;
- hardware evidence collected or still required;
- the next roadmap checkpoint or bug stage.
