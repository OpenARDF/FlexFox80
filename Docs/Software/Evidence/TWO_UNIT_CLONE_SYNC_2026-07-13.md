# Two-Unit Clone Synchronization Qualification — 2026-07-13

**Path:** B-TIME-01

**Status:** End-to-end clone/readback passes; reset-dependent one-second AVR system-time quantization reproduced on both units; edge-aligned boot fix built but not yet hardware-qualified

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

## TDD correction awaiting hardware qualification

A firmware source contract was added first and failed against the immediate boot read/set sequence. The minimal correction replaces only that boot sequence with the existing `syncSystemTimeToRTC()` helper, which waits for the next RTC edge, reads the RTC, and sets AVR system time. Clone protocol, ordinary clock writes, ISR contents, event scheduling, EEPROM, and ESP firmware are unchanged.

The contract then passed, and the exact pinned Mac AVR-GCC 7.3.0 / AVR-Dx_DFP 1.9.103 Release build completed with zero warnings and the required unchanged 274-byte EEPROM image. Hardware programming and repeated reset-phase measurements remain mandatory before this image is treated as qualified.

## What this proves

- Two complete clone attempts reached the exact clone-specific RTC readback gate and completed normal cleanup.
- The final target state was not immediately displaced from the master by multiple seconds.
- Two target observations four minutes apart reproduced the target median within 10 ms.
- The observed approximately half-second target relationship is compatible with the established DS3231 write/edge phase model.
- The previously demonstrated multi-second WebSocket delivery tails must not be interpreted as RTC phase without follow-up edges or physical timing evidence.
- Resetting either AVR can change its reported/system phase by approximately one whole second while leaving the RTC and communications operational.
- A correct clone RTC readback alone did not remove that boot-induced system-time quantization.

## Residual risk and next measurement

This is one measured final target state after two back-to-back clone attempts. It does not yet establish the distribution across repeated clones, directly timestamp the two physical RTC edges, exercise every disconnect/error cleanup path, or explain a unit that diverges after several days.

Next:

1. program the edge-aligned boot candidate on the dummy-loaded target after confirming the Atmel-ICE is attached to that unit;
2. repeat resets at varied phases and require a stable median rather than the observed one-second state changes;
3. qualify a master/target clone with both AVRs running the corrected image;
4. investigate the interrupted six-file transfer as a separate cleanup/timeout defect;
5. retain 24-hour and multi-day drift observations and compare RTC aging values before changing calibration;
6. do not close B-TIME-01 until reset, repeated clone phase, and drift evidence exclude an unacceptable tail.
