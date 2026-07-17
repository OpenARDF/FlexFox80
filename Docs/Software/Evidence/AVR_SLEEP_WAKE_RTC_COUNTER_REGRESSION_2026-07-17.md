# AVR Sleep/Wake RTC Counter Regression — 2026-07-17

**Affected release:** FlexFox80 v1.0.0, AVR `0.201`

**Corrected development version:** AVR `0.203`

**Status:** Both release-blocking scheduler failures reproduced and corrected; source, host, contract, audit, deterministic Release-build, and one-unit dummy-loaded scheduled-event/sleep qualification pass; synchronized fleet rehearsal remains required

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

## First correction: standby RTC accounting

`rtcElapsedEdges()` now determines whether `TCB2` is actually enabled and uses the shared `rtcEdgeTrackerTakePortEdge()` path:

- sampler running: retain counted-edge observation and replay;
- sampler stopped: treat the DS3231 port interrupt itself as exactly one physical second without modifying either tracker counter;
- wake reinitialization: retain the existing `TIMERB_init()` reset that re-aligns the tracker before sampling resumes.

No event duration, Sprint 12/48-second timing, RF keying, EEPROM field, Linkbus wire value, or ESP behavior changed in this correction. The first corrected image was identified as AVR `0.202` so it could not be confused with the affected published AVR `0.201` image.

## Connected-target finding: expired pre-start sleep state

The first dummy-loaded `0.202` qualification exposed a second, independent scheduler defect. The ESP is intentionally allowed to remain powered while a WiFi station is associated so field programming is not interrupted. When the Moto remained associated across the scheduled start, the AVR commenced the event but retained `SLEEP_UNTIL_START_TIME`. After the Moto disconnected and the ESP's shutdown grace later expired, the ordinary WiFi shutdown path requested sleep using that stale, already-expired mode. The immediate RTC wake then reinitialized the running event's on-air state and rephased its Classic cycle.

This explains the live sequence in which the first Classic transmission was correct, the next expected cycle was absent, and a roughly one-minute transmission appeared about two minutes late after wireless shutdown. It is not RTC drift and is distinct from the `1,255` standby accounting failure.

AVR `0.203` retires the pre-start sleep mode in both places that consume the start deadline:

- the RTC pre-start wake changes `g_sleepType` to `DO_NOT_SLEEP` before launching the event; and
- the foreground scheduled-start path defensively makes the same transition before powering the transmitter.

WiFi association still justifiably delays ESP power-off. The change only prevents a later WiFi shutdown from reusing an expired sleep deadline during an event that is already running.

## Regression and neighboring audit

The direct host regression now applies 86,400 consecutive sleeping port wakes and requires exactly 86,400 elapsed software seconds while both tracker counters remain unchanged. It also verifies that the first physical edge after timer restart is counted once and leaves the tracker aligned.

The firmware source contract additionally requires both scheduled-start paths to retire `SLEEP_UNTIL_START_TIME` before the running-event state can be launched. This locks the connected-across-start failure sequence to the corrected behavior.

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
- Resource use: flash text 41,028 bytes; initialized data 1,112 bytes; BSS 1,572 bytes.
- EEPROM initializer remains byte-identical to the established 274-byte image.

Corrected candidate flash:

```text
68007a3f7976e689a545e5da9f8156eda321332921f4921835c5f099d1a30adf  FlexFox80.hex
```

## Connected-target qualification

The identified pilot (`Tx_7C2D6FD3`, MAC `86:A8:24:2F:96:5B`) was programmed through Atmel-ICE `J41800053674`. Independent readback verified AVR `0.203` flash, the preserved EEPROM image, and the untouched fuse image. Live telemetry reported `SW_VERSIONS,2.6,0.203` with advancing clock and normal temperature/battery reports.

A real Sprint event ran 12 seconds on / 48 seconds off for its complete eight-minute test window, crossing the original approximately five-minute premature-stop point. A main-power recovery while that event was active returned it to the correct assigned slot. A three-trial clock synchronization/readback test passed before the sleep qualification.

The final Classic test was scheduled for 7:32-7:42 pm, with a future Sprint event at 7:45 pm. The Moto was disconnected after the AVR returned receiving-data status `236` followed by waiting-for-start status `255`. External input-current observations were:

| Time | Current | Observation |
|---|---:|---|
| 7:24:30 | 1.7 mA | Pre-event standby after the intentional ESP-connected grace period |
| 7:31:50 | 40.6 mA | Scheduled wake, ten seconds before start |
| 7:32 | 971 mA | First Classic transmission |
| after 7:33 | 1.7 mA | Inter-transmission standby |
| 7:36:50 | 40.6 mA | Second scheduled wake |
| 7:37 | 960 mA | Second Classic transmission |
| after 7:38 | 40.4 mA | Deliberate final-cycle awake policy, which prevents event finish while asleep |
| 7:42 | 152 mA | Classic finish and ESP startup to retrieve the next event |
| 7:42:30 | 1.7 mA | Post-event standby with the future Sprint event scheduled |
| 7:44:50 | 40.5 mA | Wake ten seconds before that future event |

This passes pre-event sleep, exact scheduled wake/start, genuine inter-transmission sleep, repeated Classic timing, exact finish handling, and post-event sleep while another event remains scheduled. It also confirms that the connected-across-start correction prevents the delayed WiFi shutdown from rephasing the event.

After the test, Sprint2 and Sprint3 were restored to their exact original epochs, the temporary `Classic80m.event` file was deleted, and a refreshed event sheet confirmed only the original six event files. The pilot's original EEPROM was restored. Final independent verification passed for AVR `0.203` flash, original EEPROM, and original fuses; the two transient backup files were then deleted.

## Remaining fleet gate

The one-unit pilot is complete, but AVR `0.203` is not yet approved for Championship deployment. Program the same identified AVR/ESP pair into the ten-fox set, then run a complete synchronized future start, repeated cycle, extended-duration, exact finish, and cold-power recovery rehearsal before Championship use. A substantially longer soak remains valuable even though the pilot crossed the original five-minute failure point.
