# Wireless Clone Time Synchronization Investigation

**Date:** 2026-07-12

**Path:** B-TIME-01

**Status:** Investigation active; no product firmware changed

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

## Ranked hypotheses and discriminating evidence

| Rank | Hypothesis | Why it fits | Observation that distinguishes it |
| --- | --- | --- | --- |
| 1 | RTC clock write failed but clone proceeded | Both ESP ACK handling and AVR I2C error reporting can falsely complete | Target is already far off immediately after a nominal clone; RTC readback disagrees with master's transmitted time |
| 2 | One RTC drifts faster because of oscillator quality or aging-register difference | One outlier after several days fits unit-specific drift; aging is queryable but is not cloned, and EEPROM `g_clock_calibration` is stored but not applied to the DS3231 | Unit begins close after setting, then offset changes approximately linearly; aging value differs from peers |
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

The first product change should be a narrow failing regression for the demonstrated mechanism, followed by exact AVR build and connected-target evidence. ESP changes remain gated on a pinned, reproducible ESP8266 build environment.
