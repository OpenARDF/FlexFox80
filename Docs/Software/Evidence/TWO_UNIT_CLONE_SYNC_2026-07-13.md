# Two-Unit Clone Synchronization Qualification — 2026-07-13

**Path:** B-TIME-01

**Status:** Edge-aligned boot fix passes exact programming and reset qualification on both units; first two corrected clone/readback trials pass with 0.48–0.55-second target lags

## Objective

Exercise the updated master and target ESP/AVR state machines together, require the target's clone-specific RTC readback before file transfer, and compare both units through the same read-only Moto/DroidTether observation path. Absolute UTC accuracy is not the objective. The important result is the target's repeatable relationship to the master's RTC second boundary.

## Units and route

| Role | SSID | ESP MAC | Reported role | Software versions |
| --- | --- | --- | ---: | --- |
| Master | `Tx_Master` | `22:C8:8E:CF:AB:84` | `MASTER,1` | `2.0,0.200` |
| Target | `Tx_7C2D69ED` | `1A:0D:BB:2E:2C:4C` | `MASTER,0` | `2.0,0.200` |

The Mac remained on its normal network. Only `73.73.73.73` was routed through the Moto USB tunnel. Master and target samples therefore used the same Mac, phone, DroidTether build, host route, observer, and sample count, although they were necessarily collected sequentially after switching the Moto between the two FlexFox access points.

The target is the authorized dummy-loaded bench unit. Its event files were replaced by the master's normal clone workflow. The master reported that no cloned event was currently scheduled to run.

## End-to-end clone handshake

The target was reset twice at the operator's direction. The master observer captured two complete transfer cycles. Each cycle:

1. entered the updated master/target clone session;
2. produced the master's next-edge one-shot `SYNC` report;
3. proceeded into `SLAVE,WFF` event-file transfer;
4. transferred all nine event files with their existing `CHECK,1013` records;
5. ended with `SLAVE,NMF`, `SUE,No events scheduled to run`, and `SLAVE,0` release.

The duplicate file transfer was expected because the operator reset the target twice; it was not an automatic retry.

This is direct evidence for the new RTC gate. In the updated target state machine, file transfer is unreachable until `$TIM,<ISO>,C;` has been queued to the AVR, the returned `!TIM,C,<epoch>;` exactly matches the requested epoch, the ordinary ACK has cleared, and neither a timeout nor NAK is present. Both observed cycles crossed that gate.

The master emitted `ERR_CODE,247` before the first transfer. Code 247 is the defined `ERROR_CODE_NO_ANTENNA_FOR_BAND`; it is not a clock-write, RTC-readback, or clone-state error. No clone-time NAK, mismatch, or timeout was observed.

## Immediate clock comparison

`just wifi-clock-observe` collected 12 ordinary reports from each unit. The observer's offset is Mac receipt time minus the integer epoch reported by the AVR. It includes ESP, WiFi, Moto, USB-tunnel, WebSocket, and Mac scheduling latency, so the robust inter-unit difference is more useful than either absolute offset.

| Unit | Samples | Median offset | Mean offset | Sample standard deviation | Minimum | Maximum | Spread |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Master | 12 | -322.5 ms | -317.25 ms | 50.44 ms | -389 ms | -217 ms | 172 ms |
| Target | 12 | +214.5 ms | +247.25 ms | 102.93 ms | +156 ms | +554 ms | 398 ms |
| Target repeat, four minutes later | 12 | +224.5 ms | +229.67 ms | 57.88 ms | +156 ms | +364 ms | 208 ms |

The target-minus-master difference was:

- **537 ms by medians**;
- **564.5 ms by means**.

A larger observer offset means the reported FlexFox epoch is later relative to the Mac receipt timestamp. Under this common path, the target was therefore approximately 0.54–0.56 seconds behind the master's second boundary. That is consistent with the expected approximately half-second phase relationship after writing the DS3231 seconds register and is far from the field symptom of more than three seconds.

One target observation arrived at +554 ms while the other eleven were between +156 and +266 ms. The repeat series' median changed by only 10 ms, its maximum was +364 ms, and its spread fell to 208 ms. The +554 ms sample therefore did not represent a persistent RTC shift; the median remains the appropriate summary for this non-real-time observation path. The repeat target median is approximately 547 ms behind the earlier master median.

## Reset-dependent one-second shift

A subsequent trial reset the master, repeated its baseline, cloned the target again, and then reset only the target with the master powered off. No clock command or clone occurred between the target's pre-reset and post-reset observations.

The master median moved from the original -322.5 ms to +606 ms and then +619 ms in two post-reset series. The persistent reset-associated change was therefore approximately +928.5 to +941.5 ms. After cloning from that reset master, the target measured +1179 ms median, +1215.58 ms mean, 100.24 ms sample standard deviation, +1114 ms minimum, +1402 ms maximum, and 288 ms spread. Its relationship to the reset master remained the expected approximately 560–573 ms lag.

After resetting only the target, its 12-sample result was +149.5 ms median, +186.50 ms mean, 116.58 ms sample standard deviation, +105 ms minimum, +523 ms maximum, and 418 ms spread. The target median changed by **-1029.5 ms**, and its mean changed by **-1029.08 ms**, without an intervening time write. The first connection attempts after the reset timed out while the Moto path was re-establishing, but the target then passed HTTP, WebSocket, identity, temperature, battery, and clock probes normally.

The same near-one-second reset-dependent behavior has therefore been reproduced independently on both AVR units. Observation-path delay can widen individual samples, but it cannot plausibly explain a persistent approximately one-second median change confined to a device reset.

Source review identified a mechanism consistent with the measurements. After `rtc_init()` enables the DS3231 square wave, boot immediately read the RTC and called `set_system_time()` at an arbitrary phase. The next square-wave ISR then called `system_tick()`. Depending on whether the RTC register transition occurred before or after that immediate read, AVR system time could acquire a whole extra second relative to the RTC edge. Clone one-shot reports and schedule decisions use AVR system time, so an otherwise correct RTC can expose this quantization.

## Transfer interruption observation

The clone following the master reset crossed the exact RTC readback gate but stopped after six of nine event files for more than one minute. Resetting the target caused one additional file to appear before that interrupted session ended, after which a new clone attempt began automatically and completed all nine files plus normal `SLAVE,NMF`, schedule evaluation, and `SLAVE,0` cleanup. This is separate robustness evidence: the successful retry protects against permanent loss, but the stalled session did not demonstrate timely autonomous recovery and should be investigated independently from clock phase.

The operator estimates that a clone requires a retry approximately 5–10% of the time in ordinary use. This is an experience-based incidence estimate rather than a controlled sample rate. The retry is easy and the failure is annoying rather than operationally blocking, so it is tracked separately as medium-severity `B-CLONE-02`; it does not block the `B-TIME-01` timing qualification. The first two corrected clone attempts both transferred all nine files and cleaned up normally, which is useful but not enough to refute an intermittent 5–10% tail.

## TDD correction and target hardware qualification

A firmware source contract was added first and failed against the immediate boot read/set sequence. The minimal correction replaces only that boot sequence with the existing `syncSystemTimeToRTC()` helper, which waits for the next RTC edge, reads the RTC, and sets AVR system time. Clone protocol, ordinary clock writes, ISR contents, event scheduling, EEPROM, and ESP firmware are unchanged.

The contract then passed, and two exact pinned Mac AVR-GCC 7.3.0 / AVR-Dx_DFP 1.9.103 Release builds completed with zero warnings and identical artifacts. The 274-byte EEPROM image remained unchanged. The Release HEX SHA-256 is `130147e4182a30897a170c16481530ddb37ebd419271ac33c3df35316cbf853b`.

The Atmel-ICE identified the powered dummy-loaded target as AVR128DA48 signature `1E 97 08` at 3.26 V. Fresh pre-write flash, complete EEPROM, and fuse captures were byte-identical to the target's prior clone-sync qualification. Programming used an explicit erase, wrote and verified the candidate flash twice, restored and verified all 512 EEPROM bytes twice, and did not write fuses. Independent post-operation reads established:

| Memory | Expected SHA-256 | Independent readback SHA-256 | Result |
| --- | --- | --- | --- |
| Candidate flash binary | `629b6b5ce564da965ba9977fae6d0653ac74b1e100980859452a7888a35a85ab` | `629b6b5ce564da965ba9977fae6d0653ac74b1e100980859452a7888a35a85ab` | byte-identical |
| Preserved EEPROM | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

The programmed target then passed HTTP, WebSocket, target role, identity, temperature, battery, and live AVR clock probes. Its post-program boot and five subsequent Atmel-ICE-induced AVR resets each received 12 ordinary clock reports through the same observation path:

| Boot | Median offset | Mean offset | Sample standard deviation | Minimum | Maximum | Spread |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Post-program baseline | +1217.5 ms | +1249.33 ms | 86.88 ms | +1148 ms | +1448 ms | 300 ms |
| Reset 1 | +1201.0 ms | +1223.58 ms | 77.87 ms | +1174 ms | +1459 ms | 285 ms |
| Reset 2 | +1227.0 ms | +1230.67 ms | 48.47 ms | +1178 ms | +1336 ms | 158 ms |
| Reset 3 | +1269.5 ms | +1267.25 ms | 86.73 ms | +1144 ms | +1464 ms | 320 ms |
| Reset 4 | +1229.0 ms | +1287.67 ms | 142.07 ms | +1152 ms | +1594 ms | 442 ms |
| Reset 5 | +1188.5 ms | +1230.17 ms | 126.56 ms | +1143 ms | +1596 ms | 453 ms |

Across the six boot medians, the mean was +1222.08 ms, the median was +1222.25 ms, the sample standard deviation was 27.98 ms, and the complete range was 81 ms. No reset produced an integral-second state change. The 81 ms range is more than twelve times smaller than the pre-fix target's 1029.5 ms reset shift and is small relative to individual WebSocket/tunnel receipt spread. This qualifies the edge-aligned boot correction on the target for the reproduced defect; it does not by itself explain all contributors to the field outlier.

## Master hardware qualification

The same candidate was then installed on the master after fresh reads reproduced its qualified prior flash, migrated configured EEPROM, and fuse hashes. Programming used an explicit erase, wrote and verified the candidate flash twice, restored and verified the complete configured EEPROM twice, did not write fuses, and ended with independent byte-identical reads of the candidate raw flash `629b6b5ce564da965ba9977fae6d0653ac74b1e100980859452a7888a35a85ab`, master EEPROM `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c`, and preserved fuses `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126`.

The programmed master passed its role/identity, HTTP, WebSocket, temperature, battery, and live-clock probes. Its post-program boot and five subsequent Atmel-ICE-induced resets produced these 12-sample results:

| Boot | Median offset | Mean offset | Sample standard deviation | Minimum | Maximum | Spread |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Post-program baseline | +649.5 ms | +667.50 ms | 71.02 ms | +601 ms | +859 ms | 258 ms |
| Reset 1 | +639.0 ms | +668.42 ms | 65.87 ms | +593 ms | +791 ms | 198 ms |
| Reset 2 | +649.5 ms | +664.92 ms | 71.42 ms | +593 ms | +817 ms | 224 ms |
| Reset 3 | +678.5 ms | +694.83 ms | 86.17 ms | +592 ms | +898 ms | 306 ms |
| Reset 4 | +665.5 ms | +687.42 ms | 78.60 ms | +609 ms | +899 ms | 290 ms |
| Reset 5 | +644.5 ms | +671.67 ms | 106.86 ms | +592 ms | +988 ms | 396 ms |

Across the master boot medians, the mean was +654.42 ms, the median was +649.5 ms, the sample standard deviation was 14.75 ms, and the complete range was 39.5 ms. No reset produced an integral-second state change. The edge-aligned correction therefore passes the reproduced reset-defect gate on both the target and master, with boot-median ranges of 81 ms and 39.5 ms respectively. See [Master clone-synchronization firmware upgrade](MASTER_CLONE_SYNC_UPGRADE_2026-07-13.md) for the master preservation and programming history.

## Corrected two-unit clone qualification

With both AVRs running the edge-aligned image, one operator-requested target reset initiated a fresh clone while the master observer remained connected. The transfer reached all nine event files:

- `Classic80m-Set1-1.event` through `Classic80m-Set1-3.event`;
- `Classic80m-Set2-1.event` through `Classic80m-Set2-3.event`;
- `Classic80m-Set3-1.event` through `Classic80m-Set3-3.event`.

Every file ended with its existing `CHECK,1013` record and `EOF`. The session then emitted `SLAVE,NMF`, `SUE,No events scheduled to run`, and `SLAVE,0`, after which ordinary clock reports resumed. File transfer remains unreachable until the target's clone-specific RTC epoch readback exactly matches the requested epoch and the ordinary Linkbus ACK clears. The corrected clone therefore crossed the intended clock gate and completed normal cleanup. The earlier six-file stall did not recur in this attempt, but one passing retry does not close that separate robustness observation.

Without resetting the target after cloning, the same Moto/DroidTether observer collected one master series and two target series:

| Unit/series | Samples | Median offset | Mean offset | Sample standard deviation | Minimum | Maximum | Spread |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Master, immediately after clone | 12 | +693.0 ms | +704.00 ms | 80.06 ms | +594 ms | +846 ms | 252 ms |
| Target, first series | 12 | +1172.0 ms | +1218.42 ms | 99.89 ms | +1139 ms | +1471 ms | 332 ms |
| Target, repeat | 12 | +1199.0 ms | +1197.42 ms | 43.17 ms | +1139 ms | +1262 ms | 123 ms |

Relative to the master, the target lag was 479 and 506 ms by medians and 514.42 and 493.42 ms by means. The two target medians differ by only 27 ms. The isolated +1471 ms target receipt did not persist in the repeat series, whose entire spread was 123 ms. These data are consistent with the intended approximately half-second DS3231 write/edge relationship and contain no additional integral-second state error.

The operator then had to reset the master to wake its WiFi before reconnecting the Moto. Its closing 12-sample series measured +685.5 ms median, +669.17 ms mean, 61.47 ms sample standard deviation, +590 ms minimum, +747 ms maximum, and 157 ms spread. The reset changed the master median by only -7.5 ms from its immediate post-clone series. This is an additional post-clone challenge of the corrected boot path and did not reproduce the pre-fix approximately one-second reset shift.

A second corrected clone was then initiated from that reset-qualified master. It again crossed the exact target RTC-readback gate, transferred all nine files with `CHECK,1013` and `EOF`, emitted normal `SLAVE,NMF` / schedule evaluation / `SLAVE,0` cleanup, and resumed clock reports. Its paired same-path series were:

| Unit/series | Samples | Median offset | Mean offset | Sample standard deviation | Minimum | Maximum | Spread |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Master, corrected clone 2 | 12 | +683.5 ms | +726.17 ms | 145.51 ms | +622 ms | +1148 ms | 526 ms |
| Target, corrected clone 2 | 12 | +1231.5 ms | +1245.67 ms | 79.18 ms | +1176 ms | +1448 ms | 272 ms |

The second clone's target lag was 548 ms by medians and 519.50 ms by means. One +1148 ms master receipt widened its mean, standard deviation, and spread, but its median stayed within 10 ms of the first corrected clone's +693 ms master median. Across the first two corrected clones, the paired median target lags were 479 and 548 ms; including the first target's immediate repeat comparison gives 479, 506, and 548 ms. No comparison contains an additional integral-second error. Both corrected file transfers completed normally, although two passes cannot refute the operator-estimated 5–10% `B-CLONE-02` retry rate.

## What this proves

- Two complete clone attempts reached the exact clone-specific RTC readback gate and completed normal cleanup.
- The final target state was not immediately displaced from the master by multiple seconds.
- Two target observations four minutes apart reproduced the target median within 10 ms.
- The observed approximately half-second target relationship is compatible with the established DS3231 write/edge phase model.
- The previously demonstrated multi-second WebSocket delivery tails must not be interpreted as RTC phase without follow-up edges or physical timing evidence.
- Resetting either AVR can change its reported/system phase by approximately one whole second while leaving the RTC and communications operational.
- A correct clone RTC readback alone did not remove that boot-induced system-time quantization.
- With the edge-aligned boot candidate, five target resets plus the post-program boot remained within an 81 ms median range and showed no whole-second state change.
- With the same candidate, five master resets plus the post-program boot remained within a 39.5 ms median range and showed no whole-second state change.
- Candidate flash, restored EEPROM, and untouched fuses were independently verified byte-for-byte on the dummy-loaded target.
- Candidate flash, migrated configured EEPROM, and untouched fuses were independently verified byte-for-byte on the master.
- With both corrected images installed, a fresh clone crossed the exact target RTC-readback gate, transferred all nine files, and completed normal cleanup.
- The first two corrected clones produced paired target lags of 479–548 ms by medians, with no integral-second outlier.
- A post-clone master reset changed its median by only -7.5 ms and did not restore the former whole-second quantization.

## Residual risk and next measurement

This is two corrected clones plus the earlier pre-correction clone series. It does not yet establish the distribution across many corrected clones, directly timestamp the two physical RTC edges, exercise every disconnect/error cleanup path, or explain a unit that diverges after several days.

Next:

1. retain 24-hour and multi-day drift observations and compare RTC aging values before changing calibration;
2. add corrected-clone trials when convenient to broaden the immediate phase distribution;
3. investigate medium-severity `B-CLONE-02` as a separate cleanup/timeout defect without blocking timing qualification;
4. do not close B-TIME-01 until drift and broader clone evidence exclude an unacceptable tail.
