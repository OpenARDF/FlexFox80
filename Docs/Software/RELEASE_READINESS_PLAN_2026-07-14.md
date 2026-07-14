# FlexFox80 Release Readiness Plan

**Decision date:** 2026-07-14

**Development branch:** `Development_AVR128DA48`

**Release branch:** `main`

**Current status:** R4 completed its full target gate on 2026-07-14; firmware behavior is frozen and release-candidate qualification is next

## Purpose

FlexFox80 has a long record of reliable field operation. The July 2026 work has corrected several bounded defects and added reproducible builds, tests, and target evidence. Time available before the next release does not justify expanding the firmware change surface merely to close every open review item.

R4 is complete. The immediate objective is therefore to preserve the behavior freeze, qualify the accumulated release candidate, and leave the repository in a documented state that can be resumed safely in a few weeks.

## Firmware cutoff

R4, ESP clone event-file checksum validation, was the only planned firmware implementation remaining before the release freeze. Its host, deterministic-build, installed, normal-clone, controlled rejection, retention, and restoration gates now pass. The release freeze is active.

Until the release candidate is qualified:

- permit an additional firmware change only to correct a newly reproduced release-blocking regression;
- do not begin R2, R3, R5, the remaining R6 work, or an R9 behavior change;
- do not resume deferred clock drift, RTC aging, clone-retry, or broader timing investigations;
- do not combine cleanup, refactoring, dependency upgrades, or branch restructuring with release preparation;
- retain the qualified Arduino ESP8266 core and library versions and the pinned AVR toolchain.

This cutoff does not declare the deferred findings unimportant or resolved. It records that their change risk and qualification cost exceed the benefit of including them in this release.

## R4 completion gate

R4 completed all required gates on 2026-07-14:

1. [x] The exact production candidate and build profile are identified by source commit and SHA-256 hash.
2. [x] Embedded release identities are assigned as AVR `0.201` and ESP `2.1`, with a source contract for `SW_VERSIONS,2.1,0.201`.
3. [x] A normal master-to-target clone succeeds with legacy event-file compatibility intact.
4. [x] A clone with a deliberately mismatched transferred checksum is rejected.
5. [x] The target retains its prior valid event after the rejected transfer.
6. [x] Normal cleanup restores reporting and leaves both devices responsive.
7. [x] Temporary fault-injection firmware and configuration were removed, and both test units were restored to documented operating configurations.
8. [x] The focused host tests, deterministic ESP builds, installed smoke checks, and [R4 evidence](Evidence/ESP_EVENT_FILE_INTEGRITY_2026-07-13.md) pass.

The temporary fault-injection build was test equipment, not a production change. The separately tracked intermittent normal-clone retry behavior remains deferred as `B-CLONE-02`.

## Work allowed without additional device intervention

The following low-risk work may proceed without putting another ESP into bootloader mode:

- run `just check` and the focused host regressions;
- build AVR and ESP release candidates twice and compare artifacts;
- record warnings, resource usage, source revisions, build profiles, and SHA-256 hashes;
- review the complete source and protocol diff against `main`;
- prepare release notes, artifact manifests, flashing instructions, and rollback instructions;
- verify documentation links, repository policy, ignored build outputs, and clean branch state;
- assemble the versioned hardware checklist and mark any approved deferrals explicitly;
- document the exact resume point for deferred hardening and bug investigations.

These activities may discover a release blocker, but they must not be used as a reason to broaden firmware scope without a reproducible failure and an explicit decision.

## Release-candidate sequence

With R4 complete:

1. Freeze AVR and ESP source at named commits.
2. Run the full repository check and deterministic release builds.
3. Capture the release identity required by `RELEASE_WORKFLOW.md`, including both firmware hashes and the LittleFS hash.
4. Run the applicable two-unit clone, clock, event-mode, RF-cycle, sleep/wake, telemetry, reset, and restoration checklist on dummy-loaded hardware. Record any deliberately deferred test.
5. Confirm the release candidate preserves the 274-byte AVR EEPROM schema and the qualified ESP event-file compatibility contract.
6. Preserve known-good rollback images and programming instructions outside ordinary source commits, with committed checksums and provenance where appropriate.
7. Write user-readable release notes that separate corrections, observed verification, and deferred work.
8. Verify `Development_AVR128DA48` is clean, fully pushed, and reproducible.
9. Review integration into `main` under the fast-forward-first release workflow. Integration and release distribution require their own explicit approval.

The separately approved replacement-tree transition completed on 2026-07-14. It preserved the former `main` at annotated tag `legacy-main-before-avr128da48-2026-07-14`, made the current `main` tree exactly match the qualified Ver 2.1 development tree, and removed the redundant `AVR128DA48` branch. Future development remains on `Development_AVR128DA48`; approved releases go to `main`.

## Deferred resume point

When hardening resumes, begin from the live tracker in `HARDENING_AND_BUG_PLAN.md`. The principal open items are:

- R2: broader post-start RTC hardware-failure behavior;
- R3: controlled AVR watchdog and reset-cause recovery;
- R5: cross-processor numeric and transmit-interval validation;
- R6: remaining EEPROM guard, schema-version, and migration work;
- R9: policy and compatibility decision for the open AP and unrestricted `PASS` bridge;
- `B-TIME-01`: residual discrete tick-loss and broader clone evidence;
- `B-CLONE-02`: independent characterization of occasional clone stalls and retry behavior.

Before changing code, confirm the released branch and artifact hashes, reproduce the selected behavior, and resume with a focused red-green test. Do not reopen completed R1, R7, R8, or R10 through R15 without new contradictory evidence.
