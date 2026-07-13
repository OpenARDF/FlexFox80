# AVR RTC Edge Target Verification

**Date:** 2026-07-13

**Source commit:** `6abc0b8` (`Recover delayed AVR RTC edges`)

**Target:** Authorized dummy-loaded AVR128DA48 test unit

**Status:** Focused programming, runtime, forced-delay recovery, restoration, and readback gates pass

## Target identity

The read-only Atmel-ICE probe reported:

- Atmel-ICE serial `J41800053674`;
- firmware `1.42.161`;
- target voltage `3.31 V`;
- UPDI clock `100 kHz`;
- signature `1E 97 08` (`AVR128DA48`);
- silicon revision `1.7`.

## Pre-write preservation

Two independent EEPROM reads and two independent fuse reads were byte-identical. A complete pre-write flash read was also retained in the ignored target-evidence directory.

| Memory | Bytes | SHA-256 |
| --- | ---: | --- |
| EEPROM | 512 | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` |
| Fuses | 16 | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` |
| Previous flash programmed span | 41,754 | `629b6b5ce564da965ba9977fae6d0653ac74b1e100980859452a7888a35a85ab` |

The fuse image matches the established test-unit baseline. Raw unit-specific images remain untracked under `Software/AVR128DA48/tmp/hardware-probe/rtc-edge-2026-07-13/`.

## Programming and independent readback

The target was explicitly erased, programmed with the exact zero-warning Release artifact from commit `6abc0b8`, and verified twice by avrdude. The complete pre-write EEPROM image was then restored and verified twice. Fuses were not included in any write.

An independent post-operation read established:

| Memory | Intended SHA-256 | Post-read SHA-256 | Result |
| --- | --- | --- | --- |
| Candidate flash programmed span | `b8d7bb66de33a59ce57777734ef27cbf0e2ceda302f9b41fb3dacfffc499c3f8` | `b8d7bb66de33a59ce57777734ef27cbf0e2ceda302f9b41fb3dacfffc499c3f8` | byte-identical |
| Preserved EEPROM | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

The test unit therefore contains the intended candidate flash, its exact pre-test settings, and unchanged fuses.

## Production runtime baseline

After the Moto USB/tether path was restored, `just wifi-probe` passed HTTP, WebSocket, ESP identity, and fresh AVR temperature/battery data. Consecutive AVR clock reports advanced normally. A 12-sample read-only baseline reported:

```text
median_offset_ms=630.5 min_offset_ms=564 max_offset_ms=869 spread_ms=305
```

The offset includes WiFi, ESP, USB-tunnel, and host scheduling latency. It is recorded as a runtime sanity check rather than an absolute RTC-phase measurement.

## Isolated forced-delay method

A temporary characterization image was built from commit `169d45f` in an isolated `/private/tmp` copy. The repository worktree remained clean and the temporary code was never committed. The pinned AVR-GCC 7.3.0 and AVR-Dx DFP 1.9.103 build completed with zero warnings.

The temporary image added one exact `$TST,550` command. It:

1. acknowledged the command before starting the test;
2. suspended event activity and inhibited RF output in RAM;
3. held the normal-priority `TCB0` ISR while the existing Level-1 `TCB2` ISR continued to run;
4. released the normal-priority ISR after 550 `TCB2` periods.

At 24 MHz with `TCB2` clocked at peripheral-clock/2 through 65,536 counts, 550 periods are approximately 3.004 seconds. This directly exercises the interrupt-priority and port-edge-coalescing condition without disconnecting or electrically faulting the Si5351.

The test image's HEX SHA-256 was `63b5473e51c1bf5374e091964eae3f22e67c0ebfab64e3143fdfecf9947af15d`. The preserved EEPROM was restored and verified when that image was installed.

## Forced-delay result

The immediate pre-delay reports and command were:

```text
SYNC,1783985615
SYNC,1783985617
SYNC,1783985619
SYNC,1783985621
BASELINE median_offset_ms=777
SEND one-use 550-tick normal-priority ISR delay
```

While the normal-priority ISR was held, the ordinary `5623` report was suppressed. The first report after release was `5625`, followed by normal two-second progression:

```text
SYNC,1783985625
SYNC,1783985627
SYNC,1783985629
SYNC,1783985631
SYNC,1783985633
```

The four-second epoch advance from `5621` to `5625` across the blocked interval is the expected catch-up behavior. Without counted-edge replay, the system clock would have advanced only once when the coalesced port interrupt was finally serviced and the next report would have remained approximately two seconds behind.

The temporary firmware also queued an unsupported `!TST,<count>` diagnostic. The installed ESP did not surface that label to the WebSocket, so the first test harness timed out waiting for this auxiliary message. The pass determination does not rely on that missing label; it relies on the AVR clock sequence and the stable post-delay phase.

The 12-sample post-delay observation reported:

```text
median_offset_ms=698 min_offset_ms=636 max_offset_ms=1039 spread_ms=403
```

The median moved by only -79 ms from the immediate 777 ms pre-delay baseline. There was no persistent whole-second offset.

## Production restoration

The temporary image was immediately erased. The committed production artifact from `6abc0b8` and the complete preserved EEPROM were restored and verified. Independent post-test reads then matched:

| Memory | Expected SHA-256 | Final SHA-256 | Result |
| --- | --- | --- | --- |
| Production flash programmed span | `b8d7bb66de33a59ce57777734ef27cbf0e2ceda302f9b41fb3dacfffc499c3f8` | `b8d7bb66de33a59ce57777734ef27cbf0e2ceda302f9b41fb3dacfffc499c3f8` | byte-identical |
| Preserved EEPROM | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

This focused test verifies the counted-edge recovery mechanism on the target. It does not substitute for later A8 sleep/wake, RF-pattern, event-boundary, and long-duration regression, nor for separate A5 electrical Si5351/I2C fault-safety testing.
