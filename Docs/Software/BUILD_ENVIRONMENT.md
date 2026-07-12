# FlexFox80 Build Environment

## Build strategy

macOS is the primary environment for source review, host-side checks, hardening, and ordinary development. The preserved Windows installation of Microchip Studio 7 remains the reference environment for exact AVR compiler equivalence and hardware programming until the Mac workflow has been compared with it.

The repository must distinguish three kinds of evidence:

- A **reference AVR build** uses AVR-GCC 7.3.0 and Microchip `AVR-Dx_DFP` 1.9.103, matching `FlexFox80.cppproj` and the historical generated Debug Makefile.
- An **exploratory AVR build** uses another compiler or device-pack version to expose portability problems. It is useful evidence, but it cannot establish release equivalence.
- A **hardware-qualified build** is a reference or otherwise approved build that has also passed the applicable UPDI programming and transmitter tests.

## AVR Release build

The portable wrapper is `scripts/build-avr-release.mjs`. It reproduces the Release configuration recorded in the Microchip Studio project without invoking the IDE. It writes only to the ignored directory `Software/AVR128DA48/tmp/avr-release/` and records warnings, size output, and artifact hashes in `build-evidence.json`.

Set these environment variables before running `just avr-build`:

- `AVR_TOOLCHAIN_ROOT`: directory containing `bin/avr-g++` and the companion AVR binary utilities;
- `AVR_DFP_ROOT`: unpacked `AVR-Dx_DFP/1.9.103` directory containing `include/` and `gcc/dev/avr128da48/`.

The wrapper accepts Windows `.exe` tools and native Unix tools. It rejects compiler or device-pack version drift by default. `AVR_ALLOW_VERSION_MISMATCH=1` permits an explicitly labeled exploratory build; such output must not be treated as the baseline or flashed as a release merely because it compiles.

The checked-in `avr8-gnu-toolchain-3.7.0.1796-win32.any.x86_64.zip.update` archive contains a Windows toolchain and cannot provide native macOS executables. Its retention and eventual replacement should be decided only after both environments reproduce the firmware.

The exact archived macOS compiler runs through Rosetta, and the historical Atmel `AVR-Dx_DFP` 1.9.103 pack has now been retrieved from the official archive. Two exact Mac Release builds are deterministic, warning-free, and match the established Windows resource totals. See the initial [Mac build environment evidence](Evidence/MAC_BUILD_ENVIRONMENT_2026-07-11.md) and completed [Mac AVR reference-build evidence](Evidence/MAC_AVR_REFERENCE_BUILD_2026-07-12.md).

### EEPROM ABI gate

The deployed Debug image stores six enum-valued configuration members with a two-byte representation, while the Release configuration correctly enables `-fshort-enums`. Those persisted members therefore use explicit `uint16_t` storage rather than compiler-dependent enum types. Every reference Release build must pass the repository layout contract, the AVR compile-time 274-byte assertion, and show a `.eeprom` size of `0x112` in the linker map. A 268-byte Release layout is incompatible with retained deployed EEPROM and must not be flashed. See [EEPROM enum-width ABI evidence](Evidence/EEPROM_ENUM_WIDTH_ABI_2026-07-12.md).

## Mac Atmel-ICE target access

The preserved Atmel-ICE and an AVR128DA48 FlexFox can be accessed from this Mac with Homebrew avrdude. Run `just avr-probe` for a no-write identity and voltage probe. The command enters UPDI programming mode and may briefly reset a running transmitter even though `-n` prevents writes.

The first live probe confirmed the expected debugger serial and AVR128DA48 signature and captured read-only flash, EEPROM, and fuse evidence. See [Mac Atmel-ICE target evidence](Evidence/MAC_ATMEL_ICE_TARGET_EVIDENCE_2026-07-12.md). Raw device images remain under the ignored `Software/AVR128DA48/tmp/` tree and must not be committed.

The connected FlexFox is authorized as a writable test unit. Before the first write, preserve its current flash, EEPROM, and fuses; select an exact, schema-compatible artifact; and define the rollback and applicable hardware test plan. Do not alter fuses or lock bits unless a separate test explicitly requires it.

## Windows reference-environment handoff

The Windows Codex environment can provide high-value evidence without becoming the primary development environment. Requests and replies travel through the repository-root [Codex mailbox](../../CODEX_MAILBOX.md). It should run read-only discovery first and commit no generated IDE output. The requested report should include:

1. Microchip Studio 7 exact version and installation path.
2. `avr-g++.exe -dumpversion` and the enclosing toolchain directory.
3. Installed `AVR-Dx_DFP` versions and the pack path used by this project.
4. Clean Release build output, all warnings, `avr-size` output, and SHA-256 hashes for ELF, HEX, EEP, LSS, MAP, and SREC files.
5. Whether two consecutive clean Release builds produce identical hashes; if not, which files or sections differ.
6. The programmer/debugger model, UPDI clock, fuse settings, and exact programming procedure.
7. Arduino IDE or CLI version, ESP8266 board-package version, selected board/options, installed `arduinoWebSockets` version, filesystem upload tool/version, and ESP build/flash/filesystem usage output.
8. The identity and hashes of firmware believed to be deployed on known reliable units, if those artifacts are available.

The report should be added under `Docs/Software/Evidence/` in a later narrow commit, accompanied only by small text logs or hashes. Do not commit a second set of binaries or rewrite the historical tracked Debug directory.

## ESP8266 status

The ESP source identifies the Adafruit Huzzah ESP8266 and depends on the ESP8266 Arduino core, LittleFS, and `arduinoWebSockets`. The exact known-good core, library, board-option, filesystem, and upload-tool versions are not recorded in the repository. Pinning guessed current versions would create a new build, not reproduce the mature product.

Mac-side source inspection and host tests may proceed, but the first pinned ESP build definition should wait for the Windows environment inventory or other known-good version evidence. Once supplied, Step A2 will add a clean `arduino-cli` build and filesystem-image workflow.
