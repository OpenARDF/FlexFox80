# Bounded AVR Text Send Characterization — 2026-07-12

## Status

Mac red-green, repository, host-boundary, exact AVR Release build, connected-target programming, post-program live Linkbus, and accumulated exact Windows evidence is complete.

## Confirmed defect

Both `lb_send_text()` and `serialbus_send_text()` copied caller-supplied text with:

```text
sprintf(*buff, text);
```

The input was therefore interpreted as a format string. A percent sequence could read unintended arguments, and neither call constrained output to its fixed destination:

- `LinkbusTxBuffer`: 50 bytes including the terminating NUL;
- `SerialbusTxBuffer`: 41 bytes including the terminating NUL.

Current Linkbus callers produce replies that fit the 50-byte buffer. The longest identified station-ID reply is at most 27 bytes including its protocol terminator, newline, and terminating NUL. Long Serialbus output already passes through `sb_send_string()`, which divides it into payloads of at most 40 characters.

## Completed TDD slice

The firmware source contract was extended first. Against the pre-fix implementation, `just test` failed red for exactly the unsafe helpers:

```text
Firmware contract check failed: Linkbus and Serialbus text send treats data as a format string
```

The production correction introduces one inline `copy_text_to_buffer()` helper and uses it in both send paths. It:

- copies percent characters as ordinary data;
- accepts at most `capacity - 1` payload bytes and always copies the terminating NUL;
- rejects null pointers and zero-capacity destinations;
- rejects oversized input before changing the destination.

Oversized Linkbus text is rejected rather than truncated because truncation could remove `;` or `?` and place a partial protocol frame on the wire. The send helpers retain their existing return convention: `false` only when a complete message has been queued, and `true` on failure.

The direct host regression compiles the same helper used by the AVR sources and covers literal `%s%n`, exact-capacity termination, oversized rejection without destination mutation, and invalid arguments. A source contract additionally requires both bus helpers to pass their actual destination size to the bounded helper.

`just check` passes with AddressSanitizer and UndefinedBehaviorSanitizer enabled for the host tests.

## Exact Mac AVR build

Two consecutive final-source builds used AVR-GCC 7.3.0 and Atmel `AVR-Dx_DFP` 1.9.103. Both completed with zero warnings and byte-identical hashes for all six artifacts:

```text
   text    data     bss     dec     hex
  40204    1106    1137   42447    a5cf
```

| Artifact | SHA-256 |
| --- | --- |
| `FlexFox80.elf` | `80ea448b9ad68168a1cb45c13335c6052363b8b384a49735eee75a23e7ee9ad5` |
| `FlexFox80.hex` | `a957b6585a7ee0faf1618715dca0eaadc25fe4129e9aade61e343c011bc97725` |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| `FlexFox80.map` | `7f07892541d7e4d72edba0d041323bff71152992c2e37f23cae7e54087850108` |
| `FlexFox80.lss` | `ca1bffa374f4b0ae75100bb44cb29f6648601ac05b423064916db501e7000732` |
| `FlexFox80.srec` | `4f340796107225284f3c14e62fa89601096d2045b1e04598e510358c600826f4` |

Compared with commit `4dbd90f`, text grows by 30 bytes; data and BSS are unchanged. The EEPROM initializer is byte-identical.

## Connected-target programming

The dummy-loaded AVR128DA48 test unit was programmed with the exact `b543f3f` Release HEX through the proven chip-erase, flash-write/verify, and complete EEPROM-restore/verify workflow.

Fresh pre-write captures matched the preserved unit baseline:

- EEPROM, 512 bytes: `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401`;
- fuses, 16 bytes: `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126`.

Avrdude verified all 41,036 input flash bytes twice. Independent post-operation reads then proved:

| Memory | Expected SHA-256 | Post-read SHA-256 | Result |
| --- | --- | --- | --- |
| Programmed flash bytes | `dc518c591970e7e7913a10865306632fbea311773c2c4071877b676ed471eb14` | `dc518c591970e7e7913a10865306632fbea311773c2c4071877b676ed471eb14` | byte-identical |
| Restored EEPROM | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | `b9a912cf6dd81c9a7ca73c9a098efcf37bc1e12ee44e60ee45d65a7fa9844401` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

The first post-program WiFi probe timed out at HTTP while the Mac host route still pointed to DroidTether `utun6`. The Moto/DroidTether path was restarted without changing the firmware or device state. The next probe then passed:

- HTTP 200 from `http://73.73.73.73/`;
- WebSocket connection to `ws://73.73.73.73:81/`;
- live AVR `TEMP,30.0C` and `BAT,12.3V` replies;
- ESP identity `SSID,Tx_7C2D69ED` and `MAC,1A:0D:BB:2E:2C:4C`;
- combined versions `SW_VERSIONS,2.0,0.200` and `MASTER,0`;
- continuing `SYNC` and battery broadcasts beyond the ESP's approximately ten-second socket timeout while the five-second read-only heartbeat monitor ran.

The transport-only timeout is therefore not counted as a firmware failure. Normal WiFi-to-AVR query/reply behavior is qualified on the programmed image without issuing configuration, RF, clock-set, reset, or EEPROM-changing commands.

## Windows verification

Two exact Windows builds of the accumulated `3bc10a5` snapshot completed deterministically with zero warnings and passed the bounded-copy host regression and source contract. Windows and Mac resource totals, HEX, and EEPROM outputs match. See [Windows accumulated hardening verification](WINDOWS_ACCUMULATED_HARDENING_VERIFICATION_2026-07-12.md).
