# AVR Sleep/Wake RTC Counter Regression — 2026-07-17

**Affected release:** FlexFox80 v1.0.0, AVR `0.201`

**Corrected development version:** AVR `0.202`

**Status:** Root cause reproduced; source, host, contract, audit, and two deterministic Release-build gates pass; connected-target scheduled-event qualification remains required

## Field reproduction

Ten Sprint transmitters were synchronized to a master and left powered without a post-sync power cycle. They were expected to wake and begin a roughly 24-hour event at 4:30 pm. None transmitted at the scheduled start. Cycling power caused all ten to transmit in sync, but they stopped far earlier than the programmed finish. Repeating the power cycle repeated the temporary recovery.

The common, deterministic behavior across all ten units and the reset-dependent recovery identified the shared AVR scheduler as the primary failure domain.

## Root cause

AVR `0.201` added a Level-1 `TCB2` sampler that counts DS3231 rising edges while ordinary foreground and normal-priority interrupt work may be delayed. That recovery is valid while the sampler is running.

The existing standby path deliberately disables `TCB2`. The sleeping AVR still wakes from the DS3231 `PORTA` interrupt once per physical second, but the pre-correction RTC handler continued to consume the sampler's `observed - serviced` counters:

1. On the first sleeping wake, no sampled edge exists, so the fallback returns one second and advances only `serviced`.
2. On the next physical wake, `observed - serviced` is an unsigned eight-bit negative difference and wraps to `255`.
3. The `PORTA` ISR therefore executes the complete one-second scheduler body 255 times during one physical second.
4. The same `1, 255, 1, 255...` sequence repeats while the sampler remains disabled.

The hardware wake source still fires once per second. The regression is that AVR `0.201` converts alternating sleeping wakes into 255 software seconds. This advances system time, event boundaries, wake decisions, on/off countdowns, WiFi timeouts, and finish handling far faster than real time. It explains both missing a future start after a long sleep and temporary recovery after a power cycle re-synchronizes system time to the RTC.

## Why prior verification missed it

The original RTC edge-recovery tests covered delayed normal-priority service, counter wrap, and a single fallback after timer restart. The focused target test kept `TCB2` running while blocking the normal-priority ISR. Neither test exercised two consecutive DS3231 port wakes while `TCB2` was stopped.

The v1.0.0 hardware disposition explicitly deferred complete scheduled-start, scheduled-finish, sleep/wake, energized Sprint-cycle, and long-duration event tests. The focused edge-recovery result was valid for its tested awake condition but was not sufficient evidence for the untested standby condition.

## Correction

`rtcElapsedEdges()` now determines whether `TCB2` is actually enabled and uses the shared `rtcEdgeTrackerTakePortEdge()` path:

- sampler running: retain counted-edge observation and replay;
- sampler stopped: treat the DS3231 port interrupt itself as exactly one physical second without modifying either tracker counter;
- wake reinitialization: retain the existing `TIMERB_init()` reset that re-aligns the tracker before sampling resumes.

No event duration, Sprint 12/48-second timing, RF keying, sleep policy, EEPROM field, Linkbus wire value, or ESP behavior changed. The corrected image is identified as AVR `0.202` so it cannot be confused with the affected published AVR `0.201` image.

## Regression and neighboring audit

The direct host regression now applies 86,400 consecutive sleeping port wakes and requires exactly 86,400 elapsed software seconds while both tracker counters remain unchanged. It also verifies that the first physical edge after timer restart is counted once and leaves the tracker aligned.

The audit rechecked every AVR change between the prior `0.200` baseline and v1.0.0 that can affect transmitting at the right time or for the right duration:

- RTC boot alignment, delayed-edge recovery, bounded RTC synchronization, and event boundary decisions;
- atomic event-start, event-finish, wake-time, and on-air stores;
- standby timer shutdown and wake reinitialization;
- Sprint role intervals and offsets;
- Linkbus ID/field parsing for all event-programming commands;
- EEPROM enum layout, RF-power width, event timing fields, and fox-array bounds;
- transmitter and Si5351 source changes; and
- ESP-to-AVR event transaction ordering and current acknowledged limitations.

No second hardening regression that changes valid event start, finish, Sprint interval, frequency, power, or RF enablement was found. The existing AVR protocol still acknowledges some commands without proving that the requested hardware setting was semantically applied, and event parameters are not atomically staged on the AVR. Those are pre-existing protocol limitations, not causes of this regression; broadening this safety-critical correction to redesign that protocol would increase immediate risk.

## Verification completed

- `just check`: pass, including host sanitizers, schedule boundaries, EEPROM layout, Linkbus bounds, ESP event transaction tests, and firmware source contracts.
- Pinned AVR-GCC `7.3.0` / AVR-Dx_DFP `1.9.103` Release build: `reference-version-match`, zero warnings.
- Two clean Release builds: all six artifact hashes byte-identical.
- Resource use: flash text 41,020 bytes; initialized data 1,112 bytes; BSS 1,572 bytes.
- EEPROM initializer remains byte-identical to the established 274-byte image.

Corrected candidate flash:

```text
7070bce580223a138328522e3bd5434457be56c4d358e82778685343e54ed63a  FlexFox80.hex
```

## Required connected-target gate

AVR `0.202` is not approved for the Championship fleet until a dummy-loaded pilot passes all of the following using a future scheduled event without a post-programming or post-sync power cycle:

1. enter standby and remain there long enough to cross multiple RTC wakes;
2. wake before the exact scheduled start and begin at the programmed epoch;
3. execute repeated Sprint 12-second-on / 48-second-off cycles in correct slots;
4. continue across an extended soak without compressed countdowns or premature finish;
5. stop at the exact scheduled finish;
6. pass a cold-power recovery run with the same retained event;
7. preserve EEPROM and fuses, verify the programmed flash by independent readback, and confirm AVR `0.202` in live telemetry.

Only after that pilot passes should the same identified AVR/ESP pair move to the ten-fox set, followed by a complete synchronized start/cycle/finish rehearsal before Championship use.
