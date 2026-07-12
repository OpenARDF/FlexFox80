# Mac Atmel-ICE and Connected FlexFox Evidence — 2026-07-12

## Scope and safety boundary

The user made an AVR128DA48 FlexFox available through an Atmel-ICE connected to this Mac. This pass used only signature, voltage, flash-read/verify, EEPROM-read, and fuse-read operations. It did not erase, program, write EEPROM, change fuses or lock bits, or select an artifact for deployment.

Entering and leaving UPDI programming mode may briefly reset normal transmitter execution even during a no-write probe.

## Debugger and target identity

- USB product: `Atmel-ICE CMSIS-DAP`
- USB vendor/product IDs: `03EB:2141`
- Atmel-ICE serial: `J41800053674`
- Atmel-ICE firmware: `1.42.161`
- Atmel-ICE hardware revision: `0`
- Target voltage observed: `3.25–3.30 V`
- Probe clock: `100 kHz` UPDI
- Target signature: `1E 97 08` (`AVR128DA48`)
- Silicon revision reported by avrdude: `1.7`
- Mac tools: avrdude `8.1`; pymcuprog `3.19.4.61`

`pymcuprog ping` independently returned `1E9708`, and its voltage command measured `3.28 V`.

## Deployed flash identity

Avrdude read 44,788 programmed bytes before erased trailing flash and wrote a local raw image:

- local ignored file: `Software/AVR128DA48/tmp/hardware-probe/flash-2026-07-12.bin`
- byte count: `44,788`
- SHA-256: `b3b1f6be8806dae59c71a4984066ee0747aa5022f239ca4ccf01a256d7c2dc93`

A separate avrdude verify-only operation compared all 44,788 bytes successfully with:

- tracked artifact: `Software/AVR128DA48/FlexFox80/Debug/FlexFox80.hex`
- tracked HEX SHA-256: `0680c750df9fb19854cb16808c45a10f7fbd284626d006c1c227d43a99362a70`
- artifact source commit: `33e64e075ae8ceb09e49ffe8f95e2201cad1315a` (`Bug Fixes`, 2024-05-06)

The connected unit is therefore running the tracked historical Debug HEX exactly. This does not qualify it as a Release build.

## EEPROM and fuse capture

Two separate avrdude reads produced byte-identical 512-byte EEPROM images:

- local ignored files: `eeprom-2026-07-12.bin` and `eeprom-avrdude-repeat-2026-07-12.bin`
- SHA-256 for each: `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401`

The raw image was not committed because it can contain station identity, unlock configuration, event data, and other unit-specific state.

Target-DWARF-backed inspection of non-text fields found:

- initialization flag at `0x000`: bytes `08 01` (`0x0108`);
- RF power at `0x09D`: bytes `B8 0B` (`3000 mW`);
- following `guard4_15` at `0x09F`: bytes `00 FF FF FF`;
- I2C failure counter at `0x110`: bytes `FF FF`.

The I2C counter has no producer or increment path outside `EepromManager` in the current source, so the erased value is presently an unused persisted diagnostic rather than an active control input. The late-layout byte pattern is real and repeatable, but this evidence does not prove whether it arose from initialization behavior, preserved EEPROM history, or another older write path.

The 16-byte fuse read was saved only in the ignored probe directory:

- SHA-256: `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126`
- raw bytes by fuse index: `00 00 F0 FF FF D3 F8 00 00 FF FF FF FF FF FF FF`

No semantic fuse interpretation or change is asserted in this pass.

## Tool discrepancy

Pymcuprog successfully identified the debugger and target but returned an all-erased EEPROM image. Its decoded EEPROM differed from the two identical avrdude reads in 166 bytes. Because both avrdude captures were deterministic and contained structured configuration matching the deployed firmware, pymcuprog's EEPROM result is not accepted as evidence without a known-compatible device pack and further tool validation.

## Repeatable no-write probe

Run:

```text
just avr-probe
```

The recipe performs an avrdude no-write signature/voltage probe. Device-image capture remains a deliberate evidence operation rather than a routine check.
