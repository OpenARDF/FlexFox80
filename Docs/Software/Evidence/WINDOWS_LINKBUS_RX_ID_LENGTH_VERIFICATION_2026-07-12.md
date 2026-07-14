# Windows Linkbus Message-ID Length Verification - 2026-07-12

Source commit tested: `912d24b` (`Development_AVR128DA48`)

This verification was run after fast-forwarding `Development_AVR128DA48` to mailbox request commit `a4daaa3`, which includes the Linkbus message-ID length bound and the request in `CODEX_MAILBOX.md`.

## Build environment

- Host: Windows 11 ARM64 VM, PowerShell 7 shell.
- AVR toolchain root: `C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain`
- Compiler: `avr-g++.exe (AVR_8_bit_GNU_Toolchain_3.7.0_1796) 7.3.0`
- `avr-g++.exe -dumpversion`: `7.3.0`
- Device pack root: `C:\Program Files (x86)\Atmel\Studio\7.0\packs\atmel\AVR-Dx_DFP\1.9.103`
- Wrapper result for both runs: `reference-version-match`

Build command:

```powershell
$env:AVR_TOOLCHAIN_ROOT='C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain'
$env:AVR_DFP_ROOT='C:\Program Files (x86)\Atmel\Studio\7.0\packs\atmel\AVR-Dx_DFP\1.9.103'
node .\scripts\build-avr-release.mjs
```

Each run was a clean AVR Release wrapper build. Outputs were copied to ignored snapshots under `Software\AVR128DA48\tmp\linkbus-rx-id-length-verification` only for local comparison and were not committed.

## Warning result

Both clean builds completed with zero warnings. No remaining or new warnings were reported by the wrapper.

## EEPROM layout evidence

The linker map for both runs reports `.eeprom` size `0x112`.

Run 1 and run 2 both show:

```text
.eeprom        0x0000000000810000      0x112
 .eeprom       0x0000000000810000      0x112 ...\src\eeprommanager.o
               0x0000000000810000                _ZN13EepromManager7ee_varsE
               0x0000000000810112                __eeprom_end = .
```

`EepromManager::ee_vars` starts at `0x810000` and `__eeprom_end` is `0x810112`, so the span is exactly `0x112` bytes, or 274 bytes.

## `avr-size` output

Both runs produced:

```text
   text    data     bss     dec     hex filename
  40278    1106    1138   42522    a61a C:\Users\charl\Documents\GitHub\FlexFox80\Software\AVR128DA48\tmp\avr-release\FlexFox80.elf
```

This matches Mac's reported resource totals for the same source: `text=40278`, `data=1106`, `bss=1138`, `dec=42522`, `hex=a61a`.

## Artifact hashes

| File | Run 1 SHA-256 | Run 2 SHA-256 | Run hashes match | Mac same-source comparison |
| --- | --- | --- | --- | --- |
| `FlexFox80.elf` | `041dfca4934fcd6de1fed543279bf7c8ee3cfe6b8ecffec00ea6fdba1ca8af8c` | `041dfca4934fcd6de1fed543279bf7c8ee3cfe6b8ecffec00ea6fdba1ca8af8c` | Yes | Differs from Mac `ff8f03587cc341623cc408a6dcfc8e8452bfc2c7b93d523c64241d7fa99ab7f6` |
| `FlexFox80.hex` | `44254d519cf8e3bc2dae82765ea9827672485d70470467b20db612fa5a8eb36c` | `44254d519cf8e3bc2dae82765ea9827672485d70470467b20db612fa5a8eb36c` | Yes | Matches Mac |
| `FlexFox80.eep` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` | Yes | Matches Mac |
| `FlexFox80.lss` | `f47018b74b552a41011f4e26dab0d3f7ceb8203a8a7875dc136d9e2512fd2d61` | `f47018b74b552a41011f4e26dab0d3f7ceb8203a8a7875dc136d9e2512fd2d61` | Yes | Differs from Mac `f883b7780081842892b49705f9f6f2ad0f5b61b66e033f251a0bb06693936b36` |
| `FlexFox80.map` | `152abd6820b001f1bbfd7ce9c8a2bef2aaf916b3e4205a593112a84cd86ffb50` | `152abd6820b001f1bbfd7ce9c8a2bef2aaf916b3e4205a593112a84cd86ffb50` | Yes | Differs from Mac `f446633a51037ff9c88a1b8eaaa940ba5161067d0e6e664860b790c07ff7e4cb` |
| `FlexFox80.srec` | `3a3ada5ac32e17071cbd3839bc03060c40363e0e5382c37a01051fd10d497ce9` | `3a3ada5ac32e17071cbd3839bc03060c40363e0e5382c37a01051fd10d497ce9` | Yes | Differs from Mac `a6def97511e186c3577de7c9a80522057c06ed56d3d8c175ccfc30c81ad64be7` |

The Windows runs are internally deterministic for every requested artifact. The Intel HEX and EEPROM outputs match Mac byte-for-byte, confirming the same flash payload and EEPROM initializer for the source snapshot. ELF, MAP, and LSS are host-sensitive because they include local paths and tool-output formatting. SREC is deterministic on Windows but does not match Mac; because same-source HEX matches exactly and the resource totals match, this appears limited to generated S-record representation rather than a firmware payload difference.

## `just check`

`just check` passed with the documented Windows shims and sanitizer setting:

```powershell
$env:PATH="Software\AVR128DA48\tmp\check-shims;C:\Program Files\LLVM\bin;C:\Program Files\Git\bin;C:\Program Files\Git\usr\bin;$env:PATH"
$env:HOST_TEST_SANITIZERS='0'
just check
```

The run included the requested message-ID regression and accumulated checks:

```text
PASS message_id_is_limited_to_three_characters
All AVR Linkbus receive bounds tests passed
PASS g_fox declaration covers every Event_t value
PASS I2C failure count writes preserve the uint16_t EEPROM width
PASS RF power writes preserve the uint16_t EEPROM width
PASS generated driver include paths are host-portable
PASS WiFi smoke probe remains read-only
PASS text send helpers copy literal data within destination bounds
PASS Linkbus receive parser guards ID length, field count, and field length
PASS Linkbus live test remains limited to approved malformed frames and read-only recovery
PASS all 65 EEPROM field offsets match the 274-byte AVR layout
```

The same run also passed the circular-buffer and bounded text-copy host suites.

## Conclusion

The Linkbus message-ID length bound builds warning-free in the Windows AVR reference environment, keeps the EEPROM layout at exactly 274 bytes, produces deterministic Windows artifacts, matches Mac's same-source HEX and EEP hashes, and passes the requested Windows host and source-contract checks.
