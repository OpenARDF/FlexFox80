# Windows g_fox Verification - 2026-07-12

Source commit tested: `9c9dade` (`Development_AVR128DA48`)

This verification was run after rebasing the local Windows evidence-only commits onto `origin/Development_AVR128DA48` at Mac request commit `3f13ef4`. No firmware source changes beyond `9c9dade` were included in the test build inputs.

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

Each run was a clean AVR Release wrapper build. Outputs were copied to ignored snapshots under `Software\AVR128DA48\tmp\g-fox-verification` only for local comparison and were not committed.

## Warning result

Both clean builds completed with zero warnings.

The three pre-fix `src/eeprommanager.cpp` `-Warray-bounds` warnings are gone:

```text
src/eeprommanager.cpp:642:25: warning: array subscript is above array bounds [-Warray-bounds]
src/eeprommanager.cpp:747:26: warning: array subscript is above array bounds [-Warray-bounds]
src/eeprommanager.cpp:748:76: warning: array subscript is above array bounds [-Warray-bounds]
```

No remaining or new warnings were reported by the wrapper.

## `avr-size` output

Both runs produced:

```text
   text    data     bss     dec     hex filename
  40170    1106    1137   42413    a5ad C:\Users\charl\Documents\GitHub\FlexFox80\Software\AVR128DA48\tmp\avr-release\FlexFox80.elf
```

## Artifact hashes

The pre-fix baseline hashes are from `Docs/Software/Evidence/WINDOWS_BUILD_ENVIRONMENT_2026-07-12.md`.

| File | Run 1 SHA-256 | Run 2 SHA-256 | Run hashes match | Matches pre-fix baseline |
| --- | --- | --- | --- | --- |
| `FlexFox80.elf` | `4145129d38abf96e4a527a911fd27891b57ec34c52aa12c2022779af03d91e0d` | `4145129d38abf96e4a527a911fd27891b57ec34c52aa12c2022779af03d91e0d` | Yes | Yes |
| `FlexFox80.hex` | `e35b1a3fa60480c8acde67afba78fc49d5a83d7813a2a90cb0953c8d2e18c39a` | `e35b1a3fa60480c8acde67afba78fc49d5a83d7813a2a90cb0953c8d2e18c39a` | Yes | Yes |
| `FlexFox80.eep` | `905808d6fb3c3d7e490386b6a5c613afb9c49c488c6047d9c94eb84bb8efde09` | `905808d6fb3c3d7e490386b6a5c613afb9c49c488c6047d9c94eb84bb8efde09` | Yes | Yes |
| `FlexFox80.lss` | `5e81d42507721b759e25b09e06d75cb943576d982b1497d8f6dc3e61a2e4f9e7` | `5e81d42507721b759e25b09e06d75cb943576d982b1497d8f6dc3e61a2e4f9e7` | Yes | Yes |
| `FlexFox80.map` | `24e737fe5ed1802e5de52980e9a3f1e98b168c5990560e5e065c799a76f32e82` | `24e737fe5ed1802e5de52980e9a3f1e98b168c5990560e5e065c799a76f32e82` | Yes | Yes |
| `FlexFox80.srec` | `f666b131c504899294266a7b9f7cedc8f649e773dd2938cc47c045cb82bca1cf` | `f666b131c504899294266a7b9f7cedc8f649e773dd2938cc47c045cb82bca1cf` | Yes | Yes |

## Conclusion

The `g_fox` declaration correction removes the three AVR-GCC array-bounds diagnostics while preserving deterministic output. Corresponding artifacts from the two clean post-fix builds match each other, and all six requested artifacts also match the pre-fix Windows baseline hashes.
