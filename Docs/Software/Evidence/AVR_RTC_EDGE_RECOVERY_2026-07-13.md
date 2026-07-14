# AVR RTC Edge Recovery

**Date:** 2026-07-13

**Baseline:** `db66786` (`Document AVR clock-edge timing risks`)

**Status:** Source, host-test, firmware-contract, exact-build, and focused target forced-delay gates pass

## Defect addressed

The DS3231 one-second square wave drives a rising-edge `PORTA` interrupt. That interrupt previously called `system_tick()` once and then cleared the port flags. A normal-priority interrupt can spend more than one second retrying Si5351 I2C while the RTC interrupt is blocked. Multiple physical RTC edges then coalesce into one port flag, so the AVR system clock advances only once and remains one or more whole seconds behind the RTC.

This is a discrete tick-loss mechanism, not oscillator drift and not ordinary report-delivery latency. The static call-path evidence is recorded in [AVR clock-edge and jitter audit](AVR_CLOCK_EDGE_AND_JITTER_AUDIT_2026-07-13.md).

## Red-green checkpoint

The first `just test` run included the new RTC edge-tracker test before its source helper existed. It failed at compile time with:

```text
fatal error: 'rtc_edge_tracker.h' file not found
```

The completed host test now covers:

- the port ISR running before the Level-1 sampler sees the same edge;
- the sampler running before the port ISR;
- recovery of three edges while the port ISR is blocked;
- eight-bit counter wrap;
- timer shutdown/restart around sleep.

## Conservative implementation

The change reuses `TCB2`, the existing 183 Hz Level-1 I2C-timeout timer. It samples the RTC SQW input and increments an eight-bit rising-edge counter. The normal-priority RTC handler consumes the difference between observed and serviced counters and runs the existing one-second body once per elapsed edge.

Important boundaries:

- every shared tracker field is one byte and therefore transfers atomically on the AVR;
- the RTC handler samples the live input while briefly masking only `TCB2` interrupts, closing the narrow same-pin port-flag read/clear race without globally disabling interrupts;
- a `TCB2` compare flag raised while masked remains pending and is serviced when its interrupt is restored;
- port flags are cleared before the existing slow work, so a later edge remains pending;
- an armed clone one-shot captures the newest recovered second, not the oldest;
- `TCB2` remains disabled in sleep and the tracker is realigned when the existing timer initialization runs after wake;
- no timer, peripheral clock, or event-system user was enabled in standby;
- no RF keying, Si5351 retry, Linkbus wire format, EEPROM layout, or sleep policy was changed.

The eight-bit difference preserves as many as 255 coalesced edges. A continuous blockage of exactly 256 seconds or more is outside this bounded recovery and remains an A5 watchdog/fault-recovery concern.

## Verification

`just check` passes, including all existing host suites, repository policy checks, firmware contracts, and EEPROM layout checks. The exact AVR release build also passes with zero warnings using the pinned Mac toolchain and AVR-Dx DFP.

| Resource | Baseline `db66786` | Candidate | Delta |
| --- | ---: | ---: | ---: |
| Flash text | 40,916 bytes | 41,084 bytes | +168 bytes |
| Initialized data | 1,112 bytes | 1,112 bytes | 0 |
| BSS | 1,569 bytes | 1,572 bytes | +3 bytes |

The generated EEPROM artifact is byte-identical to baseline:

```text
c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906  FlexFox80.eep
```

Candidate artifact hashes:

```text
96210ef9df396e8ba5dc038e6045fc28def2a42fc58421bd26fffb403a23138c  FlexFox80.elf
75f2ff0b489a3f2fc3191de986c2b88e5b9de0d5f7a5a8dbf9f975c3595fe1dd  FlexFox80.hex
c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906  FlexFox80.eep
a76bed06b09503c41d1a16588e62b2c945806f79f81706435517945ef04448f2  FlexFox80.map
c874f9fec70bd5d03afae7dd4249bee3dab9ec08baf7664fc269e18d782aed6e  FlexFox80.lss
092044ed11c6d048409d106b9eb47f3172ba199445442a7ac0640ca2598e134f  FlexFox80.srec
```

## Target gate result

The dummy-loaded target passed ordinary WiFi/AVR clock progression and an isolated approximately 3.004-second normal-priority ISR blockage. The first post-block report caught up four epoch seconds, normal two-second progression resumed, and the post-test receive-offset median moved only -79 ms. Production flash was then restored with byte-identical independent readback, complete EEPROM restoration, and unchanged fuses. Details and limitations are recorded in [AVR RTC edge target verification](AVR_RTC_EDGE_TARGET_VERIFICATION_2026-07-13.md).

This focused gate verifies the edge-recovery mechanism but does not claim that this mechanism caused the field outlier. Broader sleep/wake, RF-pattern, event, and long-duration regression remains in A8; electrical Si5351/I2C fault-safety work remains in A5.
