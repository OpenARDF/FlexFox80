# Mac AVR Programming Evidence — 2026-07-12

## Scope

- Branch: `Development_AVR128DA48`
- Firmware source commit: `57d70a7` (`Stabilize EEPROM enum field widths`)
- Documentation commit at programming time: `5a99660`
- Programmer: Atmel-ICE over UPDI at 100 kHz
- Target: AVR128DA48 test unit, signature `1E 97 08`

The user authorized this connected FlexFox as a writable test unit, including deliberate EEPROM changes, provided its original settings were restored when practical.

## Selected artifact

The image was produced by the exact Mac Release wrapper with AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103:

- HEX SHA-256: `0ef047f859bc9d6aa2bcd8b6b717feaecbf98e77c8b73f70d8e836c6ac9dbe5a`
- warning count: `0`
- linker `.eeprom` size: `0x112` (274 bytes)
- build evidence: [EEPROM_ENUM_WIDTH_ABI_2026-07-12.md](EEPROM_ENUM_WIDTH_ABI_2026-07-12.md)

The preserved EEPROM showed an initialized schema, an `EVENT_NONE` low byte, and an event window that ended on 2026-05-31. This removed the immediate scheduled-transmission risk on reboot, but did not replace later RF safety testing.

## Stable pre-write state

Fresh reads immediately before programming matched the earlier captures exactly:

| Memory | Bytes | SHA-256 |
| --- | ---: | --- |
| EEPROM | 512 | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` |
| Fuses | 16 | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` |

The unit-specific raw captures remain in the ignored `Software/AVR128DA48/tmp/hardware-probe/` directory and were not committed.

## Failed no-chip-erase attempt

The first flash-only command used `-D` to disable avrdude's automatic chip erase. It wrote the requested pages but verification correctly failed:

```text
Warning: flash verification mismatch
  device 0x02 != input 0x52 at addr 0x0036 (error)
```

This is expected when a replacement image requires a flash bit to transition from 0 to 1. EEPROM and fuses were not included in that command, but the flash was a mixed, unqualified image after the failure.

The operational rule is therefore: do not use `-D` alone as a general EEPROM-preservation strategy for arbitrary replacement firmware. Preserve EEPROM first, perform the required erase, and restore EEPROM explicitly if retention is not independently guaranteed.

## Recovery and final state

Recovery was performed immediately in one Atmel-ICE session:

1. explicit chip erase;
2. Release flash write;
3. implicit and explicit flash verification;
4. write the exact pre-test raw EEPROM image;
5. explicit EEPROM verification.

Avrdude reported both flash verification passes for all 41,010 input bytes. Independent post-operation reads then established:

| Memory | Expected SHA-256 | Post-read SHA-256 | Result |
| --- | --- | --- | --- |
| Programmed flash binary | `c1a27ec13c031e7654764ebe742b3edaf2e8572952973c14aa3ce60969569f41` | `c1a27ec13c031e7654764ebe742b3edaf2e8572952973c14aa3ce60969569f41` | byte-identical |
| Restored EEPROM | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

The test unit therefore runs the intended `57d70a7` Release flash with its complete original EEPROM image restored and its fuses unchanged.

## Qualification boundary

This proves programming, flash identity, EEPROM restoration, and fuse preservation. It does not yet prove UI communications, RTC behavior, RF output, antenna interlock, event timing, sleep/wake, or long-duration reliability. Exact Windows same-source build evidence and controlled functional hardware tests remain open.
