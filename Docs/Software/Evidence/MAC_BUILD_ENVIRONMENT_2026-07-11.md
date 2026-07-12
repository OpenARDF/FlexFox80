# Mac Build Environment Evidence — 2026-07-11

## Host

- Operating system: macOS 26.5.2
- Architecture: Apple silicon (`arm64`)
- Rosetta x86_64 translation: available

## AVR compiler

Microchip's archived macOS AVR 8-bit Toolchain 3.7.0 was retrieved from the official AVR-GCC archive page. The macOS package has a platform-specific build number even though it provides the same GCC version used by the Windows project:

- archive: `avr8-gnu-toolchain-osx-3.7.0.518-darwin.any.x86_64.tar.gz`
- archive size: 52,111,217 bytes
- archive SHA-256: `378b210cc82dc06599b5a45dede0ed188a9e87f2de045f12cf3547554edd6ec8`
- `avr-g++ --version`: `avr-g++ (AVR_8_bit_GNU_Toolchain_3.7.0_518) 7.3.0`
- extracted `avr-g++` architecture: Mach-O x86_64
- extracted `avr-g++` SHA-256: `6a04fb06256b30b0b199471994e39bd290503dc4591dcf7c6e92d90666babf2c`

The compiler starts successfully through Rosetta. The archive and extracted toolchain remain temporary local inputs and were not added to the repository.

## Device-pack boundary

The project requires the Atmel-namespaced `AVR-Dx_DFP` 1.9.103 pack selected by Microchip Studio 7. The current Microchip pack repository contains a separately versioned Microchip lineage, while the legacy Atmel repository lists the required historical release but was not reachable from this Mac during the check.

No substitute pack was relabeled or treated as equivalent. Consequently, `just avr-build` has not yet produced a reference artifact on macOS.

The most direct next input is the installed Windows pack directory, or the original `Atmel.AVR-Dx_DFP.1.9.103.atpack`, copied from the preserved Microchip Studio environment. Once available locally, set:

```text
AVR_TOOLCHAIN_ROOT=/path/to/avr8-gnu-toolchain-darwin_x86_64
AVR_DFP_ROOT=/path/to/AVR-Dx_DFP/1.9.103
```

Then run `just avr-build` twice and compare `Software/AVR128DA48/tmp/avr-release/build-evidence.json` and all artifact hashes.
