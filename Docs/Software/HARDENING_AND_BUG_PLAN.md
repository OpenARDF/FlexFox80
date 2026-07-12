# FlexFox80 Hardening and Bug Plan

**Plan status:** Step A1 is complete on `Development_AVR128DA48`; the exact AVR build is reproducible on Mac and Windows while ESP8266 pinning remains open in Step A2; characterization-first TDD and narrow firmware hardening slices are active.

## Purpose

FlexFox80 development will proceed along two coordinated paths:

- **Path A: Code hardening and clear-error removal.** Establish safe engineering controls, characterize current behavior, remove demonstrable defects, and improve recovery without changing proven transmitter behavior unnecessarily.
- **Path B: Specific bug investigation.** Capture observed failures, reproduce them, trace them to a subsystem, and fix them with focused regression evidence.

Path A starts first because repository controls, reproducible builds, and characterization tests reduce the risk of every later change. At each Path A checkpoint, the open bug list will be reassessed: hardening work may expose or eliminate a root cause, improve diagnostics, or make a specific bug reproducible.

This is the operational roadmap. [SAFE_HARDENING_STRATEGY.md](SAFE_HARDENING_STRATEGY.md) contains the supporting engineering principles and hardware fault matrix, while [RELIABILITY_REVIEW.md](RELIABILITY_REVIEW.md) contains the initial findings.

## Non-negotiable safety rules

1. The current Git branch and worktree state must be announced before edits.
2. Pre-existing changes are never included in a commit without explicit authorization.
3. No behavior change is accepted without a stated expected behavior and proportionate verification.
4. RF-off and antenna-inhibit behavior must never be weakened to simplify another change.
5. Protocol and persistence changes must define compatibility with the other processor and deployed data.
6. Timing-critical ISR, sleep/wake, RTC, EEPROM-layout, and RF-control changes require stronger evidence than ordinary parser or documentation changes.
7. Clear-looking defects are still fixed in narrow slices after characterization; unrelated cleanup is not bundled into a bug fix.
8. Generated or vendor-managed files are not reformatted or reorganized as part of normal source work.
9. A failed or unavailable verification step is reported; it is not silently treated as passed.
10. Every completed milestone ends with an updated evidence record and bug-list reassessment.

## Test-driven hardening policy

FlexFox80 was not originally developed with TDD, so the project will use **characterization-first TDD** for mature behavior and conventional red-green-refactor TDD for new behavior. The objective is regression control around the highest-risk seams, not a retrospective coverage percentage.

### Existing mature behavior

1. State the contract or observed behavior that must remain stable.
2. Add a characterization test, golden fixture, protocol trace, or repeatable hardware procedure that passes on the current implementation.
3. Include awkward or undesirable current behavior when changing it is not part of the approved slice; labeling a behavior does not silently approve changing it.
4. Refactor only after the relevant characterization remains green.

### Confirmed bugs and clear defects

1. Add the narrowest automated or repeatable test that reproduces the failure.
2. Run it against the pre-fix implementation and record that it fails for the expected reason: **red**.
3. Apply the smallest owning-subsystem change that makes it pass: **green**.
4. Re-run neighboring characterization, build, protocol, resource, and hardware gates.
5. Refactor only when useful and only while all applicable tests remain green.

The failing test and production fix normally belong to the same focused fix commit so the branch remains usable. Test-infrastructure changes that pass independently should be committed before the first production fix. A deliberately failing test may be preserved temporarily only on an explicitly identified diagnostic branch or in recorded pre-fix evidence; it must not leave `Development_AVR128DA48` red.

### New behavior

New testable behavior uses ordinary red-green-refactor TDD. Define the public contract and boundary cases in tests before adding the implementation. New hardware-facing behavior additionally requires a safe bench test or instrumentation plan before implementation.

### Test layers and limits

- **Host unit tests:** Pure calculations, parsers, buffers, state transitions, and serializers. These should be fast, deterministic, dependency-light, and runnable through `just test`.
- **Golden and protocol tests:** Checked-in event fixtures, Linkbus messages, malformed inputs, cross-processor compatibility, and timing traces.
- **Build tests:** Pinned AVR and ESP builds, warnings, resource usage, and artifact identity.
- **Hardware-in-the-loop tests:** RTC, I2C, EEPROM interruption, ISR timing, reset/watchdog, sleep/wake, antenna interlock, and RF behavior into a dummy load.

Host tests do not replace target verification. In particular, host `int`, pointer, alignment, floating-point, and timing behavior may differ from the AVR ABI. Firmware-facing tests must use fixed-width types where the contract depends on width, preserve explicit AVR boundary cases, and be corroborated by the pinned AVR build or target tests as appropriate.

Do not reorganize ISR, RF, persistence, or sleep/wake code merely to increase testability or coverage. Introduce the smallest seam needed for a demonstrated behavior, prefer proven existing helpers, and keep the production path shared between tests and firmware so a parallel test-only implementation cannot drift.

## Resolved provisional branch model

The present repository does not have the same branch model as SignalSlinger:

- `origin/main` is the default branch.
- active AVR128DA48 development is on `AVR128DA48`.
- `main` has two merge commits not present on `AVR128DA48`.
- `AVR128DA48` has substantial development history not reachable from the current `main` tip.
- the source layout differs between the two branches.

The project has adopted this provisional model:

- Leave `main` unchanged while its historical role and different source layout are audited and while the AVR128DA48 line is hardened.
- Treat `AVR128DA48` as the current integration/release baseline for this hardware generation.
- Use `Development_AVR128DA48`, created from `AVR128DA48`, as the active hardening and bug-development branch.
- Use narrow topic branches for safety-critical or independently reviewable slices when they materially improve isolation.
- Merge into the AVR128DA48 integration branch only after the applicable checkpoint passes; reconcile to `main` only through a separately planned and reviewed operation.

This recommendation protects the known AVR128DA48 lineage without claiming that the current `main` branch is obsolete or safe to overwrite.

## Path A: Code hardening and clear-error removal

### Step 1: Establish Git, source, and change-control workflows

**Goal:** Give every later change a predictable branch, review, build, test, and commit path comparable to SignalSlinger, adapted to FlexFox80's two-processor architecture.

**Deliverables:**

- A repository-internal workflow document defining:
  - branch roles and merge policy;
  - when commits and pushes require explicit authorization;
  - narrow staging requirements for dirty worktrees;
  - required review and verification before a commit;
  - release and rollback expectations;
  - rules for generated and vendor-managed files.
- A line-ending and text/binary policy using `.gitattributes` and `.editorconfig`.
- An expanded `.gitignore` covering IDE state, `.DS_Store`, objects, dependency files, generated firmware images, maps/listings, temporary test output, and local toolchains.
- A reviewed inventory dividing files into:
  - hand-maintained AVR source;
  - generated Microchip START/Atmel Studio source;
  - hand-maintained ESP source and web assets;
  - generated build artifacts;
  - hardware/CAD files outside software-change scope.
- A decision on currently tracked build artifacts. If they are removed from Git, do so in a dedicated source-hygiene commit that does not delete the developer's local copies or mix firmware changes.
- Thin repository commands, preferably a `Justfile`, for repeatable operations such as:
  - `status` or `doctor`;
  - documentation checks;
  - AVR build;
  - ESP build;
  - host tests;
  - full verification;
  - secret scan;
  - staged-diff verification.
- A commit policy that does not automatically stage the whole worktree. A convenience publish command may be added later, but selective staging remains mandatory when unrelated work exists.

**Proposed Step 1 file set:**

- `Docs/Software/CODEX_WORKFLOW.md` — everyday branch announcement, edit, verification, staging, commit, and generated-file rules.
- `Docs/Software/RELEASE_WORKFLOW.md` — branch roles, release checklist, artifact identity, hardware verification, rollback, and merge policy.
- `Docs/Software/GENERATED_FILES.md` — hand-maintained, generated, vendor-managed, release-artifact, and CAD boundaries.
- `.gitattributes` — authoritative line endings and text/binary classification.
- `.editorconfig` — editor-independent indentation, encoding, newline, and whitespace rules.
- `.gitignore` — comprehensive local/generated-file exclusions.
- `Justfile` — thin entry points for checks and scripts, without embedding complex build logic.
- `scripts/` — portable or clearly platform-specific build/verification helpers invoked by the `Justfile`.

The root `README.md` remains user-facing and is not the location for these development policies.

**Checkpoint A1 — Governance ready:**

- [x] Branch roles are explicitly approved and documented.
- [x] The workflow states which branch is safe for active hardening.
- [x] Existing KiCad or other unrelated changes can remain present without entering software commits.
- [x] Line-ending rules were checked across the tracked tree; only `linkbus.cpp` required a dedicated behavior-neutral normalization.
- [x] Generated-file cleanup is explicitly deferred until Step A2; the current policy reports the 60 tracked generated/IDE artifacts without removing them.
- [x] Routine Step A1 commands have names and expected outcomes; AVR and ESP builds fail clearly as deferred rather than claiming verification.
- [x] Another thread can read the workflow and correctly explain how to start, verify, commit, and hand off a change.

**Step A1 evidence:**

- `Development_AVR128DA48` was created from and initially matched `AVR128DA48`, then published with upstream tracking.
- Branch, release, generated-file, staging, and handoff policies are documented under `Docs/Software/`.
- `.gitattributes`, `.editorconfig`, and selective `.gitignore` rules protect source and manufacturing assets without importing unsafe global `*.lib` or `*.zip` rules.
- `just check` runs the repository doctor, Markdown link validation, ignore/attribute safeguards, KiCad JSON parsing, and diff hygiene.
- `just secrets` completes successfully on the current checkout.
- Firmware builds and generated-artifact cleanup remain Step A2 work.

**Stop condition:** No firmware edits begin until the branch-role decision and narrow-commit policy are settled.

### Step 2: Make both firmware builds reproducible

**Goal:** Build the AVR and ESP8266 from a clean checkout with recorded toolchain versions, without relying on undocumented IDE state.

**Deliverables:**

- A scripted AVR Release build using a pinned AVR-GCC/device-pack combination.
- A scripted ESP8266 build using pinned board-core and library versions through `arduino-cli` or PlatformIO.
- Documented flashing outputs and commands for each processor.
- Baseline build evidence:
  - compiler and dependency versions;
  - warnings;
  - AVR flash, SRAM, and EEPROM usage;
  - ESP flash and filesystem usage;
  - SHA-256 hashes of known baseline outputs.
- A method to compare a no-behavior-change build with the known deployed artifact when exact reproduction is possible.
- CI or a documented reason why a particular hardware-dependent verification remains local.

**Step A2 progress:**

- [x] The repository now has a platform-neutral AVR Release wrapper derived from the checked-in Microchip Studio configuration. It requires AVR-GCC 7.3.0 and `AVR-Dx_DFP` 1.9.103 for reference-build status and labels an explicitly allowed version mismatch as exploratory.
- [x] macOS is defined as the primary development environment and the preserved Windows Microchip Studio 7 installation as the release-equivalence and programming reference.
- [x] The exact Windows evidence request is documented, including deterministic-build, resource, hash, programmer, and ESP dependency information.
- [x] Two exact Windows AVR Release wrapper runs at `4d17bab` were deterministic across all requested artifacts and established the initial size/hash baseline. They also exposed three repeatable `g_fox` declaration bounds warnings for test-first investigation.
- [x] The Mac can identify and read the attached AVR128DA48 through Atmel-ICE without writing it. The connected unit's flash verifies exactly against the tracked `33e64e0` Debug HEX, and repeatable EEPROM/fuse captures now provide deployed-state evidence for R6.
- [x] The connected test unit has been programmed with the exact `57d70a7` Release image. Independent readback matches the intended flash, the complete original EEPROM was restored byte-for-byte, and all fuse bytes remain unchanged.
- [x] The supported WiFi-to-AVR path is traced from ESP soft AP and HTTP/WebSocket commands through Linkbus and AVR replies; a read-only Mac probe is available through `just wifi-probe`.
- [x] Run the AVR wrapper with exact-version Mac and Windows inputs. The accumulated `3bc10a5` snapshot builds deterministically and warning-free on both hosts; HEX, EEPROM, resource totals, and the 274-byte layout match.
- [x] The Release/Debug persisted-enum ABI mismatch is removed: explicit `uint16_t` storage, a source-layout regression, an AVR compile-time assertion, and an exact Mac linker map all preserve the deployed 274-byte EEPROM schema.
- [ ] Pin the ESP8266 build only after retrieving the known-good Arduino core, board-option, WebSockets, and filesystem-tool versions.

**Checkpoint A2 — Build baseline ready:**

- [ ] A clean checkout can build both processors from documented commands.
- [ ] Repeated builds are deterministic or all known nondeterminism is explained.
- [ ] Warnings are captured and triaged rather than hidden.
- [ ] Baseline resource and artifact hashes are recorded.
- [ ] Build outputs are not accidentally committed.
- [ ] The currently deployed firmware identity can be related to source, or the gap is explicitly documented.

**Bug reassessment:** Compare reported bugs with compiler warnings, stale artifacts, version mismatches, and processor-to-processor firmware compatibility.

### Step 3: Establish characterization-first TDD infrastructure

**Goal:** Make test-first behavior the routine hardening workflow and capture current mature behavior before changing it.

**Deliverables:**

- [x] A dependency-light host test harness invoked by `just test`, with failures that identify the behavioral contract and input case.
- [x] An initial passing characterization slice for the AVR circular buffer covering empty/full state, FIFO order, wraparound, overwrite, and reset without changing firmware behavior.
- [x] A source-contract regression captures the AVR compiler's required `g_fox` extent after deterministic Windows builds exposed a declaration mismatch.
- [x] A red-green defect slice demonstrates and corrects documented LIFO `pop()` behavior, with deterministic exact Windows target-build verification.
- Separate red-green slices for allocation and zero-capacity handling if characterization demonstrates a mismatch.
- Host-side tests or testable adapters for:
  - Linkbus framing, parsing, and resynchronization;
  - event-file parsing and validation;
  - role and transmitter-slot assignment;
  - numeric field parsing and range handling;
  - event-cycle position calculations;
  - Morse pattern generation and duration;
  - power-table selection;
  - EEPROM layout offsets and access widths.
- Golden fixtures based on the checked-in classic, sprint, and foxoring event files.
- A protocol compatibility table mapping every ESP command to AVR handling and every AVR reply to ESP handling.
- A timeline trace format that records requested RF power, key state, pattern/ID state, countdown, status, error, and sleep request.

**Checkpoint A3 — Behavior captured:**

- [ ] Existing valid event fixtures pass on both sides of the protocol model.
- [ ] Boundary and malformed cases are represented even when the current result is undesirable.
- [ ] Known mature timing examples have golden traces.
- [ ] Test failures clearly distinguish an intended behavior change from accidental regression.
- [x] At least one defect has complete recorded red-green evidence without leaving the development branch in a failing state. The `g_fox` declaration correction removed three deterministic AVR warnings while producing byte-identical target artifacts.
- [ ] Host tests explicitly cover relevant AVR-width boundaries rather than assuming the host ABI matches the target.
- [ ] Tests run through one repository command.

**Bug reassessment:** Attempt to encode each reported bug as a failing test or trace. Bugs that cannot yet be represented receive an explicit hardware/logging requirement.

**Completed A3.1 defect slice — `g_fox` declaration extent:**

- The exact Windows baseline reproducibly emitted three array-bounds warnings for the blind-ARDF entry.
- A source-contract regression failed red against the four-element external declaration.
- The external declaration was corrected to cover all `EVENT_NUMBER_OF_EVENTS` entries without changing the array definition, enum, EEPROM schema, or runtime flow.
- Mac host and repository gates passed green.
- Two exact post-fix Windows AVR Release builds completed with zero warnings, unchanged size, deterministic artifacts, and hashes identical to the pre-fix baseline.
- Evidence: [EEPROM_G_FOX_DECLARATION_2026-07-11.md](Evidence/EEPROM_G_FOX_DECLARATION_2026-07-11.md) and [WINDOWS_G_FOX_VERIFICATION_2026-07-12.md](Evidence/WINDOWS_G_FOX_VERIFICATION_2026-07-12.md).

**Completed A3.2 defect slice — circular-buffer `pop()` ordering:**

- A focused host regression failed red because `pop()` read the next write position instead of the last occupied position.
- The minimal correction moves the existing head decrement before the read; FIFO, wraparound, overwrite, reset, busy-state, allocation, and public-interface behavior were not changed.
- Mac host tests passed with AddressSanitizer and UndefinedBehaviorSanitizer enabled, and the full repository check passed.
- Two exact Windows AVR Release builds completed with zero warnings, unchanged size, deterministic artifacts, and the expected executable-artifact differences from the prior baseline.
- The full Windows host suite passed with the documented LIFO regression and existing characterization tests.
- Evidence: [CIRCULAR_BUFFER_POP_2026-07-11.md](Evidence/CIRCULAR_BUFFER_POP_2026-07-11.md) and [WINDOWS_CIRCULAR_BUFFER_POP_VERIFICATION_2026-07-12.md](Evidence/WINDOWS_CIRCULAR_BUFFER_POP_VERIFICATION_2026-07-12.md).

**Completed defect slice — I2C failure-count EEPROM width:**

- The EEPROM field, global value, load path, and change comparison are all 16-bit.
- A source-contract regression failed red because ordinary saves wrote one byte and first-time initialization wrote four bytes.
- Both write paths now use the existing word writer without changing the deployed field type or offset.
- Mac host and repository gates pass green; the accumulated exact Windows build and host-contract run pass at `3bc10a5`.
- Evidence: [EEPROM_I2C_FAILURE_COUNT_WIDTH_2026-07-12.md](Evidence/EEPROM_I2C_FAILURE_COUNT_WIDTH_2026-07-12.md).

**Completed Mac defect slice — RF power initialization width:**

- The RF power field, global value, normal read path, and normal update path are 16-bit.
- A source-contract regression failed red because first-time initialization used the dword writer, spanning the field and two bytes of the following reserved guard.
- Commit `4dbd90f` changes only that initialization call to the word writer.
- The full Mac suite passes green; two exact builds are deterministic and warning-free, text decreases by four bytes, EEPROM initialization remains byte-identical, and the schema remains 274 bytes.
- Connected-target fault injection passed: invalid initialization plus `A5 5A C3 3C` in `Guard4_15` produced the 500 mW default while preserving all sentinel bytes.
- The exact latest flash, complete original EEPROM, and all fuses were independently restored and verified byte-for-byte.
- The accumulated exact Windows build and host-contract run pass at `3bc10a5`.
- Evidence: [EEPROM_RF_POWER_INITIALIZATION_WIDTH_2026-07-12.md](Evidence/EEPROM_RF_POWER_INITIALIZATION_WIDTH_2026-07-12.md).

**Completed Mac defect slice — bounded Linkbus and Serialbus text sends:**

- Both transmit helpers passed caller data directly as the `sprintf()` format string and did not enforce their fixed destination sizes.
- A source contract failed red against both unsafe calls before the production correction.
- One shared bounded helper now copies percent characters literally, preserves exact-fit NUL termination, and rejects oversized input without queuing a partial protocol frame.
- Direct host regressions cover percent sequences, the maximum accepted length, oversized rejection without destination mutation, and invalid arguments.
- The full Mac suite passes green; two exact builds are deterministic and warning-free, EEPROM output is unchanged, and text grows by 30 bytes.
- The dummy-loaded test unit now runs the exact corrected flash; independent flash, complete EEPROM, and fuse readbacks match the intended and preserved bytes.
- The post-program read-only WiFi probe and sustained heartbeat passed with live AVR temperature/battery, version, identity, master-state, and synchronization replies.
- The accumulated exact Windows build and host-contract run pass at `3bc10a5`.
- Evidence: [BOUNDED_TEXT_SEND_2026-07-12.md](Evidence/BOUNDED_TEXT_SEND_2026-07-12.md).

**Completed Mac defect slice — Linkbus receive field bounds:**

- The USART receive parser wrote field bytes without checking the 21-byte field width or three-field table extent.
- A source contract failed red because all three required append, termination, and next-field guards were absent.
- Inline guards now preserve the valid 20-byte payload and three-field limits while rejecting an oversized field or fourth field before any out-of-bounds write.
- Rejected frames remain unpublished and the existing start-marker logic resynchronizes at the next `$` or `!` frame.
- Direct host regressions cover the final valid byte and field, NUL reservation, both overflow boundaries, invalid indices, and zero capacity.
- The full Mac suite passes green; two exact builds are deterministic and warning-free, EEPROM output is unchanged, and text grows by 52 bytes.
- The dummy-loaded test unit now runs the exact corrected flash; independent flash, complete EEPROM, and fuse readbacks match the intended and preserved bytes.
- Controlled oversized-field and fourth-field target tests were rejected without acknowledgment; after the ESP's existing retry cycle, fresh read-only temperature queries proved next-frame resynchronization in both cases.
- Two exact Windows builds at `3bc10a5` are deterministic and warning-free; HEX, EEPROM, size, layout, and all requested host contracts match the Mac evidence.
- Evidence: [LINKBUS_RX_BOUNDS_2026-07-12.md](Evidence/LINKBUS_RX_BOUNDS_2026-07-12.md).

**Completed Mac defect slice — Linkbus message-ID length:**

- Linkbus IDs are defined as one to three characters, but the receive parser previously accumulated unlimited ID bytes and truncated the result to the 16-bit command enum.
- Concrete four-character aliases reach valid handlers: `AOSU` aliases `KEY`, `AWVZ` aliases `RST`, and read-only reproducer `AZRX` aliases `VER`.
- A source contract failed red because the parser had no ID-length guard.
- The parser now rejects a fourth ID byte before accumulation while preserving every valid one-to-three-character ID.
- Direct host coverage protects the boundary, and the constrained target test distinguishes rejection from the pre-fix immediate `VER` alias response.
- The full Mac suite passes green; two exact builds are deterministic and warning-free, EEPROM output is unchanged, text grows by 22 bytes, and BSS grows by one byte.
- The dummy-loaded test unit now runs the exact corrected flash; independent flash, complete EEPROM, and fuse readbacks match the intended and preserved bytes.
- The read-only `$AZRX?` target reproducer remains unanswered through the ESP retry cycle instead of aliasing `VER`; a following temperature query proves next-frame resynchronization.
- Exact Windows verification remains open.
- Evidence: [LINKBUS_RX_ID_LENGTH_2026-07-12.md](Evidence/LINKBUS_RX_ID_LENGTH_2026-07-12.md).

**R6 layout characterization checkpoint:**

- A host-side AVR-width model now checks all 65 `EE_prom` member offsets against `EE_var_t`.
- The current declaration is confirmed as a 274-byte layout, matching the checked-in reference map.
- The layout test also requires fixed-width storage and offset expressions for all six persisted enum-valued fields.
- The exact Release build enables short enums, but its linker map remains 274 bytes and the AVR compile fails if the structure drifts.
- The test protects deployed offsets; a real-device EEPROM image is now captured separately, while guard-content interpretation and validation remain open work.
- Evidence: [EEPROM_LAYOUT_CONTRACT_2026-07-12.md](Evidence/EEPROM_LAYOUT_CONTRACT_2026-07-12.md) and [EEPROM_ENUM_WIDTH_ABI_2026-07-12.md](Evidence/EEPROM_ENUM_WIDTH_ABI_2026-07-12.md).

**R6 deployed-image checkpoint:**

- Two read-only avrdude EEPROM captures from the connected `33e64e0` FlexFox are byte-identical.
- The live image confirms field-address access but contains unexplained late-layout values, including erased bytes in the unused I2C counter despite a current initialization flag.
- The observation does not yet prove whether initialization, preserved EEPROM history, or another write-path behavior produced those bytes; no device data was changed.
- Evidence: [MAC_ATMEL_ICE_TARGET_EVIDENCE_2026-07-12.md](Evidence/MAC_ATMEL_ICE_TARGET_EVIDENCE_2026-07-12.md).

**R6 first programming checkpoint:**

- A no-chip-erase replacement attempt failed verification on a required 0-to-1 flash transition and was not accepted as a valid image.
- Immediate recovery explicitly erased and verified the Release flash, restored and verified the complete pre-test EEPROM image, and left all fuses unchanged.
- Independent post-operation reads are byte-identical to the intended flash and preserved EEPROM/fuse baselines.
- Functional RF, RTC, communications, and timing qualification remain open.
- Evidence: [MAC_AVR_PROGRAMMING_2026-07-12.md](Evidence/MAC_AVR_PROGRAMMING_2026-07-12.md).

**WiFi hardware-observation checkpoint:**

- FlexFox has no supported wired serial console; runtime access is through the ESP8266 soft AP and its HTTP/WebSocket-to-Linkbus bridge.
- The built-in WebSocket connection requests live AVR temperature and battery values without changing configuration.
- A dependency-free Mac probe sends only identity queries and heartbeat traffic, then requires both ESP and live AVR replies.
- Raw `PASS`, RF keying, transmission, clock synchronization, configuration, event, and WiFi-shutdown commands are excluded from the initial probe.
- Live execution awaits Mac association with the FlexFox SSID while internet connectivity is provided separately or temporarily relinquished.
- Procedure: [WIFI_AVR_ACCESS.md](WIFI_AVR_ACCESS.md).

The first live routed probe is complete: the Mac retained its normal WiFi/default route, FlexFox-only traffic used the Moto/DroidTether `utun6` host route, and HTTP, WebSocket, ESP identity, and live AVR temperature/battery/version/time replies passed. Continuous `just wifi-monitor` mode supplies a five-second `!&` heartbeat, safely inside the ESP's approximately ten-second socket timeout, for longer sessions. Evidence: [MAC_WIFI_AVR_PROBE_2026-07-12.md](Evidence/MAC_WIFI_AVR_PROBE_2026-07-12.md).

**WiFi command-boundary hardening candidate:**

- The default soft AP is open, and any associated WebSocket client can submit `PASS,<text>` for unrestricted forwarding to the AVR Linkbus.
- The forwarded surface includes benign queries as well as EEPROM-affecting configuration, reset, immediate transmission, manual keying, and WiFi-shutdown commands.
- Existing `/test.html` and `/radio.html` behavior depends on raw pass-through, so removing it or requiring authentication is a compatibility and field-workflow decision rather than a safe opportunistic edit.
- Before changing it, characterize every legitimate direct command, define an allow-list and authorization model, preserve a recovery path, and verify that malformed or incomplete messages cannot leave RF keyed or communications disabled.
- The initial Mac probe is independently constrained by a source contract and does not use `PASS`.

### Step 4: Remove clear, locally bounded defects

**Goal:** Correct defects whose unsafe behavior is directly demonstrated, using one narrow change at a time.

**Proposed order:**

1. Bounds-check Linkbus fields and delimiters; discard malformed frames and resynchronize safely.
2. Replace data-as-format-string sends with bounded copies/formatting.
3. Correct role-index extraction and cover all valid role/slot forms.
4. Define and correct event-file integrity validation while retaining existing-file compatibility.
5. Validate timing and other numeric input before mutating active AVR state.
6. Verify and correct EEPROM access widths without moving deployed offsets.
7. Add allocation-failure handling where dynamic allocation remains necessary.

Each item is a separate implementation slice unless two changes cannot be tested independently.

**Checkpoint A4.n — Clear defect removed:**

- [ ] A pre-fix test demonstrates the defect and its expected red failure is recorded.
- [ ] The fix changes only the intended behavior.
- [ ] The focused regression is green after the fix.
- [ ] Existing valid protocol, event, timing, and RF-safety tests remain green.
- [ ] Resource usage remains within the recorded budget.
- [ ] Cross-processor compatibility is verified where applicable.
- [ ] A focused commit describes the failure mechanism and evidence.

**Bug reassessment:** After every slice, re-run all reproducible bug cases and record whether any symptom is fixed, narrowed, or unchanged.

### Step 5: Add fault recovery and diagnostic evidence

**Goal:** Prevent a single peripheral or timing failure from leaving the product permanently hung or silently unsafe.

**Deliverables:**

- A bounded RTC synchronization operation with an explicit degraded/error result.
- Defined behavior for RTC loss at boot and after boot.
- Reset-cause capture and reporting.
- A watchdog policy covering startup, normal foreground operation, long operations, EEPROM writes, and sleep/wake.
- A proven remote-reset command.
- Defined I2C bus recovery after timeout/NACK, including safe Si5351 behavior.
- Persistent or externally retrievable diagnostics sufficient to identify why a reset or degraded state occurred.

**Checkpoint A5 — Recovery proven:**

- [ ] Removing the RTC square wave cannot hang the foreground forever.
- [ ] A forced foreground stall produces a classified watchdog reset.
- [ ] Sleep does not cause unintended watchdog resets.
- [ ] Si5351/I2C failure leaves RF in the defined safe state.
- [ ] Reset cause and relevant error state are visible after recovery.
- [ ] Repeated fault injection does not corrupt EEPROM or event files.

**Bug reassessment:** Re-run bugs involving freezes, unexpected resets, missed schedules, and loss of communications with new diagnostics enabled.

### Step 6: Harden concurrency and timing-critical state

**Goal:** Remove torn reads and fragile cross-context invariants without disturbing proven transmit timing.

**Deliverables:**

- An ownership map for globals shared among foreground, RTC ISR, TCB0 ISR, and USART ISRs.
- Atomic snapshots or brief critical sections for multi-byte shared values.
- Measured TCB0 ISR normal and worst-case duration.
- Explicit state-transition invariants for event start, on/off boundaries, ID insertion, finish, and sleep/wake.
- Refactoring only where measurement and characterization prove equivalence or expose a specific defect.

**Checkpoint A6 — Timing integrity proven:**

- [ ] Multi-byte shared state is either single-owner or atomically transferred.
- [ ] ISR execution stays within a documented timing budget.
- [ ] Classic, sprint, foxoring, beacon, and ID timing traces remain correct.
- [ ] Start, finish, reset, and wake at boundary seconds have directed tests.
- [ ] On-target timing measurements agree with the model within the accepted tolerance.

**Bug reassessment:** Revisit intermittent timing, missed-transition, incorrect-slot, and wake/sleep bugs.

### Step 7: Improve persistence and long-running resilience

**Goal:** Ensure configuration survives interruption and extended operation without mixed or corrupt state.

**Deliverables:**

- A versioned AVR EEPROM schema and validation strategy.
- Compatibility or migration behavior for deployed EEPROM contents.
- Transactional or recoverable multi-field AVR configuration commits.
- Transactional LittleFS event saves using temporary file plus validated replacement where supported.
- Power-loss tests during both AVR and ESP persistence.
- ESP heap/fragmentation stress tests for extended editing and master/clone sessions.

**Checkpoint A7 — Persistence resilient:**

- [ ] Interruption produces either the prior valid configuration or the complete new one.
- [ ] Deployed EEPROM images load correctly or migrate predictably.
- [ ] Corruption is detected and produces a safe, visible recovery path.
- [ ] Extended ESP sessions remain within a defined heap budget.
- [ ] Event files remain compatible with supported firmware versions.

### Step 8: Hardware regression and hardening release gate

**Goal:** Prove the accumulated hardening on representative hardware before treating it as a release candidate.

**Deliverables:**

- A versioned hardware test checklist covering:
  - power-up and power-cycle recovery;
  - event transfer and persistence;
  - classic, sprint, foxoring, and beacon operation;
  - antenna removal and reconnection;
  - RTC, Si5351, ESP, and I2C fault injection;
  - sleep and scheduled wake;
  - temperature/fan behavior;
  - long-duration event operation;
  - all fixed specific bugs.
- Captured firmware hashes, board revision, test equipment, results, and approved skips.
- A rollback artifact and flashing procedure.

**Checkpoint A8 — Hardening release candidate:**

- [ ] All automated gates pass from a clean checkout.
- [ ] All safety-critical hardware tests pass.
- [ ] Every skipped test has an explicit reason and owner approval.
- [ ] Open bugs are classified by severity and release impact.
- [ ] Release notes distinguish robustness improvements from user-visible changes.
- [ ] Rollback has been verified, not merely documented.

## Path B: Specific bug investigation

Path B starts as soon as an observed bug is selected. It uses the infrastructure available at that point and may feed new requirements back into Path A. Urgent safety bugs can interrupt the Path A sequence, but they still require a narrow branch/commit and explicit verification.

### Bug intake record

Every bug should begin with:

- a short symptom statement;
- safety and operational impact;
- hardware revision and unit identity;
- AVR and ESP firmware versions or artifact hashes;
- event file and transmitter assignment;
- exact setup and steps;
- expected and observed behavior;
- frequency or reproducibility rate;
- power, antenna, temperature, and timing context;
- logs, serial traffic, LED pattern, reset cause, or RF trace;
- whether power cycling changes the result;
- last known version without the problem, if any.

### Bug investigation stages

#### B1: Classify and preserve evidence

Determine whether the symptom is primarily configuration, ESP/UI/filesystem, Linkbus, AVR scheduling, RTC/sleep, RF control, power, or hardware. Preserve the original event/configuration and avoid testing first with a modified copy.

**Checkpoint B1:** Another person can reproduce the setup or state exactly what evidence is missing.

#### B2: Reproduce at the narrowest layer

Attempt reproduction in this order where appropriate:

1. host-side pure logic or parser test;
2. recorded/replayed Linkbus traffic;
3. ESP plus simulated AVR, or AVR plus simulated ESP;
4. complete bench unit into a dummy load;
5. field-equivalent environment.

**Checkpoint B2:** The symptom is reproducible, or the investigation has a concrete instrumentation plan rather than an unsupported theory.

#### B3: Form and discriminate hypotheses

List plausible root causes and identify the observation that would distinguish each. Prefer instrumentation and controlled fault injection over speculative edits.

**Checkpoint B3:** One code or hardware mechanism explains all reproduced observations and competing explanations have been tested where practical.

#### B4: Add a failing regression

Encode the reproduction as an automated test, trace, protocol fixture, or repeatable hardware checklist before applying the fix.

**Checkpoint B4:** The regression fails for the expected reason on the pre-fix implementation.

#### B5: Apply the smallest fix

Change only the owning subsystem. Do not combine opportunistic cleanup. If the root cause is a Path A hardening item, reference that milestone and keep one source of truth for verification.

**Checkpoint B5:** The regression passes, neighboring behavior remains unchanged, and required hardware evidence is complete.

#### B6: Close and cross-check

Document the root cause, affected versions, correction, test evidence, residual risk, and whether the finding changes any Path A priority.

**Checkpoint B6:** The bug can be closed without relying on undocumented knowledge from the investigating thread.

## Coordination between the paths

Use a small internal issue table or tracker with these fields:

| ID | Path | Severity | Owner subsystem | Reproduction | Evidence | Blocked by | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| R1 | A | High | AVR Linkbus | Direct field-boundary regression; exact Mac build | Static review and focused red-green evidence | A3/A4 | In progress |
| R2 | A/B | High | AVR RTC | Hardware fault test needed | Static review | A2/A5 | Planned |
| R3 | A | High | AVR recovery | Directly visible | Static review | A5 | Planned |
| R4 | A | Medium | ESP event files | Pending fixture | Static review | A3 | Planned |
| R5 | A/B | High | Cross-processor timing input | Pending fixture | Static review | A3 | Planned |
| R6 | A | High | AVR EEPROM | I2C counter and enum ABI regressions; 65-field layout protected | Static review, focused red-green evidence, and exact Mac build | A3/A4 | In progress |
| R7 | A | High | AVR text output | Direct boundary regression; exact Mac build | Static review and focused red-green evidence | A3/A4 | In progress |
| R8 | A/B | Medium | ESP role assignment | Pending Event test | Static review | A3 | Planned |
| R9 | A/B | High | ESP WebSocket/AVR bridge | Open AP plus unrestricted `PASS` forwarding | Static end-to-end trace; safe probe contract | A3/A4 | Characterized |

Specific field bugs should be added with distinct `B-` identifiers. At each Path A checkpoint:

1. run every available bug reproduction;
2. update evidence and likely subsystem;
3. identify bugs fixed incidentally by an approved hardening change;
4. keep symptom-specific tests even when a shared root cause is found;
5. reprioritize the next hardening slice based on safety and evidence.

## Initial execution order

The recommended order is:

1. **A1 — Git and workflow policy.** Confirm branch roles and establish safe repository controls.
2. **A2 — Reproducible AVR and ESP builds.** Know exactly what is being tested and deployed. A3 host-harness work that does not change firmware may proceed in parallel.
3. **A3 — Characterization-first TDD.** Establish `just test`, capture protocol, event, timing, and persistence behavior, and prove the first red-green defect slice.
4. **A4.1 and A4.2 — Linkbus bounds and bounded output.** Address the clearest memory-safety risks first.
5. **A4.3 through A4.6 — Remaining bounded defects.** Role parsing, event integrity, numeric validation, and EEPROM widths.
6. **A5 — RTC/watchdog/fault recovery.** Proceed only after builds and core tests are dependable.
7. **A6 and A7 — Concurrency, timing, and persistence.** Use accumulated evidence to minimize architectural churn.
8. **A8 — Full hardware regression.** Produce a hardening release candidate.

Path B bug intake can begin immediately, but implementation should normally wait for the earliest Path A checkpoint that supplies the required build or regression evidence.

## Next action

Continue characterization-first TDD one bounded defect at a time. The next slice should establish a failing regression before changing production code, preserve deployed storage and protocol compatibility, pass `just check`, and receive exact Windows AVR build verification whenever executable firmware changes. Keep allocation/zero-capacity buffer behavior separate from EEPROM-width work; do not combine opportunistic fixes.
