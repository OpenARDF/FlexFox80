# FlexFox80 Source and Generated-File Boundaries

## Purpose

This inventory prevents routine firmware work from accidentally rewriting generated Microchip files, committing IDE state, discarding required project inputs, or hiding intentional CAD/manufacturing assets.

## Hand-maintained AVR product source

The following are primarily product-maintained and may be changed with the verification required by [CODEX_WORKFLOW.md](CODEX_WORKFLOW.md):

- `Software/AVR128DA48/FlexFox80/main.cpp`
- `Software/AVR128DA48/FlexFox80/defs.h`
- most product-specific files under `Software/AVR128DA48/FlexFox80/src/`
- most product-specific files under `Software/AVR128DA48/FlexFox80/include/`

Exceptions and generated-derived files are listed below.

## Generated-derived or vendor-managed AVR files

Treat these as regeneration-sensitive:

- `atmel_start.cpp`
- `atmel_start.h`
- `Config/`
- `modules/`
- `utils/`
- `include/atmel_start_pins.h`
- `include/port.h`
- low-level START-generated peripheral headers and sources
- `FlexFox80.cppproj`
- `FlexFox80.componentinfo.xml`
- `Default.xml`
- generated `Debug/Makefile` and future `Release/Makefile`

`driver_isr.cpp` has a generated header but also contains product-specific USART and Linkbus parsing. It is a customized generated-derived file. A necessary edit must be narrow, retain its license/header, and be documented so a later START regeneration does not silently erase it.

`include/driver_init.h` has one portability correction from the generated Windows-style `utils\\compiler.h` include to the C/C++ portable `utils/compiler.h` spelling. Preserve that correction if Microchip START regenerates the file; changing the separator does not change the selected header or firmware behavior.

Low-level product adaptations in otherwise generated-style files should be identified during Step A2 before any START regeneration is attempted.

## AVR build outputs and IDE state

The repository currently tracks historical Debug output and IDE state, including:

- `.vs/.../*.atsuo`
- `Debug/*.o`
- `Debug/*.d`
- `Debug/*.elf`
- `Debug/*.hex`
- `Debug/*.eep`
- `Debug/*.map`
- `Debug/*.lss`
- `Debug/*.srec`
- `Debug/makedep.mk`
- `Debug/src/` build products

The tracked KiCad `fp-info-cache` is also generated cache data. Its 2022 history contains a vendor PDF URL whose `hkey` query parameter triggers the generic API-key detector. `.gitleaksignore` suppresses only that exact historical fingerprint; it does not suppress the file, query parameters, or generic-key rule broadly.

These are generated artifacts, not hand-maintained source. Adding ignore rules does not remove already tracked files. Reproducible wrappers now exist, but do not untrack historical artifacts until a dedicated cleanup commit evaluates their remaining evidence value.

Generated release images should eventually be published as release assets with hashes rather than committed as ordinary source.

## Hand-maintained ESP8266 source and assets

The following are product-maintained:

- `Software/Huzzah/ARDF_Transmitter/ARDF_Transmitter.ino`
- `Software/Huzzah/ARDF_Transmitter/Event.*`
- `Software/Huzzah/ARDF_Transmitter/Transmitter.*`
- `Software/Huzzah/ARDF_Transmitter/Helpers.*`
- `Software/Huzzah/ARDF_Transmitter/Blinkies.*`
- `Software/Huzzah/ARDF_Transmitter/CircularStringBuff.*`
- `Software/Huzzah/ARDF_Transmitter/esp8266.h`
- HTML, default settings, icons, and event fixtures under `Software/Huzzah/ARDF_Transmitter/data/`

The web assets and example event files are runtime inputs, not generated build output.

## KiCad and manufacturing files

- `.kicad_sch`, `.kicad_pcb`, `.kicad_pro`, symbol libraries, footprint libraries, Gerbers, drill files, placement files, and intentional BOM/manufacturing packages are project assets.
- `.kicad_prl` is local/session-oriented KiCad metadata but is currently tracked by project choice. Do not silently untrack or ignore it.
- `*-backups/` directories are local automatic backups and should be ignored.
- Intentional ZIP packages under `KiCad/Gerber/` are manufacturing deliverables. A global `*.zip` rule can hide future packages and should be replaced by path-specific temporary/toolchain rules.
- Legacy `.lib` files under `KiCad/` are real KiCad symbol/cache/rescue assets. Never import a global `*.lib` ignore rule from a software-only repository.

## Documentation and workflow files

Files under `Docs/Software/`, plus `.gitattributes`, `.editorconfig`, `Justfile`, and repository scripts, are hand-maintained sources of truth. Keep the root `README.md` user-facing unless a user-visible release or product instruction requires a change.

## Safe cleanup sequence

1. Establish ignore and text/binary rules without untracking files.
2. Establish reproducible AVR and ESP builds.
3. Classify any generated-derived exceptions needed by the wrappers.
4. Compare regenerated outputs with the historical tracked artifacts.
5. Propose an exact untrack list and confirm release-asset retention.
6. Commit artifact cleanup separately from firmware changes.
7. Verify a clean checkout still builds and packages both processors.
