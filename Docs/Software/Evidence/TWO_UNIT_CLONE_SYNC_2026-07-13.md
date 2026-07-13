# Two-Unit Clone Synchronization Qualification — 2026-07-13

**Path:** B-TIME-01

**Status:** First end-to-end master/target clone and immediate phase comparison pass; repeated phase and drift series remain open

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

The target-minus-master difference was:

- **537 ms by medians**;
- **564.5 ms by means**.

A larger observer offset means the reported FlexFox epoch is later relative to the Mac receipt timestamp. Under this common path, the target was therefore approximately 0.54–0.56 seconds behind the master's second boundary. That is consistent with the expected approximately half-second phase relationship after writing the DS3231 seconds register and is far from the field symptom of more than three seconds.

One target observation arrived at +554 ms while the other eleven were between +156 and +266 ms. This increases the linear spread but does not indicate a persistent RTC shift; the median remains the appropriate summary for this non-real-time observation path.

## What this proves

- Two complete clone attempts reached the exact clone-specific RTC readback gate and completed normal cleanup.
- The final target state was not immediately displaced from the master by multiple seconds.
- The observed approximately half-second target relationship is compatible with the established DS3231 write/edge phase model.
- The previously demonstrated multi-second WebSocket delivery tails must not be interpreted as RTC phase without follow-up edges or physical timing evidence.

## Residual risk and next measurement

This is one measured final target state after two back-to-back clone attempts. It does not yet establish the distribution across repeated clones, directly timestamp the two physical RTC edges, exercise every disconnect/error cleanup path, or explain a unit that diverges after several days.

Next:

1. repeat clone-plus-immediate-observation trials and record target-minus-master median phase for every trial;
2. retain the current target state for 24-hour and multi-day drift observations if practical;
3. preserve and compare RTC aging values before changing calibration;
4. use physical RTC-edge or transmission-phase instrumentation if the routed observer shows a persistent or integral-second outlier;
5. do not close B-TIME-01 until the repeated phase distribution and drift evidence exclude an unacceptable tail.
