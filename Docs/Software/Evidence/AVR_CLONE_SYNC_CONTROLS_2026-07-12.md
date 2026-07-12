# AVR Clone Synchronization Controls

**Date:** 2026-07-12

**Path:** B-TIME-01

**Status:** AVR implementation and target identity verified; live protocol behavior pending WiFi re-association

## Scope

This slice reduces clone-time jitter and removes false clock-write success on the AVR side without changing the established RTC phase model. Absolute UTC accuracy is not the objective: cloned units need the same repeatable second boundary and delay.

The ESP-to-AVR control protocol adds three backward-compatible `$ESP` fields:

- `C`: enter clone quiet mode and suppress ordinary time, error, status, and next-event reports;
- `S`: arm one time report at the next DS3231 square-wave edge;
- `R`: resume normal reports.

The RTC ISR captures the one-shot epoch and sets a foreground flag. It does not transmit over Linkbus. The main loop services that flag immediately after received Linkbus messages. Quiet mode has a 900-second AVR fail-safe and is also explicitly resumable.

Clone clock writes use `$TIM,<ISO>,C;`. The DS3231 setter now reports whether all seven bytes were written. After a successful write, the AVR waits for its existing RTC edge synchronization, reads the RTC back, and replies `!TIM,C,<epoch>;`. A failed write or read produces `!NAK;`; the ordinary ACK follows only a successful operation. Legacy `$TIM,<ISO>;` writes remain supported.

The Linkbus ACK decision was also moved inside the receive loop. Previously, one frame that disabled its ACK could accidentally suppress ACKs for later frames processed in the same call.

## Red-green evidence

The dependency-free firmware checker first failed because the AVR had no `C/S/R` quiet controls. After implementation it requires:

- quiet controls and fail-safe state;
- RTC-edge arming with no Linkbus transmission inside the ISR;
- quiet gating for unsolicited reports;
- per-frame ACK state;
- a boolean DS3231 write result;
- clone-specific RTC epoch readback.

`just test` passes all host tests, firmware contracts, and the 274-byte EEPROM-layout contract.

## Exact Mac build

The repository Release wrapper used AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103. It completed with zero warnings:

```text
   text    data     bss     dec     hex
  40924    1112    1569   43605    aa55
```

The EEPROM section remains exactly `0x112` bytes. The generated HEX SHA-256 is `3f409e1d58ac972f5cc105934714e299376a28fe3bf1bde8f74e3b438b27f5e9`.

## Connected target programming evidence

Fresh pre-write captures matched the established unit baselines:

- EEPROM: `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401`;
- fuses: `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126`.

The authorized dummy-loaded AVR128DA48 test unit was explicitly erased, programmed, and flash-verified twice. All 512 preserved EEPROM bytes were restored and verified. Independent post-operation reads prove:

| Memory | Expected SHA-256 | Post-read SHA-256 | Result |
| --- | --- | --- | --- |
| Programmed flash bytes | `59cb852dd6f8a03f7bceffb34e25315ee5f4fb121bf6e3a7abf5b6b83e59c45f` | `59cb852dd6f8a03f7bceffb34e25315ee5f4fb121bf6e3a7abf5b6b83e59c45f` | byte-identical |
| Restored EEPROM | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

The Moto/DroidTether host route remained installed after programming, but the Moto had lost its route onward to the FlexFox AP. No HTTP or WebSocket connection opened and no protocol test frame was sent. Live assertions for quiet suppression, one-shot edge reporting, verified readback, and explicit resume therefore remain open and must not be inferred from programming success.
