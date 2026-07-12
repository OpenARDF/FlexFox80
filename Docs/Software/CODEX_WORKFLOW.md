# FlexFox80 Development Workflow

## Branch roles

- `AVR128DA48` is the current integration and release baseline for the AVR128DA48 hardware generation.
- `Development_AVR128DA48` is the active development branch. Routine hardening and bug work starts here and returns here after integration checks.
- `main` preserves the earlier product lineage until the AVR128DA48 hardware and software are ready for the separately approved overwrite integration described in [RELEASE_WORKFLOW.md](RELEASE_WORKFLOW.md).
- `ATMEGA328p` and any other legacy hardware branches are historical product lines. Do not merge them into current firmware work without a specific compatibility objective.

## Before editing

1. State the current branch in the first work update.
2. Run `git status --short --branch` and identify pre-existing changes.
3. Confirm that the task belongs on `Development_AVR128DA48` unless it is an explicitly authorized integration, release, or legacy-maintenance operation.
4. Read [GENERATED_FILES.md](GENERATED_FILES.md) before changing IDE-generated, vendor-managed, build-output, or CAD files.
5. Identify the behavior that must remain unchanged and the evidence required for the proposed change.

Pre-existing changes belong to the user. Do not alter, discard, stage, or publish them unless the user explicitly includes them in the task.

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

## Integration back to AVR128DA48

Integration requires explicit user authorization.

1. Finish the applicable hardening or bug checkpoint on `Development_AVR128DA48`.
2. Verify both the development branch and `AVR128DA48` remote state.
3. Require a clean worktree.
4. Prefer a fast-forward integration when `AVR128DA48` is exactly behind the development branch.
5. If histories diverge, stop and review the commits and merge strategy before merging.
6. Push `AVR128DA48` only after the integration result is verified.
7. Return the checkout to `Development_AVR128DA48` and verify it tracks its remote.

Do not integrate current work into `main` through this routine. The legacy-to-current transition has separate preservation and verification gates.

## Work handoff

Every handoff should state:

- current branch and remote alignment;
- commits created and pushed;
- files intentionally left modified or untracked;
- checks run and checks unavailable;
- hardware evidence collected or still required;
- the next roadmap checkpoint or bug stage.
