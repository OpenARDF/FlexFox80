# Linkbus Message-ID Encoding Characterization — 2026-07-12

## Status

Mac red-green, direct collision, repository, exact AVR Release build, connected-target programming, and live alias-rejection evidence is complete. Routine Windows duplication is not required under the established parity policy.

## Confirmed defect

The three-character length guard prevents 32-to-16-bit truncation, but the legacy message-ID calculation was itself non-unique:

```text
id = first * 100 + second * 10 + third
```

ASCII character values are much larger than decimal digits, so distinct labels can have the same weighted sum. Exhaustive uppercase checking found aliases for both read-only and state-changing handlers:

| Defined ID | Same-length aliases |
| --- | --- |
| `KEY` | `IYY`, `IZO`, `JOY`, `JPO`, `JQE`, `KFO`, `KGE` |
| `RST` | `RTJ`, `SIT`, `SJJ` |
| `GO` | `FY`, `HE` |
| `VER` | `TYR`, `TZH`, `UOR`, `UPH`, `VFH` |
| `TEM` | `RXW`, `RYM`, `RZC`, `SNW`, `SOM`, `SPC`, `TDW`, `TFC` |
| `BAT` | `AKT`, `ALJ`, `BBJ` |
| `WI` | `VS` |

For example, the old encoding gives both `KEY` and `IYY` value 8279. Length checking alone cannot distinguish them.

## Completed TDD slice

The firmware contract was extended first to require a 32-bit collision-free `LBMessageID` and collision-free receive accumulation. Against the decimal encoding, `just test` failed red with:

```text
Firmware contract check failed: Linkbus message IDs are not collision-free uint32_t values
```

The production correction encodes each one-to-three-character ID as its ordered bytes in a `uint32_t`. For example, `KEY` is `0x4B4559`, exactly the ASCII bytes `K`, `E`, `Y`. The receive parser uses the same left-shift-and-append helper as the enum definitions.

This is an internal representation change only:

- wire labels and delimiters are unchanged;
- every defined one-to-three-character command, reply, and query retains its existing text;
- the main dispatch continues to switch on `LBMessageID` values generated from those same characters;
- no EEPROM field, protocol payload, event setting, RF setting, or ESP source changes;
- each of the three AVR receive buffers grows by two bytes because its internal ID member is now four bytes.

Direct host tests prove the canonical `KEY`, `RST`, and `GO` byte encodings and prove known decimal aliases for `KEY`, `RST`, `GO`, and `TEM` are distinct. Existing ID-length, field-count, field-width, text-copy, EEPROM, and circular-buffer regressions remain green.

The constrained target test includes read-only `$RXW?`, which the old decimal encoding aliases to `TEM`. A legacy-alias response is visible over WebSocket as `TEMP`; the corrected parser instead returns a Linkbus NAK, which the ESP consumes without broadcasting. Two probes make one incidental periodic temperature update insufficient to fail the test, and a following battery query proves that parsing and ESP queue processing remain healthy.

## Exact Mac AVR build

Two consecutive final-source builds used AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103. Both completed with zero warnings and byte-identical hashes for all six artifacts:

```text
   text    data     bss     dec     hex
  40350    1106    1144   42600    a668
```

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `fd81c93ebdeea3651b4e74483b6a295d95bd4f93301e3665415793141dd688f3` |
| `FlexFox80.hex` | `12382531a84bbbd39ff7d180a8b759019b9fee48c379620a8b74eba3c33089d5` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `d97348e03758f862818ca589cd940248514078fcef5d2cdd7c492cef24d4e54a` |
| `FlexFox80.lss` | `df77c4a4cd045ea728a590b9e5161e1a4d3be4b921a9e96dae99572312dce9e4` |
| `FlexFox80.srec` | `cedc943f0670ada12f60fb97e28a88aace3b5885f17890fbb6b4365bec223098` |

Compared with the preceding ID-length build, text grows by 72 bytes and BSS by six bytes. Data is unchanged, and the EEPROM initializer remains byte-identical.

## Connected-target programming

The dummy-loaded AVR128DA48 test unit was programmed with the exact `1cb372a` Release HEX through the proven chip-erase, flash-write/verify, and complete EEPROM-restore/verify workflow.

Fresh pre-write captures matched the preserved unit baseline:

- EEPROM, 512 bytes: `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401`;
- fuses, 16 bytes: `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126`.

Avrdude verified all 41,182 input flash bytes twice. Independent post-operation reads then proved:

| Memory | Expected SHA-256 | Post-read SHA-256 | Result |
| --- | --- | --- | --- |
| Programmed flash bytes | `8e38a46062a9266d010e936ec4e34a470b780d1f8d4bdcf5a4dd2787b421869c` | `8e38a46062a9266d010e936ec4e34a470b780d1f8d4bdcf5a4dd2787b421869c` | byte-identical |
| Restored EEPROM | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

The first four-case Linkbus test attempt timed out at HTTP after programming, before its WebSocket opened or any test frame was sent. This matches the known Moto/DroidTether association loss after extended Atmel-ICE activity and is not counted as a firmware result.

## Connected-target qualification

After reconnecting the Moto/DroidTether route, the two field-boundary frames and overlength `$AZRX?` frame each remained unanswered through the ESP's retry cycle. Fresh temperature and battery replies then proved recovery and next-frame resynchronization.

An initial `$TYR?` timing assertion was invalidated during review: a syntactically valid unknown ID intentionally produces `!NAK;`, so correct rejection clears the ESP queue immediately rather than remaining unanswered. In addition, periodic `TEMP` telemetry could satisfy the old single-message recovery assertion. No firmware conclusion was drawn from those failed harness attempts.

The corrected target assertion used `$RXW?`, the legacy alias for `TEM`, and observed:

- two consecutive short alias-probe windows with no `TEMP` reply;
- a fresh `BAT,12.1V` reply after the probes;
- all prior bounds and resynchronization checks passing in the same session.

This directly proves that the programmed target no longer dispatches the legacy same-length temperature alias. The harness now requires both temperature and battery recovery replies after unanswered frames, so incidental periodic telemetry cannot satisfy that check by itself. No connected-target verification remains open for this slice.
