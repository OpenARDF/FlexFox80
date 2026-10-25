# FlexFox80 Development Workflow

## Branch roles

- `main` is always the deployed, release-ready branch and the default GitHub branch for the current Ver 2.1 product line.
- `Development1` collects firmware, software, tests, documentation, and release-workflow changes that have passed their applicable gates.
- `Development1-KiCad` separately collects schematic, PCB, symbol, footprint, model, and simulation changes under `KiCad/`.
- `Development_AVR128DA48` was retired after its final tip was verified identical to `main`. Do not recreate it.
- The transitional `AVR128DA48` branch was removed after the approved 2026-07-14 replacement-tree merge. Do not recreate it as an integration layer.
- `ATMEGA328p` and any other legacy hardware branches are historical product lines. Do not merge them into current firmware work without a specific compatibility objective.

## Before editing

1. State the current branch in the first work update.
2. Run `git status --short --branch` and identify pre-existing changes.
3. Route the task to `Development1` or `Development1-KiCad`, then run `just branch-route-check <collector>`.
4. Read [GENERATED_FILES.md](GENERATED_FILES.md) before changing IDE-generated, vendor-managed, build-output, or CAD files.
5. Identify the behavior that must remain unchanged and the evidence required for the proposed change.

Pre-existing changes belong to the user. Do not alter, discard, stage, or publish them unless the user explicitly includes them in the task.

## Mac and Windows coordination

Use the repository-root [Codex mailbox](../../CODEX_MAILBOX.md) for cross-machine requests and replies. Mailbox, firmware, test, and release-evidence work uses `Development1`; KiCad-only work uses `Development1-KiCad`.

- Preserve unacknowledged messages from the other agent.
- Put durable results under `Docs/Software/Evidence/`; the mailbox is the coordination channel, not the only record.
- Transfer toolchains, device packs, firmware images, and other large binaries outside Git. Record filenames, byte sizes, and SHA-256 hashes so the transfer can be verified.
- Never resolve a mailbox push conflict with a force-push. Fetch and preserve both agents' messages.
- Polling the mailbox means fetching `origin` and reviewing new commits on `origin/Development1`; do not assume the local file is current.

## Topic branches and collector promotion

1. Fetch `origin`; require a clean worktree and a synchronized local collector.
2. Create a focused `codex/<topic>` branch from the appropriate collector. Do not start it from `main`, the other collector, or a stale topic.
3. Keep firmware/software and KiCad changes in separate topics. `just branch-route-check <collector>` must pass before work and before return.
4. Run focused checks plus `just check`; KiCad topics also run [the KiCad gate](KICAD_VALIDATION.md). Treat every unwaived warning as a failure.
5. Merge only the proven topic into its collector with a merge-commit pull request, then return the checkout to that collector.
6. Use a separate cumulative promotion pull request from each collector to `main`, merged by squash or rebase so `main` remains linear. Do not mix the collectors' pending contents.
7. After any promotion reaches `main`, fetch `origin`, create a focused synchronization topic from each exact remote collector tip, and merge exact `origin/main` into it with `--no-ff`. Run `just collector-sync-check <collector>`, then merge each synchronization pull request with a merge commit. Do not rebase, squash, reset, or force-push collectors. Finish both synchronizations before starting another topic.

`just collector-preflight <collector>` is the required local promotion gate. The firmware/software collector rejects `KiCad/` paths. The KiCad collector rejects firmware, tests, release notes, and release evidence, and runs every currently available KiCad/model/simulation check.

`main` accepts squash or rebase pull requests and keeps a linear release history. Collectors accept merge commits only so topic ancestry and the exact `main` baseline remain auditable rather than being reproduced as equivalent file content under unrelated commits.

Post-publication evidence is the only exception to the ordinary cumulative promotion sequence. Evidence that can exist only after publication may use a later evidence-only promotion PR from `Development1`; it must not alter firmware, KiCad content, the tag, or the already published artifacts.

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

1. Finish the applicable checkpoint on the correctly routed collector.
2. Verify that collector, the other collector, and `main` remote state.
3. Require a clean worktree.
4. Run the collector preflight and review the complete cumulative diff against `main`.
5. Use a separate promotion pull request for that collector, merged by squash or rebase. If histories diverge, stop and review before any merge.
6. Do not merge the pull request or push `main` without separate explicit authorization.
7. After the separately authorized promotion, synchronize both collectors through exact two-parent merge topics validated by `just collector-sync-check <collector>`, return to the appropriate collector, and verify remote alignment.

The one-time legacy-to-current transition is complete. Its preservation tag, exact parents, tree hash, and verification are recorded in [RELEASE_WORKFLOW.md](RELEASE_WORKFLOW.md).

## Work handoff

Every handoff should state:

- current branch and remote alignment;
- commits created and pushed;
- files intentionally left modified or untracked;
- checks run and checks unavailable;
- hardware evidence collected or still required;
- the next roadmap checkpoint or bug stage.
