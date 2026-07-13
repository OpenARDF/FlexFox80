# AVR RTC Edge Target Verification

**Date:** 2026-07-13

**Source commit:** `6abc0b8` (`Recover delayed AVR RTC edges`)

**Target:** Authorized dummy-loaded AVR128DA48 test unit

**Status:** Programming identity and preserved-state gates pass; functional WiFi/RTC gate is waiting for the Moto USB link

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

## Functional gate boundary

The first post-program `just wifi-probe` timed out. The Mac still had the intended host route through `utun4`, and DroidTether processes were running, but the Moto was absent from `adb devices`. The existing DroidTether session log recorded an earlier HTTP 200 response through the same route. This result therefore identifies an unavailable Moto USB/tether leg; it does not establish an AVR or ESP startup failure.

Once the Moto is visible to the Mac again and USB tethering is enabled, repeat the read-only WiFi/AVR probe. Then qualify ordinary clock progression and a controlled delay spanning multiple RTC edges. No RF-path or physical I2C fault should be introduced until the read-only runtime gate passes.
