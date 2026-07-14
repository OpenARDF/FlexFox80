# FlexFox80 v1.0.0 Installed-Pair Verification

**Date:** 2026-07-14

**Hardware:** FlexFox Ver 2.1 (Mar 2022), dummy load, Atmel-ICE `J41800053674`, FT232R `ABAKJ3Y8`

**Result:** Pass — the exact AVR `0.201` and ESP `2.1` release-candidate images are independently verified on the representative master, its unit configuration is preserved, and the running pair reports `SW_VERSIONS,2.1,0.201`.

## Selected artifacts

| Processor | Artifact | SHA-256 |
| --- | --- | --- |
| AVR128DA48 | `Software/AVR128DA48/tmp/avr-release/FlexFox80.hex` | `897076608a3473d48c0226dfd8800f5ac058e553286a2f97450bfd03a77f1ba4` |
| ESP8266 | `Software/Huzzah/tmp/esp-build/ARDF_Transmitter.ino.bin` | `0749b0e493254d526cb7a5e8afd938bd65b04c22bfffaa6c5150614f6315bad8` |

These are the two warning-free, deterministic images selected at clean candidate commit `10dd7c200bc1d63a13b174b2833b452801ae57a8`. Later release-record commits did not change firmware source.

## ESP installation

The removable HUZZAH identified as MAC `44:17:93:0f:09:3e` and 4 MiB flash. A full pre-release backup happened to be completed before the owner clarified that routine full-device backups are optional:

- bytes: 4,194,304;
- SHA-256: `a2be7dfa650951f4c1e325d05ed84a3fc152e813058486f831cd3aeff9dedc1d`;
- an independent full-device `verify_flash` returned `verify OK (digest matched)`.

The release sketch was then written only at `0x0`. The 1 MiB LittleFS region was not erased or replaced. The write's immediate hash check passed, and a separate manual bootloader entry plus independent comparison of all 503,824 sketch bytes returned:

```text
verify OK (digest matched)
```

After normal RESET, the operator observed the expected LED pattern and normal SSID advertisement.

## AVR installation

The Atmel-ICE probe reported AVR128DA48 signature `1E 97 08`, silicon revision 1.7, and 3.29 V target voltage. Two complete pre-write EEPROM reads were byte-identical:

| Region | Bytes | SHA-256 |
| --- | ---: | --- |
| Configured EEPROM | 512 | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` |
| Fuses | 16 | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` |

The EEPROM hash matches the already-qualified master configuration used during clone and timing work. Per the updated policy, no full prior AVR flash backup was made.

Programming used an explicit erase, wrote the selected AVR HEX, verified flash twice, restored all 512 EEPROM bytes, verified EEPROM twice, and did not write fuses. A new programmer session then independently read flash, EEPROM, and fuses. The raw expected image and device readback were byte-identical:

| Region | Expected SHA-256 | Readback SHA-256 | Result |
| --- | --- | --- | --- |
| Used AVR flash, 41,848 bytes | `5a7791083dc112e036e160d09964ef1d66293452e056d4282eb7a0b9dfaf95cf` | `5a7791083dc112e036e160d09964ef1d66293452e056d4282eb7a0b9dfaf95cf` | Exact |
| Complete EEPROM | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | Exact |
| Fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | Unchanged |

## Installed-pair result

With the HUZZAH reinstalled and the unit running, `just wifi-probe` passed through the Moto route:

```text
PASS HTTP 200 http://73.73.73.73/
PASS WebSocket connected ws://73.73.73.73:81/
RECV TEMP,25.0C
RECV BAT,11.2V
RECV SSID,Tx_Master
RECV MAC,22:C8:8E:CF:AB:84
RECV SW_VERSIONS,2.1,0.201
RECV MASTER,1
PASS WiFi-to-AVR read-only path returned temperature and battery data
```

Successive `SYNC` epochs advanced normally. The exact EEPROM before/after comparison, sketch-only ESP write, retained `Tx_Master` identity and MAC, `MASTER,1`, normal telemetry, and combined version report establish that the expected master configuration and both processor communication paths remain intact. No event, assignment, time, RF, or filesystem mutation was needed for this release gate.

## Scope

This closes the v1.0.0 representative installed-version gate. The separately approved A8 deferrals remain recorded in the release hardware disposition; this focused result does not silently convert them into passes.
