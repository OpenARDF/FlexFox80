# ESP clone corrective diagnostic build — 2026-09-02

## Scope

This is an ESP-only corrective diagnostic candidate. It identifies itself as ESP `2.30` with AVR `0.210` unchanged. It does not change the AVR firmware, EEPROM layout, event-file format, clone protocol, or LittleFS contents installed on a field unit.

Install only `Software/Huzzah/tmp/esp-build/ARDF_Transmitter.ino.bin`. Do not install the generated LittleFS image.

## Hardware evidence from ESP 2.29

The diagnostic build was installed on one master and five named targets and exercised for two clone attempts per target.

| Target | Successful | Failed | Failure evidence |
| --- | ---: | ---: | --- |
| Alpha | 1 | 1 | Target reset or stalled while validating file 7; six files had validated. |
| Bravo | 2 | 0 | Both attempts completed. |
| Charlie | 1 | 1 | Controlled clock write/readback failure caused by a one-second epoch rollover. |
| Delta | 1 | 1 | Target reset or stalled while validating file 4; three files had validated. |
| Echo | 2 | 0 | Both attempts completed. |
| **Total** | **7** | **3** | Two target resets/stalls and one clock rollover rejection. |

During each severe file failure, the target disappeared before emitting a diagnostic failure or cleanup record. It later advertised using the ESP SDK fallback network state while its HTTP service remained unavailable. The master retained clone ownership indefinitely because target progress was not bounded. The target AVR also remained in clone quiet mode, suppressing the time report that the rebooted ESP required to finish startup.

## Source diagnosis and corrective changes

The strongest source-level cause of the file-number-dependent resets was the ESP `CircularStringBuff` ownership behavior. Each received file line was copied into and out of a 202-entry `String` queue. A consumed slot retained its allocation until the queue was destroyed, and the queue destructor did not delete its backing `String[]` array. Across several event files this retained and fragmented scarce ESP8266 heap, matching the observed resets during validation of later files.

ESP 2.30:

- moves each line into and out of the queue so a consumed slot releases ownership immediately;
- clears all owned strings on reset and deletes the backing array on destruction;
- bounds clone-file validation to five seconds and reports the exact validation/install subphase;
- accepts an RTC readback at either the requested epoch or the immediately following second;
- releases a master clone session after 15 seconds without target progress, resumes the master AVR, and disconnects the stale socket;
- keeps the master WebSocket listener available for the target's retry;
- retains the target's active clone phase and file position across an ESP reset, queues AVR clone-resume before waiting for the first time report, and exposes the reset reason in `/firmware/status`;
- freezes terminal diagnostic elapsed times and initializes the immutable target identity before any diagnostic record can be emitted.

These defenses address both the primary heap fault and the persistent states observed after a target reset. Hardware testing is still required to establish that the source diagnosis is sufficient under repeated real clone traffic.

## Verification

- `just test`: passed, including AddressSanitizer coverage of production ESP queue ownership/destruction, diagnostic terminal timing, clock rollover, retained reset-marker integrity, and master watchdog wraparound.
- `shellcheck scripts/run-host-tests.sh`: passed.
- `shfmt -d scripts/run-host-tests.sh`: passed with no differences.
- `just check`: passed all repository, documentation, host-test, and firmware-contract gates.
- `just esp-build`: passed using the pinned HUZZAH configuration and ESP8266 toolchain.
- Sketch usage: 572,156 bytes (54% of flash program space).
- Global data usage: 53,876 bytes (65% of RAM), leaving 28,044 bytes for local variables and heap.

## Installable artifact

- Path: `Software/Huzzah/tmp/esp-build/ARDF_Transmitter.ino.bin`
- Size: 576,304 bytes
- SHA-256: `b95d2d74c8a1ab0ac098c6181d163b10e9d52063f510505c8f2a8251e8f259ce`
- MD5: `d328f5489029f3e17859d8d48700b650`

The build also generated `ARDF_Transmitter.littlefs.bin` as part of the normal wrapper, but that file is outside this ESP-sketch-only install scope.

## Hardware installation evidence

The identical ESP 2.30 sketch was installed through the guarded Moto ADB relay on the master and all five test targets. Every updater run verified the rebooted sketch MD5, mounted/protected LittleFS, and retained AVR 0.210. A separate post-install WiFi/WebSocket probe then verified the advertised identity, master role, and bidirectional ESP–AVR temperature and battery telemetry.

| Name | Immutable identity | Role | Post-install result |
| --- | --- | --- | --- |
| Master | `Tx_930F093E` (advertises `Tx_Master`) | Master | ESP 2.30, AVR 0.210, telemetry passed |
| Alpha | `Tx_C22DD73C` | Target | ESP 2.30, AVR 0.210, telemetry passed |
| Bravo | `Tx_4D47BBC0` | Target | ESP 2.30, AVR 0.210, telemetry passed |
| Charlie | `Tx_4D481FF8` | Target | ESP 2.30, AVR 0.210, telemetry passed |
| Delta | `Tx_7C2CB9FD` | Target | ESP 2.30, AVR 0.210, telemetry passed |
| Echo | `Tx_7C2D69ED` | Target | ESP 2.30, AVR 0.210, telemetry passed |

All six units reported the same installed sketch size (576,304 bytes) and normalized ESP8266 installed-sketch MD5 (`26030e2e0f3627d8b2399dad29e013b1`). No AVR firmware or LittleFS image was installed.

## ESP 2.30 corrective trial evidence

Alpha was synchronized twice against the ESP 2.30 master:

- Trial 1 completed all 9 files in 33,468 ms, confirmed AVR cleanup, reported no receive overflow or reset, and had an exact clock readback.
- Trial 2 reached `file-install-rename` for file 7 after receiving 53 lines. No further target progress arrived. The master watchdog released clone ownership and resumed the master AVR after 15 seconds, proving that the former permanent master lockup was corrected.
- After the failure, Alpha advertised its retained SSID on the ESP SDK fallback `192.168.4.x` subnet, did not serve HTTP, and showed both ESP LEDs continuously. This is consistent with an ESP reset before normal network initialization completed.
- With the master off, an isolated hard power cycle restored Alpha. ESP 2.30 remained installed, LittleFS mounted without recovery mode, and bidirectional ESP–AVR telemetry passed.

The failure phase showed that retaining the empty 202-entry receive queue and repeatedly constructing multi-allocation diagnostic strings still left the target vulnerable at the validation/install boundary. Testing was paused rather than continuing the fleet matrix.

## ESP 2.31 follow-up candidate

ESP 2.31 keeps the 2.30 queue ownership, clock, reset, and master-watchdog corrections and additionally:

- destroys the per-file receive queue immediately after `EOF`, before file validation or rename;
- constructs each `CDIAG` record with one reserved `String` allocation instead of a chained series of temporary allocations; and
- records current/minimum free heap, largest free block, and heap fragmentation in diagnostic records and `/firmware/status`.

Verification passed with `just test`, `just check`, `git diff --check`, and the pinned `just esp-build` compiler gate. The ESP 2.31 sketch uses 574,696 bytes (55% of program storage) and 54,048 bytes (65% of statically allocated RAM), leaving 27,872 bytes for stack and heap.

ESP 2.31 installable sketch:

- Path: `Software/Huzzah/tmp/esp-build/ARDF_Transmitter.ino.bin`
- Size: 578,848 bytes
- SHA-256: `f00cf308323824e460245a3257b55ff3f97d382c67e7a5fa82d7be62947db7d0`
- MD5: `9757c2ce331fb9bca1f2c4467bfe6d28`

This remains an ESP-sketch-only candidate. The generated LittleFS image and AVR firmware are outside the installation scope.

## ESP 2.31 Alpha hardware trials

The master and Alpha were installed and independently verified on ESP 2.31. Both reported the normalized installed-sketch MD5 `5733375b91e3af428e86bbbacdf71cdd`, mounted and protected LittleFS, and live AVR 0.210 telemetry.

- Trial 1 completed all 9 files in 33,903 ms with exact clock readback, confirmed cleanup, no receive overflow, no recovered reset, and a 14,272-byte minimum free heap.
- Trial 2 completed all 9 files in 35,451 ms with an accepted one-second clock readback delta, confirmed cleanup, no receive overflow, no recovered reset, and a 14,576-byte minimum free heap.
- Trial 3 stopped after reporting `file-install-rename` for file 7. At that point it still had 21,976 free heap bytes, a 17,936-byte minimum, a 20,192-byte largest free block, 8 percent heap fragmentation, no receive overflow, and no reset evidence in the last transmitted record. The master recovered after its 15-second no-progress watchdog.
- During the failure Alpha continued answering ICMP at the ESP AP/STA default address `192.168.4.1`, but neither HTTP port 80 nor WebSocket port 81 was listening. Both ESP LEDs remained continuously illuminated and the PCB red LED blinked slowly.
- After an isolated hard power cycle, Alpha returned on its normal `73.73.73.73` address with ESP 2.31, mounted/protected LittleFS, and live AVR telemetry. The filesystem contained a complete 1.19 KB `Temp` file but no `Classic80m-Set3-1.event` file. This proves the preceding removal completed and the following rename did not complete.

The 2.31 trial disproves the working heap-pressure hypothesis: the failure repeated with substantially more contiguous heap than the successful trials. It also corrects the earlier fallback-address inference; `192.168.4.1` with no application listeners shows that the ESP application stopped making progress during its clone-mode filesystem operation, but does not by itself prove a reset.

## ESP 2.32 atomic replacement candidate

The pinned ESP8266 2.7.4 LittleFS implementation performs replacement of an existing destination inside `lfs_rename()` as one directory commit. ESP 2.32 removes the clone path's redundant `LittleFS.remove(updatedFileName)` call and renames the validated `Temp` file directly over the live event file. This eliminates the observed missing-live-file window and avoids forcing LittleFS to allocate a new directory entry after every removal.

Verification passed with `just test`, `just check`, `git diff --check`, and the pinned `just esp-build` compiler gate. The ESP 2.32 sketch uses 574,632 bytes (55% of program storage) and 54,048 bytes (65% of statically allocated RAM), leaving 27,872 bytes for stack and heap.

ESP 2.32 installable sketch:

- Path: `Software/Huzzah/tmp/esp-build/ARDF_Transmitter.ino.bin`
- Size: 578,784 bytes
- SHA-256: `eec56756900a23c7c3d8658f4f1da2c60f2641c41073754f1c8d499ae6ef3006`
- MD5: `678765e9a8bd3cbc028999bd570027a7`

This remains an ESP-sketch-only candidate. The generated LittleFS image and AVR firmware remain outside the installation scope.

## ESP 2.32 Alpha hardware trials

The master and Alpha were installed and independently verified on ESP 2.32. Both reported the normalized installed-sketch MD5 `d61d2fdee00b5e12e3e2e079a40bf794`, mounted and protected LittleFS, and live AVR 0.210 telemetry.

- Trial 1 completed all 9 files in 30,927 ms with exact clock readback, confirmed cleanup, no receive overflow, no recovered reset, and a 14,256-byte minimum free heap. Every file proceeded directly from validation to `file-install-rename`; no separate remove phase occurred.
- Trial 2 completed all 9 files in 30,497 ms with exact clock readback, confirmed cleanup, no receive overflow, no recovered reset, and a 16,720-byte minimum free heap. All nine atomic replacements completed and the target reported `SLAVE,0`.
- Trial 3 completed all 9 files in 30,007 ms with exact clock readback, confirmed cleanup, no receive overflow, no recovered reset, and a 16,720-byte minimum free heap. All nine atomic replacements completed and the target reported `SLAVE,0`.
- The previously missing file 7, `Classic80m-Set3-1.event`, passed the atomic rename and advanced the installed-file count from 6 to 7. The clone then completed files 8 and 9, programmed the selected event, resumed the AVR, released ownership, and reported `SLAVE,0`.
- After an isolated restart with the master off, Alpha reported ESP 2.32, the expected installed-sketch MD5, mounted/protected LittleFS, and live AVR 0.210 telemetry. Its filesystem contained the restored 1.19 KB `Classic80m-Set3-1.event` and no `Temp` file.

Trial 1 therefore verifies both successful atomic replacement and recovery of the filesystem damage left by the ESP 2.31 failure. Trials 2 and 3 add two consecutive clean repeat power-cycle results. These three passes qualify the correction for a second-unit trial; broader fleet rollout remains gated on that independent-device result because the original failure was intermittent.

## ESP 2.32 Delta installation

Delta (`Tx_7C2CB9FD`) was updated from ESP 2.30 to ESP 2.32 using the sketch-only WiFi updater. The pre-update probe verified ESP 2.30 / AVR 0.210 and the exact device identity. Post-reboot verification reported the expected 578,784-byte sketch and normalized installed MD5 `d61d2fdee00b5e12e3e2e079a40bf794`, mounted/protected LittleFS without recovery mode, and live AVR 0.210 temperature and battery telemetry. Neither AVR firmware nor LittleFS content was installed.

## ESP 2.32 Delta hardware trials

- Trial 1 completed all 9 files in 34,027 ms with exact clock readback, confirmed cleanup, no receive overflow, no recovered reset, and a 16,216-byte minimum free heap. All nine atomic replacements completed, the selected Fox 4 event was programmed, and the target reported `SLAVE,0`.
- Trial 2 completed all 9 files in 33,893 ms with exact clock readback, confirmed cleanup, no receive overflow, no recovered reset, and a 15,920-byte minimum free heap. All nine atomic replacements completed, the selected Fox 4 event was programmed, and the target reported `SLAVE,0`.
- Trial 3 stopped after reporting `file-install-rename` for file 1. Its last record showed 22,168 free heap bytes, an 18,592-byte minimum, a 21,752-byte largest free block, 2 percent fragmentation, no receive overflow, and no reset evidence. The master recovered after its 15-second no-progress watchdog.
- In the failed state Delta's ESP LEDs were dark and its PCB red LED blinked slowly. The recovered master's ESP blue LED blinked slowly and all other master LEDs were dark. The Moto could not associate with Delta's exact SSID, so Delta was not advertising a usable access point after the stall.
- After an isolated hard restart, Delta returned on ESP 2.32 with the expected installed-sketch MD5, mounted/protected LittleFS, and live AVR 0.210 telemetry. Both the 1.13 KB `Temp` file and `Classic80m-Set1-1.event` remained. Their SHA-256 values were identical: `65681562c18abfa36f0a87fa9a687e360e76e64b007b3153f4269a779276adb7`.

The repeated failure inside direct `LittleFS.rename()` shows that removing the preceding explicit delete eliminated the missing-live-file window but did not eliminate the underlying filesystem-operation stall. The failure is not explained by low or fragmented heap. The retained source and destination prove that the atomic directory commit did not occur and that the existing live file was preserved.

## ESP 2.33 isolated LittleFS 2.5.1 diagnostic

A whole-core migration is not an acceptable diagnostic because prior hardware characterization found that ESP8266 cores 3.0.2 and 3.1.2 repeatedly reset before advertising an SSID. ESP 2.33 therefore keeps the hardware-qualified ESP8266 core 2.7.4, WebSockets 2.3.6, HUZZAH profile, WiFi stack, and core-2.7.4 LittleFS wrapper. Only the upstream LittleFS engine files `lfs.c`, `lfs.h`, `lfs_util.c`, and `lfs_util.h` are replaced with LittleFS 2.5.1 from the official ESP8266 core 3.1.2 source archive.

The diagnostic build is isolated and fail-closed:

- `just esp-build` remains the default ESP 2.32/core-2.7.4/LittleFS-2.2 build.
- `just esp-clone-littlefs-diagnostic-build` produces ESP 2.33 under `Software/Huzzah/tmp/esp-build-littlefs-2.5.1-diagnostic/`.
- The source archive SHA-256 must equal `b3f47686d7783c120c2f10bf82788f921c53db8642cc87012599abb6e335b182`.
- Build evidence verifies that all three compiled LittleFS translation units came from the isolated diagnostic tree.
- Both engines use LittleFS disk format 2.0, and the build rejects a diagnostic header with any other disk-format version; the existing filesystem is mounted in place without a format conversion.
- No LittleFS image is generated in diagnostic mode.
- The dedicated WiFi update recipe rejects an artifact whose size and SHA-256 do not match its sibling build evidence, or whose evidence does not identify core 2.7.4, WebSockets 2.3.6, and LittleFS 2.5.1.
- The update still requires an exact canonical target SSID and the normal explicit ESP-update confirmation.

Two independent builds were byte-identical and warning-free:

- Sketch path: `Software/Huzzah/tmp/esp-build-littlefs-2.5.1-diagnostic/ARDF_Transmitter.ino.bin`
- Size: 580,624 bytes
- SHA-256: `81f4d14a9af878c9a4551cab3775c4e08e885bc757cd7629a0014dbbee5ce190`
- File MD5: `4bd5593d882052f6b003294969114405`
- Expected normalized installed MD5 for the fleet flash mode: `e8aafd212f7894c013f2280903bac7a5`
- Sketch use: 576,472 bytes, 55 percent
- Static RAM: 54,156 bytes, 66 percent, leaving 27,764 bytes
- IRAM: 27,676 of 32,768 bytes

This candidate has 108 more bytes of static RAM than ESP 2.32 and remains within the same flash and IRAM percentages.

### Rename-work characterization

The upstream [LittleFS 2.5 release notes](https://github.com/littlefs-project/littlefs/releases/tag/v2.5.0) identify a former O(n^3) performance spike when rename coincides with directory compaction and state that the correction reduced `lfs_dir_traverse` calls by 97.8 percent in the contributor's case. The [ESP8266 core 3.1.2 release notes](https://github.com/esp8266/Arduino/releases/tag/3.1.2) confirm that its filesystem library contains LittleFS 2.5.1.

`just esp-clone-littlefs-diagnostic-characterize` exercises 3,000 direct `Temp` replacements across nine 1,200-byte event files in a 40-file root directory. It uses the FlexFox geometry: 1,024,000 bytes, 8,192-byte blocks, 64-byte read/program units, 125 blocks, and 16 block cycles.

| Engine | Total rename reads | Worst rename reads | Worst programs | Worst erases |
| --- | ---: | ---: | ---: | ---: |
| LittleFS 2.2 | 44,943,751 | 888,329 | 21 | 1 |
| LittleFS 2.5.1 | 10,015,056 | 31,267 | 21 | 1 |

LittleFS 2.5.1 reduced total read callbacks by 4.49 times and the worst replacement rename by 28.41 times. This synthetic result does not prove the exact hardware stall is fixed, but it establishes a concrete mechanism consistent with the failures: old-engine rename compaction can perform extraordinarily more flash reads without low heap, corruption, or a reset. The Delta pilot must still pass startup, mount/readback, retained-file inspection, and repeated master-to-target sync trials before any second unit is changed.

The same characterization also performed a rollback-compatibility cycle: LittleFS 2.2 created an image and two files, LittleFS 2.5.1 mounted it and atomically replaced the target file, then LittleFS 2.2 remounted the modified image and verified the replacement, the untouched marker file, and removal of `Temp`. This provides host-level evidence that returning Delta to ESP 2.32 after the pilot would not require rewriting its LittleFS contents.

### Morning pilot procedure

1. Keep the master off and power-cycle Delta alone.
2. Re-establish the Moto relay to exact target `Tx_7C2CB9FD` and verify ESP 2.32, AVR 0.210, mounted/protected LittleFS, and live bidirectional telemetry.
3. Install only the ESP 2.33 diagnostic sketch using `just wifi-esp-clone-littlefs-diagnostic-update Tx_7C2CB9FD` with the normal explicit confirmation and relay variables.
4. Require post-reboot ESP 2.33, normalized installed MD5 `e8aafd212f7894c013f2280903bac7a5`, unchanged AVR 0.210, mounted/protected LittleFS, no recovery mode, and live telemetry.
5. With Delta off, power the master and verify its retained ESP 2.32 state. Then power Delta to initiate each trial.
6. Run at least three initial sync trials, preserving master and target diagnostics after every trial. If any trial stalls, hard-restart Delta alone and compare `Temp` with the destination event before further testing.

### ESP 2.33 Delta hardware pilot

On 2026-09-03, Delta (`Tx_7C2CB9FD`) was independently verified on ESP 2.32 with AVR 0.210 and live HTTP, WebSocket, temperature, and battery telemetry. Only the evidence-bound ESP 2.33 diagnostic sketch was then installed. The updater verified the normalized installed-sketch MD5 `e8aafd212f7894c013f2280903bac7a5`, an uptime reset, mounted and protected LittleFS without recovery mode, and unchanged AVR 0.210 telemetry. No AVR firmware or LittleFS image was installed.

- Trial 1 appeared to complete normally and Delta performed its expected prompt post-clone power-down. The Moto did not establish the diagnostic connection before shutdown, so this is retained as an operator-observed success rather than an instrumented pass.
- Trial 2 synchronized the clock exactly at epoch `1788434900`, installed one file, and then timed out while receiving file 2 after 16 lines and 12,497 ms. Delta reported `file-receive` / `file data timed out`, 19,104 bytes free heap, a 16,592-byte minimum, a 17,872-byte largest block, 7 percent fragmentation, no receive-buffer overflow, no recovered reset, and confirmed AVR/session cleanup. This is a transfer interruption rather than a LittleFS rename stall.
- After Trial 2, LittleFS remained mounted and protected without recovery mode. The original file-2 destination remained present at its full 1.13 KB size, while a partial 399-byte `Temp` file remained. Thus the interrupted reception preserved the live event but session cleanup did not remove the incomplete temporary file.
- An isolated hard restart after Trial 2 returned normally on ESP 2.33 with the expected installed MD5, AVR 0.210, mounted/protected LittleFS, no recovery mode, and live bidirectional telemetry. The 399-byte `Temp` file and full live destination both remained. Because the master was intentionally off, the new startup attempt ended at `master-association` and replaced the in-memory Trial 2 diagnostic snapshot; the earlier exact Trial 2 record had already been captured above. The next received file opens `/Temp` with write/truncate semantics, so this retained partial file cannot be appended to or installed as the next event.
- Trial 3 was captured through the ESP 2.32 master's forwarded diagnostic stream. Delta synchronized its clock exactly, received and validated all nine files, completed all nine LittleFS 2.5.1 atomic replacement renames, programmed `Classic 80m Set 1-1` as `Fox 4 - MOH`, resumed its AVR, confirmed cleanup, and released ownership with `SLAVE,0`. The master's retained terminal target record was `release` at 33,392 ms with nine files installed, 16,776 bytes free heap, a 16,104-byte minimum, a 15,992-byte largest block, 5 percent fragmentation, no receive-buffer overflow, no recovered reset, and zero master-watchdog recoveries.
- An isolated post-Trial-3 restart verified the exact ESP 2.33 installed MD5, unchanged AVR 0.210, mounted/protected LittleFS without recovery mode, and live bidirectional telemetry. All nine transferred event files were present and `/Temp` was absent, confirming that the final successful atomic rename consumed the temporary file and left a clean filesystem.
- Trial 4 was also captured through the master and completed all nine receive, validation, and LittleFS 2.5.1 replacement-rename cycles. Delta again programmed `Classic 80m Set 1-1` as `Fox 4 - MOH`, resumed its AVR, confirmed cleanup, and released with `SLAVE,0`. The retained terminal record was `release` at 35,896 ms with 16,776 bytes free heap, a 16,104-byte minimum, a 16,000-byte largest block, 5 percent fragmentation, exact clock readback, no receive-buffer overflow, no recovered reset, and zero master-watchdog recoveries.
- Trial 5 completed the same nine-file sequence in 32,812 ms with exact clock readback at epoch `1788436034`, confirmed cleanup, and `SLAVE,0` release. Its terminal record again showed 16,776 bytes free heap, a 16,104-byte minimum, a 16,000-byte largest block, 5 percent fragmentation, no receive-buffer overflow, no recovered reset, and zero master-watchdog recoveries. Trials 3 through 5 therefore provide three consecutive instrumented passes and 27 completed LittleFS 2.5.1 replacement renames, including three passes through the former file-7 failure area.
- The final isolated post-pilot restart passed the exact ESP 2.33 installed MD5, unchanged AVR 0.210, mounted/protected LittleFS without recovery mode, live bidirectional telemetry, presence of all nine transferred event files, and absence of `/Temp`. Its terminal `master-association` status is the expected result of deliberately booting Delta while the master remained off, not a clone failure. These results complete the planned initial Delta pilot gate; expansion beyond Delta remains a separate decision.

### ESP 2.34 pre-Alpha diagnostic hardening

The Delta pilot exposed two smaller forensic gaps outside the LittleFS rename stall. A file-receive timeout left a partial, still-open `/Temp` file, and an operator had to copy the master's forwarded terminal diagnostic before later power changes made the live endpoints unavailable.

ESP 2.34 retains the same isolated LittleFS 2.5.1 engine and adds selective staging cleanup. A clone failure removes `/Temp` only when reception is still active and EOF was never received. A complete temporary file is still retained when validation or replacement fails so filesystem forensics remain possible. `CDIAG` and `/firmware/status` now report whether incomplete-Temp cleanup was attempted and confirmed.

Diagnostic retention remains deliberately off the target filesystem so extra diagnostic flash writes cannot alter the rename workload under test. `just wifi-adb-clone-monitor Tx_Master` captures the filtered master-side identity, health, `CDIAG`, `SLAVE`, success, and error records into a timestamped ignored JSONL file under `Software/Huzzah/tmp/clone-monitor/`; it excludes `FDAT` event contents and repetitive `SYNC` traffic. Alpha testing must use this capture before the target powers up.

ESP 2.34 remains sketch-only. It changes neither AVR 0.210 nor the on-device LittleFS image or disk format.

Host verification passed the full `just test` suite, including the selective temporary-file state model, filtered clone-monitor capture, firmware contracts, and existing clone safety tests. Two independent pinned builds were byte-identical and warning-free:

- Sketch path: `Software/Huzzah/tmp/esp-build-littlefs-2.5.1-diagnostic/ARDF_Transmitter.ino.bin`
- Size: 581,120 bytes
- SHA-256: `a50c130c80bf80177cdd521c42f833c3e69f49878b1e8a4dc029fd3c3819c3da`
- File MD5: `f8ef467aaefda51a9403eb1e98a4db9c`
- Expected normalized installed MD5 for fleet flash mode 2: `498f9dbb800a3d8dff00781729dd21f5`
- Sketch use: 576,972 bytes, 55 percent
- Static RAM: 54,312 bytes, 66 percent, leaving 27,608 bytes
- IRAM: 27,676 of 32,768 bytes

Alpha hardware testing must first verify ESP 2.34 installation and the exact normalized MD5, then use the durable master-side monitor for at least three trials. Successful trials cover unchanged rename behavior and the new capture path. A naturally occurring file-receive interruption additionally covers the new selective partial-`Temp` cleanup fields; absence of such an interruption is not evidence that the cleanup branch ran.

### ESP 2.34 Alpha hardware verification

On 2026-09-03, Alpha (`Tx_C22DD73C`) was first identified through the Moto relay on ESP 2.32 / AVR 0.210 with mounted and protected LittleFS, no recovery mode, and live temperature and battery telemetry. Only the evidence-bound ESP 2.34 sketch was installed. The guarded updater and an independent post-reboot probe verified the expected 581,120-byte sketch, normalized installed MD5 `498f9dbb800a3d8dff00781729dd21f5`, unchanged AVR 0.210, mounted/protected LittleFS without recovery mode, and live bidirectional telemetry. No AVR firmware or LittleFS image was installed.

Three Alpha sync trials were captured through the ESP 2.32 master (`Tx_Master`, immutable identity `Tx_930F093E`) using separate filtered JSONL files. Each capture retained identity, health, control, and every forwarded `CDIAG` transition while excluding event-file `FDAT` contents and repetitive `SYNC` traffic.

- Trial 1 completed all nine file receives, checksum validations, and LittleFS 2.5.1 replacement renames; programmed `Classic 80m Set 1-1` as `Fox 1 - MOE`; confirmed normal cleanup; resumed the AVR; and released with `SLAVE,0` in 41,736 ms. Minimum free heap was 16,456 bytes. No receive overflow, recovered reset, failure, or incomplete-Temp cleanup was reported.
- Trial 2 completed the same nine-file sequence and role programming in 33,057 ms. Minimum free heap was again 16,456 bytes. Normal cleanup and release were confirmed with no receive overflow, recovered reset, failure, or incomplete-Temp cleanup.
- Trial 3 completed the same nine-file sequence and role programming in 27,067 ms. Minimum free heap was again 16,456 bytes. The master's retained terminal record was the exact `release` diagnostic, with zero watchdog recoveries and no master failure. No receive overflow, recovered reset, failure, or incomplete-Temp cleanup was reported.

The three consecutive Alpha passes add 27 successful hardware replacement renames to the 27 previously completed by Delta. They also live-verify that the new durable capture path survives the target's prompt post-clone shutdown and records a complete terminal result without writing diagnostics to the target filesystem. Because no Alpha receive interruption occurred, the selective partial-`Temp` cleanup branch remains host-tested but was not exercised by these three hardware trials.

Two manual master-power interruptions then bounded the narrow first-file timing window without harming Alpha:

- A five-second cut occurred after file 1 had fully received, validated, and renamed. Alpha detected `WebSocket disconnected during clone` in `file-wait`, confirmed normal session cleanup, resumed live ESP/AVR telemetry, and retained the exact ESP 2.34 MD5 with mounted/protected LittleFS. One file was installed and no incomplete temporary file existed, so the selective cleanup correctly reported neither attempted nor confirmed.
- A four-second cut occurred before the first file opened. Alpha again recovered to `file-wait` / `WebSocket disconnected during clone`, with zero files received or installed, confirmed normal session cleanup, the exact ESP 2.34 MD5, mounted/protected LittleFS, and live ESP/AVR telemetry. The incomplete temporary-file fields again correctly remained clear.

The captures show that the interval in which `/Temp` is actively receiving the first file is under one second on these runs, between approximately 4.4 and 5.1 seconds after Alpha starts. Manual switch timing is therefore not a reliable or efficient way to exercise selective partial-file cleanup. A purpose-built opt-in fault hook or a naturally occurring receive interruption is required for that remaining live branch.

Intentional master power loss also exposed a host-monitor presentation defect: after already proving the live path, the monitor treated the expected WebSocket loss as an unhandled rejection and printed a stack trace. The host-only monitor now records `monitor-disconnect`, exits the monitoring promise cleanly, and still writes `monitor-end`; initial connection failures remain errors. The updated unit test and full `just test` suite pass. No device firmware change or reflash was required for this monitor correction.

## ESP 2.35 normal-build promotion and fleet plan

After the Delta and Alpha pilot results, the qualified LittleFS 2.5.1 engine was promoted from the opt-in sketch-only path into the normal `just esp-build` workflow. The normal build retains ESP8266 core 2.7.4, WebSockets 2.3.6, the qualified HUZZAH profile, LittleFS disk format 2.0, and all diagnostic and corrective behavior tested in ESP 2.34. It now verifies that all three compiled LittleFS translation units came from the checksum-pinned qualified library and records the source archive identity in normal build evidence.

The normal firmware is ESP 2.35 so a changed production artifact cannot share the already deployed ESP 2.34 diagnostic version. Two independent normal builds were byte-identical and warning-free. The normal sketch is also byte-identical to the current sketch-only qualification artifact.

- Sketch path: `Software/Huzzah/tmp/esp-build/ARDF_Transmitter.ino.bin`
- Size: 581,120 bytes
- SHA-256: `3e3817c2b60e2f8b94d4cb40ff2fcb6360cf5db80404e295af1114d634749acb`
- File MD5: `2c19e447d53279c2f23fb4267b397205`
- Expected normalized installed MD5 for fleet flash mode 2: `08bd25aad7c950470eb3734edf05fbc4`
- Sketch use: 576,972 bytes, 55 percent
- Static RAM: 54,312 bytes, 66 percent, leaving 27,608 bytes
- IRAM: 27,676 of 32,768 bytes
- LittleFS source archive SHA-256: `b3f47686d7783c120c2f10bf82788f921c53db8642cc87012599abb6e335b182`

The normal build also generates the usual 1,024,000-byte LittleFS image, but the fleet plan installs only the ESP sketch and preserves every unit's existing filesystem and AVR firmware. `just wifi-esp-normal-update <immutable-device-ssid> <reconnect-ssid>` requires sibling normal-build evidence, exact artifact size and SHA-256, ESP 2.35, core 2.7.4, WebSockets 2.3.6, LittleFS 2.5.1/disk 2.0, the checksum-pinned source archive, exact immutable device identity, and an explicit update confirmation. A master reconnects through `Tx_Master`; a target reconnects through its canonical device SSID.

The full `just check` suite and 3,000-cycle LittleFS characterization pass after promotion. The planned rollout is Master and Alpha followed by a sync test; then Bravo through Echo with one master-observed sync each; then Quebec as the second master followed by Foxtrot through Oscar with one sync each. Unless the master reports a failure, stall, missing terminal diagnostic, or ambiguous result, target-side reconnection is deliberately omitted to reduce fleet time.

### ESP 2.35 Master and Alpha hardware gate

On 2026-09-03, the primary master (advertised SSID `Tx_Master`, immutable identity `Tx_930F093E`) was verified on ESP 2.32 / AVR 0.210 with master role, mounted and protected LittleFS, no recovery mode, and live HTTP, WebSocket, temperature, and battery telemetry. Only the evidence-bound normal ESP 2.35 sketch was installed. The guarded updater and an independent post-reboot connection verified the expected 581,120-byte sketch, normalized installed MD5 `08bd25aad7c950470eb3734edf05fbc4`, unchanged AVR 0.210, master role, mounted/protected LittleFS without recovery mode, and live bidirectional telemetry.

Alpha (`Tx_C22DD73C`) was independently verified on ESP 2.34 / AVR 0.210 with target role, the expected prior diagnostic MD5, mounted/protected LittleFS, and live telemetry. Its retained `master-association` failure was the expected consequence of starting Alpha while the master was deliberately off. Only the normal ESP 2.35 sketch was installed. The guarded updater and an independent post-reboot connection verified the same expected 581,120-byte sketch and normalized installed MD5, unchanged AVR 0.210, target role, mounted/protected LittleFS without recovery mode, and live bidirectional telemetry. Neither installation changed AVR firmware or installed the generated LittleFS image.

The first ESP 2.35 production sync was captured from the primary master in `Software/Huzzah/tmp/clone-monitor/clone-monitor-2026-09-03T15-52-38-853Z.jsonl`. Alpha matched the master's clock exactly at epoch `1788450881`, received and validated all nine files, completed all nine LittleFS 2.5.1 atomic replacement renames, programmed `Classic 80m Set 1-1` as `Fox 1 - MOE`, confirmed normal cleanup, resumed its AVR, and reached terminal `release` followed by `SLAVE,0` in 29,102 ms. The master's retained target record reported 14,304 bytes minimum free heap, no receive-buffer overflow, no recovered reset, zero master-watchdog recoveries, and no master failure. This passes the Master/Alpha hardware gate for expansion to Bravo through Echo.
