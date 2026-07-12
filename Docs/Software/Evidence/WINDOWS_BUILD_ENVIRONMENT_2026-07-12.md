# Windows Build Environment Evidence - 2026-07-12

Source commit tested: `4d17bab` (`Development_AVR128DA48`)

## Host and IDE

- Host: Windows 11 ARM64 VM, PowerShell 7 shell.
- Microchip Studio installation path: `C:\Program Files (x86)\Atmel\Studio\7.0`
- Microchip Studio registry entry: `Microchip Studio Development Environment` version `7.0.2594`, publisher `Atmel`, install date `20250329`.
- Microchip Studio executable: `C:\Program Files (x86)\Atmel\Studio\7.0\AtmelStudio.exe`
- `AtmelStudio.exe` file version: `1.0.0.2`
- `AtmelStudio.exe` product version: `7.0.0.4033`
- AVR8 Device Support registry entry: version `7.0.2557`.
- AVR8 Toolchain registry entry: version `7.0.2522`.

## AVR toolchain

- Toolchain root: `C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain`
- Compiler: `C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-g++.exe`
- `avr-g++.exe --version`: `avr-g++.exe (AVR_8_bit_GNU_Toolchain_3.7.0_1796) 7.3.0`
- `avr-g++.exe -dumpversion`: `7.3.0`
- `avr-g++.exe` SHA-256: `35B10D511BCEFD2DAF16108CDC38D88B86CC3FF20202CB9563319D5D64894374`

Other `avr-g++.exe` installs found, not used for the reference build:

- `C:\Program Files\Microchip\xc8\v2.36\avr\bin\avr-g++.exe`
- `C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain-backup-5.4.0\bin\avr-g++.exe`
- `C:\Users\charl\AppData\Local\Arduino15\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino7\bin\avr-g++.exe`

## AVR-Dx device pack

- Pack root used by this project: `C:\Program Files (x86)\Atmel\Studio\7.0\packs\atmel\AVR-Dx_DFP\1.9.103`
- Other installed version: `C:\Program Files (x86)\Atmel\Studio\7.0\packs\atmel\AVR-Dx_DFP\1.10.114`
- Transfer ZIP, outside repository: `C:\Users\charl\Documents\FlexFox80-transfer\Atmel.AVR-Dx_DFP.1.9.103.WindowsCodex.2026-07-12.zip`
- Transfer ZIP size: `6293003` bytes
- Transfer ZIP SHA-256: `F5E685EFA7549EF365FC55A146C9DD8E27E858CF6193C289D2CB0D2B72965AE7`

The ZIP was not added to the repository.

## AVR Release build

Build environment:

```powershell
$env:AVR_TOOLCHAIN_ROOT='C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain'
$env:AVR_DFP_ROOT='C:\Program Files (x86)\Atmel\Studio\7.0\packs\atmel\AVR-Dx_DFP\1.9.103'
node .\scripts\build-avr-release.mjs
```

`just avr-build` was not used for these two runs because this shell initially could not find `sh`; the Node wrapper was invoked directly with the same environment variables.

Both runs reported `reference-version-match`.

### Warnings

Both clean Release runs produced the same three warnings:

```text
src/eeprommanager.cpp:642:25: warning: array subscript is above array bounds [-Warray-bounds]
src/eeprommanager.cpp:747:26: warning: array subscript is above array bounds [-Warray-bounds]
src/eeprommanager.cpp:748:76: warning: array subscript is above array bounds [-Warray-bounds]
```

### `avr-size` output

Both runs produced:

```text
   text    data     bss     dec     hex filename
  40170    1106    1137   42413    a5ad C:\Users\charl\Documents\GitHub\FlexFox80\Software\AVR128DA48\tmp\avr-release\FlexFox80.elf
```

### Artifact hashes

| File | Run 1 SHA-256 | Run 2 SHA-256 | Match |
| --- | --- | --- | --- |
| `FlexFox80.elf` | `4145129d38abf96e4a527a911fd27891b57ec34c52aa12c2022779af03d91e0d` | `4145129d38abf96e4a527a911fd27891b57ec34c52aa12c2022779af03d91e0d` | Yes |
| `FlexFox80.hex` | `e35b1a3fa60480c8acde67afba78fc49d5a83d7813a2a90cb0953c8d2e18c39a` | `e35b1a3fa60480c8acde67afba78fc49d5a83d7813a2a90cb0953c8d2e18c39a` | Yes |
| `FlexFox80.eep` | `905808d6fb3c3d7e490386b6a5c613afb9c49c488c6047d9c94eb84bb8efde09` | `905808d6fb3c3d7e490386b6a5c613afb9c49c488c6047d9c94eb84bb8efde09` | Yes |
| `FlexFox80.lss` | `5e81d42507721b759e25b09e06d75cb943576d982b1497d8f6dc3e61a2e4f9e7` | `5e81d42507721b759e25b09e06d75cb943576d982b1497d8f6dc3e61a2e4f9e7` | Yes |
| `FlexFox80.map` | `24e737fe5ed1802e5de52980e9a3f1e98b168c5990560e5e065c799a76f32e82` | `24e737fe5ed1802e5de52980e9a3f1e98b168c5990560e5e065c799a76f32e82` | Yes |
| `FlexFox80.srec` | `f666b131c504899294266a7b9f7cedc8f649e773dd2938cc47c045cb82bca1cf` | `f666b131c504899294266a7b9f7cedc8f649e773dd2938cc47c045cb82bca1cf` | Yes |

Corresponding hashes match for all requested outputs.

## Programmer and debugger metadata

The checked-in Microchip Studio project records:

- Active AVR tool: `com.atmel.avrdbg.tool.atmelice`
- Tool name: `Atmel-ICE`
- Tool serial: `J41800053674`
- Interface: `UPDI`
- Atmel-ICE `UpdiClock`: `500000`
- Project-level `avrtoolinterfaceclock`: `750000`
- Additional recorded tool entry: `nEDBG`, serial `MCHP3280041800002565`, `UPDI`, `UpdiClock` `750000`
- Expected device signature: `0x1E9708`
- `preserveEEPROM`: `true`
- `CacheFlash`: `true`
- `ProgFlashFromRam`: `true`
- `eraseonlaunchrule`: `0`
- `ResetRule`: `0`
- `BootSegment`: `2`

No explicit fuse byte values or normal operator programming procedure were found in the checked-in project files. No hardware programmer was queried during this evidence pass.

## ESP8266 / Arduino environment

- Arduino IDE executable: `C:\Program Files\Arduino IDE\Arduino IDE.exe`
- Arduino IDE version: `2.3.5`
- Arduino IDE build date from `package.json`: `2025-04-02T13:16:49.885Z`
- Bundled `arduino-cli`: `C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe`
- `arduino-cli version`: `arduino-cli Version: 1.2.0 Commit: 9c495211 Date: 2025-02-24T15:57:34Z`
- `arduino-cli.exe` SHA-256: `1B32BA44BFD00CE207026EEE1DE431F77BAA749DD54D9E2518C3676E77B7240C`
- `arduino-cli config dump`: `{}`
- `arduino-cli core list`: only `arduino:avr` `1.8.6` installed.
- `arduino-cli lib list`: `No libraries installed.`
- `arduino-cli core search esp8266`: `No platforms matching your search.`
- `arduino-cli board listall huzzah`: no board FQBN found.

The ESP source identifies the hardware target as `Adafruit HUZZAH ESP8266` in `Software/Huzzah/ARDF_Transmitter/ARDF_Transmitter.ino` and includes `LittleFS.h`, `WebSocketsServer.h`, and `WebSocketsClient.h`. This Windows profile does not currently contain a known-good ESP8266 Arduino core, selected board-option values, installed `arduinoWebSockets`/WebSockets library, LittleFS upload tool, or normal compile/upload/filesystem commands. Those items still need operator evidence or an older Arduino installation/profile if exact reproduction is required.

The Arduino CLI probe downloaded and installed its missing built-in discovery helper tools under the Arduino user data area. No repository files were changed by that probe.

## Known-good deployed artifacts

No reliable deployed AVR or ESP artifact provenance was available in this pass. The repository contains historical tracked Debug outputs, but this report does not claim that those files are deployed or known-good artifacts.
