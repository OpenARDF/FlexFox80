# Bounded AVR Text Send Characterization — 2026-07-12

## Status

Mac red-green, repository, host-boundary, and exact AVR Release build evidence is complete. Exact Windows and connected-target Linkbus verification remain open.

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

## Remaining verification

- Obtain exact Windows same-source builds and the full Windows host-contract run.
- Program the connected dummy-loaded test unit through the proven flash/EEPROM-preservation workflow.
- Confirm the normal WiFi-to-AVR query/reply path still returns temperature, battery, version, and identity data.
