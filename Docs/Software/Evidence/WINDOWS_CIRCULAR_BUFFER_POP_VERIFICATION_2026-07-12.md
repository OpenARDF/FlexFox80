# Windows Circular Buffer Pop Verification - 2026-07-12

Source commit tested: `2e2e94c` (`Development_AVR128DA48`)

This verification was run on `Development_AVR128DA48` at mailbox request commit `4466572`, which includes the circular-buffer `pop()` correction from `2e2e94c`.

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

Each run was a clean AVR Release wrapper build. Outputs were copied to ignored snapshots under `Software\AVR128DA48\tmp\circular-buffer-pop-verification` only for local comparison and were not committed.

## Warning result

Both clean builds completed with zero warnings. No remaining or new warnings were reported by the wrapper.

## `avr-size` output

Both runs produced:

```text
   text    data     bss     dec     hex filename
  40170    1106    1137   42413    a5ad C:\Users\charl\Documents\GitHub\FlexFox80\Software\AVR128DA48\tmp\avr-release\FlexFox80.elf
```

The resource totals are unchanged from the prior Windows `g_fox` verification: `text=40170`, `data=1106`, `bss=1137`, `dec=42413`, `hex=a5ad`.

## Artifact hashes

| File | Run 1 SHA-256 | Run 2 SHA-256 | Run hashes match | Compared with prior Windows baseline |
| --- | --- | --- | --- | --- |
| `FlexFox80.elf` | `350dd82a6015d7955a3676d10c3809b304caac2d41d7fdcd516462b8241a9c7a` | `350dd82a6015d7955a3676d10c3809b304caac2d41d7fdcd516462b8241a9c7a` | Yes | Changed |
| `FlexFox80.hex` | `c44c2bf36b9b0b1423eeb988319b979eff934a4feaf7cb28e20507d2b8a5291c` | `c44c2bf36b9b0b1423eeb988319b979eff934a4feaf7cb28e20507d2b8a5291c` | Yes | Changed |
| `FlexFox80.eep` | `905808d6fb3c3d7e490386b6a5c613afb9c49c488c6047d9c94eb84bb8efde09` | `905808d6fb3c3d7e490386b6a5c613afb9c49c488c6047d9c94eb84bb8efde09` | Yes | Unchanged |
| `FlexFox80.lss` | `6aa7dc7a62839930e6279d8037da738d269f28d984999a774997498720c6f3a3` | `6aa7dc7a62839930e6279d8037da738d269f28d984999a774997498720c6f3a3` | Yes | Changed |
| `FlexFox80.map` | `24e737fe5ed1802e5de52980e9a3f1e98b168c5990560e5e065c799a76f32e82` | `24e737fe5ed1802e5de52980e9a3f1e98b168c5990560e5e065c799a76f32e82` | Yes | Unchanged |
| `FlexFox80.srec` | `0e5c5db328c6e87e82d4ed5ab9d8bed36aeee31c77d542db5256307a88853e2f` | `0e5c5db328c6e87e82d4ed5ab9d8bed36aeee31c77d542db5256307a88853e2f` | Yes | Changed |

The prior Windows baseline referenced here is the post-`g_fox` verification in `Docs/Software/Evidence/WINDOWS_G_FOX_VERIFICATION_2026-07-12.md`. The executable-bearing artifacts changed as expected for an intentional behavior fix. EEPROM and linker-map output remained byte-identical.

## `just check`

`just check` passed with the documented Windows shims and sanitizer setting:

```powershell
$env:PATH="Software\AVR128DA48\tmp\check-shims;C:\Program Files\LLVM\bin;C:\Program Files\Git\bin;C:\Program Files\Git\usr\bin;$env:PATH"
$env:HOST_TEST_SANITIZERS='0'
just check
```

The check run included:

```text
PASS initial_state_is_empty
PASS fifo_storage_is_uppercased
PASS full_buffer_overwrites_oldest_entry
PASS indices_wrap_without_changing_fifo_order
PASS reset_clears_data_and_busy_state
PASS pop_returns_entries_in_lifo_order
All AVR circular buffer characterization tests passed
PASS g_fox declaration covers every Event_t value
```

## Conclusion

The circular-buffer `pop()` correction builds cleanly in the Windows AVR reference environment, produces deterministic repeatable artifacts, keeps the AVR memory footprint unchanged, and passes the Windows host regression suite with the new LIFO `pop()` test.
