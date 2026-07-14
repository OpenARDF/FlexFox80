# FlexFox80 Software Documentation

This directory documents the current software architecture on the `AVR128DA48` branch and records the initial reliability review performed in July 2026. It is intended to give future maintainers and project threads a shared starting point before changing mature transmitter behavior.

## Documents

- [Architecture](ARCHITECTURE.md) describes the ESP8266/AVR128DA48 division of responsibility, major subsystems, runtime flow, persistence, communications, and build layout.
- [Reliability review](RELIABILITY_REVIEW.md) records existing strengths, confirmed code-level concerns, suspected failure modes, and current verification gaps.
- [Safe hardening strategy](SAFE_HARDENING_STRATEGY.md) defines a conservative process for reproducing and fixing bugs without unnecessarily changing proven transmitter behavior.
- [Hardening and bug plan](HARDENING_AND_BUG_PLAN.md) turns the review into two coordinated work paths with specific deliverables, decision points, and exit checkpoints.
- [Development workflow](CODEX_WORKFLOW.md) defines everyday branch, verification, staging, commit, integration, and handoff rules.
- [Release workflow](RELEASE_WORKFLOW.md) defines current branch roles, integration gates, release evidence, and the future AVR128DA48 transition to `main`.
- [July 2026 release-readiness plan](RELEASE_READINESS_PLAN_2026-07-14.md) records the decision to complete R4, freeze firmware scope, qualify an official release candidate, and defer the remaining review items with a precise resume point.
- [July 2026 release-candidate snapshot](RELEASE_CANDIDATE_SNAPSHOT_2026-07-14.md) records the frozen firmware source, exact artifact hashes, completed verification, branch alignment, rollback position, and still-open official-release gates.
- [July 2026 release hardware checklist](RELEASE_HARDWARE_CHECKLIST_2026-07-14.md) maps the A8 matrix to passed, partial, pending, and proposed-defer evidence and isolates the remaining owner decisions.
- [Firmware release-notes draft](RELEASE_NOTES_DRAFT_2026-07-14.md) separates user-visible improvements, internal reliability corrections, compatibility boundaries, verification, and explicitly deferred work.
- [Source and generated-file boundaries](GENERATED_FILES.md) classifies product source, generated-derived files, IDE/build output, ESP assets, and CAD/manufacturing files.
- [Build environment](BUILD_ENVIRONMENT.md) defines Mac and Windows roles, the portable AVR Release wrapper, evidence classes, and the reference-environment handoff checklist.
- [WiFi-to-AVR access](WIFI_AVR_ACCESS.md) documents the supported HTTP/WebSocket/Linkbus path, safe probes, and the feasibility, reset-power constraint, recovery design, optional hardware changes, and decision gates for future AVR bootloading over WiFi.
- [Mac build environment evidence](Evidence/MAC_BUILD_ENVIRONMENT_2026-07-11.md) records the verified archived compiler and the remaining historical device-pack requirement.
- [Wireless clone time synchronization investigation](Evidence/WIRELESS_TIME_SYNC_INVESTIGATION_2026-07-12.md) records the rare multi-unit timing symptom, confirmed false-success paths, read-only phase observer, ranked hypotheses, and measurement plan.
- The repository-root [Codex mailbox](../../CODEX_MAILBOX.md) carries committed Mac/Windows requests and replies; durable results belong under `Evidence/`.

## Scope and status

These documents describe the source present on the `AVR128DA48` branch at commit `33e64e0` (`Bug Fixes`, 2024-05-06). They are an engineering review, not a statement that every identified risk has been observed on deployed hardware.

Items in the reliability review use the following terms:

- **Confirmed code issue** means the problematic behavior is directly visible in the implementation.
- **Failure candidate** means the code supports a plausible failure mechanism that still needs a focused reproduction.
- **Verification gap** means the repository does not presently provide a repeatable way to prove behavior.

No broad refactor should be started solely from this review. Begin with an observed symptom or a focused reproduction, add a behavioral test where practical, and make the smallest change that addresses the demonstrated problem.
