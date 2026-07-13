# Wireless Clone Time Synchronization Investigation

**Date:** 2026-07-12

**Path:** B-TIME-01

**Status:** Investigation active; AVR and ESP controls implemented; compatible build and single-unit live controls pass; full two-unit clone pending

## Observed field symptom

Ten Sprint FlexFoxes were wirelessly cloned/configured from one master several days before an event. One target was more than three seconds out of schedule. The other targets were operationally acceptable, although some appeared capable of closer synchronization. Setting the outlier from a cellphone improved it to within approximately one second.

The code has otherwise proved highly reliable, so this investigation is being kept separate from broad hardening. Preserve the original unit identities, elapsed time since synchronization, event assignment, RTC aging value, and firmware versions whenever they become available.

## Important correction to the working model

A completed clone workflow does **not** currently prove that the target RTC accepted the master's time. Event-file transfer and clock setting are separate stages, and neither stage presently supplies the assumed end-to-end proof:

1. The slave ESP receives `SYNC,<epoch>`, converts the integer epoch to an ISO time, and writes the `$TIM,...;` frame directly with `Serial.printf`.
2. That direct write bypasses the ESP's normal Linkbus output queue and ACK-pending state.
3. The following state declares success whenever the normal queue is empty and no unrelated ACK is pending. It can therefore report `Sync ack received` without waiting for the AVR to acknowledge this clock command.
4. On the AVR, `ds3231_set_date_time()` retries the I2C write ten times but returns `void` and discards final failure. The Linkbus handler then calls `syncSystemTimeToRTC()` and completes normally. If every write attempt fails, system time can simply be reloaded from the unchanged RTC.
5. The event-file sender transmits `EVENT_END`, then a separate `CHECK,<value>` line. `Event::validEventFile()` stops its loop as soon as it sees `EVENT_END`, so its checksum branch is unreachable. Its local `checksum` variable is also not initialized. A successful file workflow therefore does not validate the clock and does not currently demonstrate a matching event checksum in this validation path.

These are confirmed protocol/implementation gaps. They establish plausible failure mechanisms, but they do not yet prove which mechanism caused the field outlier.

## Expected phase limitation

The master broadcasts an integer epoch with no fractional second. The slave converts that integer back to an ISO timestamp only after WebSocket and application processing delay. The cellphone/browser path can supply milliseconds, but the ESP deliberately removes the fractional part before sending the AVR clock command.

Consequently, the current protocol cannot intentionally align second boundaries to substantially better than one second. Transport and processing phase can add further error. This explains ordinary sub-second or near-one-second differences, but a persistent error greater than three seconds calls for another contributor.

## Read-only observer

`just wifi-clock-observe` was added to sample the AVR epoch broadcasts without setting time or changing configuration. The observer sends only the `!&` heartbeat, converts each `SYNC,<epoch>` message to an offset from the Mac clock, and reports median and spread. The heartbeat is repeated every five seconds because a trial 30-second interval allowed this firmware's approximately ten-second WebSocket inactivity timer to close the connection after five samples. The result includes WebSocket and USB-tunnel latency, so comparisons must use the same route.

Initial attached-unit observation through the Moto/DroidTether route:

```text
samples=12
median_offset_ms=-2051
min_offset_ms=-2126
max_offset_ms=-1840
spread_ms=286
```

A negative offset means the FlexFox-reported epoch was ahead of the Mac clock. This unidentified bench unit was therefore approximately 2.05 seconds ahead during the completed observation. The 286 ms spread is small relative to a multi-second field error and supports using repeated medians to compare units. This is a baseline only; it is not evidence that the attached unit was the field outlier.

## Controlled RTC-write series

The attached dummy-loaded test unit is not one of the ten field units, so its original absolute offset cannot be compared with that group. It can, however, qualify the clock-setting mechanism. `just wifi-clock-sync-test` exercises the same browser `SYNC,<ISO timestamp>` path while requiring the explicit `FLEXFOX_ALLOW_CLOCK_SET=1` opt-in.

A later failed write would be invisible if every trial requested already-correct time. The test therefore uses a distinctive repeating `+8 seconds`, `-8 seconds`, and current-time sequence. It waits until the AVR's returned epoch matches each requested signature before continuing, restores current Mac time at the end or on a handled failure, and checks the restored phase. It sends no event, EEPROM, RF, or raw pass-through commands.

Completed evidence:

| Run | Verified writes | First returned report matched | Final current-time receive offset |
| --- | ---: | ---: | ---: |
| Qualification | 3/3 | 3/3 | median 794 ms; range 752–944 ms |
| Batch 1 | 10/10 | 10/10 | median 1437 ms; range 1432–1789 ms |
| Batch 2 | 10/10 | 10/10 | median 856 ms; range 841–903 ms |
| Batch 3 | 10/10 | 10/10 | median 1516 ms; range 1377–1726 ms |
| Batch 4, compatible clone-sync firmware | 30/30 | 29/30 | median 953 ms; range 904–1059 ms |

Primary result: **63/63 requested RTC writes were verified; no failed write was observed.** In Batch 4, one previously queued report arrived before the requested signature in trial 5, which then matched on its second report. The other 62 writes matched on the first report. A five-sample observation after Batch 4 signature traffic cleared reported a 975 ms median offset, 870–1407 ms range, and 537 ms spread, confirming that the test left the unit on current time.

Rapid alternating writes also demonstrated that previously emitted `SYNC` broadcasts can arrive after a later clock change. An interrupted long run briefly produced non-monotonic observer epochs from those queued signatures before current-time reports settled. Clone verification must therefore correlate a response with the requested value rather than treating any subsequent clock message as proof.

This result shows that the normal ESP-to-AVR/RTC write mechanism is repeatably functional on this unit. It does not remove the confirmed false-success paths, prove a rare I2C failure impossible, exercise the master/slave clone state machine, or explain multi-day unit-specific drift. It shifts the next experiment toward an actual clone followed immediately by readback, plus aging/drift measurements on the field outlier.

## Single-unit clone-control qualification

The clone-sync ESP image was built with the hardware-compatible ESP8266 core 2.7.4/WebSockets 2.3.6 profile, programmed with the mature filesystem preserved, and verified first on the standalone HUZZAH startup gate. After installation on the dummy-loaded FlexFox, HTTP, WebSocket, and live AVR telemetry all passed.

The opt-in `just wifi-clone-control-test` then exercised only the AVR clone-control frames through the deployed WiFi path. It observed ordinary clock reports before the test, verified that `$ESP,C;` suppressed them, verified that `$ESP,S;` produced exactly one report at the next RTC edge while quiet, and verified that `$ESP,R;` restored ordinary reports. The test sends a resume command during handled cleanup if quiet mode remains active. It does not write RTC, EEPROM, event, RF, or filesystem state.

This confirms the control primitives on one installed unit. It does not exercise the ESP master/target state machines, the target RTC write and exact clone readback, disconnect/error cleanup on both units, or inter-unit phase spread. Those require a second FlexFox with the matching AVR and ESP changes.

## One-unit edge-phase versus delivery characterization

The 2.372-second matching report in Batch 4 raised a necessary question: did the RTC edge actually move by that amount, or did a correct report arrive late? The opt-in `just wifi-clock-phase-test` narrows that ambiguity without changing product firmware. It suppresses ordinary reports, samples baseline one-shot edges, sends queued `$TIM,<ISO>,C;` writes at 100 ms after a Mac second boundary, and requests three consecutive one-shot edges after every write. Current Mac time and normal reports are restored at the end or after a handled failure. Raw evidence is retained beneath the ignored ESP temporary tree.

The completed 12-baseline/30-write run produced:

| Population | Samples | Mean receipt error | Sample standard deviation | Circular phase mean | Circular phase standard deviation | At least 1 s |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Baseline one-shot edges, before writes | 12 | 814.8 ms | 208.0 ms | 734.3 ms | 127.0 ms | 2/12 |
| First one-shot edge after each write | 30 | 842.6 ms | 279.7 ms | 789.6 ms | 125.7 ms | 3/30 |
| Second and third edges after writes | 60 | 801.3 ms | 276.0 ms | 765.8 ms | 74.7 ms | 1/60 |

All 30 writes produced the expected logical epoch: the first one-shot report was exactly the requested epoch plus one second. The circular first-edge phase mean shifted only 55.3 ms from baseline, and its circular spread was essentially unchanged. Multi-second delivery tails occurred independently of clock setting:

- baseline reports reached 1.051 and 1.395 seconds before any test write;
- write 19's correct first report arrived at 2.204 seconds, then its next two edges returned immediately to 796 and 717 ms;
- write 29's second edge arrived at 2.825 seconds, then the following edge returned to 667 ms.

The delayed write-19 report caused the next armed observation to skip an intermediate epoch, consistent with more than one second elapsing before the Mac could request another edge. These results prove that the WiFi/Linkbus/WebSocket/tunnel observation path has a transient multi-second delivery tail. They make the earlier 2.372-second receipt an unreliable proxy for RTC phase and show why matching a returned epoch alone cannot measure fractional synchronization.

The final current-time restore returned an edge at 766 ms. A separate post-test probe confirmed normal identity, temperature, battery, and clock traffic; three read-only clock samples then reported a 753 ms median, 747–893 ms range, and 146 ms spread.

They do **not** prove that the field schedule outlier was only an observation artifact: the field symptom concerned physical schedule coordination, and this single-unit path still cannot timestamp the RTC edge at generation independently of delivery. The remaining decisive test is repeated master-target cloning followed by physical inter-unit edge or transmission-phase comparison.

## Ranked hypotheses and discriminating evidence

| Rank | Hypothesis | Why it fits | Observation that distinguishes it |
| --- | --- | --- | --- |
| 1 | One RTC drifts faster because of oscillator quality or aging-register difference | One outlier after several days fits unit-specific drift; aging is queryable but is not cloned, and EEPROM `g_clock_calibration` is stored but not applied to the DS3231 | Unit begins close after setting, then offset changes approximately linearly; aging value differs from peers |
| 2 | RTC clock write failed but clone proceeded | Both ESP ACK handling and AVR I2C error reporting can falsely complete, although 63/63 browser-path and 30/30 queued phase-test writes passed on the bench unit | Target is already far off immediately after a nominal clone; RTC readback disagrees with master's transmitted time |
| 3 | Whole-second truncation and transport phase | Confirmed in both master-clone and cellphone paths | Stable initial error generally within about one second; repeated setting produces different sub-second phase |
| 4 | AVR system time loses a DS3231 square-wave tick after RTC setting | Event cycle countdown is driven by the one-second interrupt; a discrete missed tick could shift schedule while RTC remains correct | RTC readback remains correct while broadcast/system schedule jumps by an integer second |
| 5 | Master time was wrong | All targets inherit the master epoch | Most or all targets share a similar absolute offset rather than one target becoming an outlier |

As scale checks, three seconds accumulated over three days is about 11.6 ppm; over five days it is about 6.9 ppm. Either is larger than the commonly expected error of a healthy, uncompensated temperature-compensated RTC, so a measured slope at that level would justify comparing aging values and RTC hardware among units.

## B1/B2 measurement plan

Use the same Mac, Moto, route, and observer settings for the master and every target:

1. Label each result with physical unit identity, role/slot, master or target status, AVR/ESP versions, last time-set source, and elapsed time since that set.
2. Collect at least 12 samples and record median offset and spread.
3. For an outlier, collect a pre-set observation, set it once from the cellphone, then observe immediately, after 24 hours, and after several days.
4. Do not change its aging register until the drift series and original value have been preserved.
5. If possible, repeat a clone and observe the target immediately. An immediate multi-second error points toward the unverified write; gradual divergence points toward RTC drift.
6. If an offset jumps discretely while RTC readback remains steady, instrument DS3231 square-wave ticks, system time, reset cause, and schedule countdown separately.

Useful observer overrides:

```text
FLEXFOX_CLOCK_SAMPLES=30 FLEXFOX_CLOCK_TIMEOUT_MS=120000 just wifi-clock-observe
FLEXFOX_CLOCK_DRY_RUN=1 just wifi-clock-observe
```

## Candidate correction, after reproduction

Do not change the mature clock path until the measurements discriminate the failure:

- make the AVR clock setter return an explicit success/failure result;
- validate the input and read the RTC back after the next second edge;
- ACK only a verified write and NAK a failed or mismatching write;
- send the slave clock command through the ESP's normal Linkbus queue;
- wait for the specific clock response rather than inferring success from an empty queue;
- compare target readback with the master under a defined phase tolerance;
- validate event checksums independently of clock synchronization;
- decide explicitly whether aging calibration belongs to hardware identity or should be part of a clone.

The first product change began with a failing source contract and is recorded in [AVR clone synchronization controls](AVR_CLONE_SYNC_CONTROLS_2026-07-12.md). The ESP controls and hardware-compatible pinned build are recorded in [ESP clone synchronization controls](ESP_CLONE_SYNC_CONTROLS_2026-07-12.md). The remaining product gate is end-to-end qualification with a second updated unit.
