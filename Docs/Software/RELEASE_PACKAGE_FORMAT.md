# FlexFox80 Release Package Format

## Purpose

This document defines the maintenance-release package contract adapted from SignalSlinger's manifest, checksum, validation, and plain-language packaging practices. FlexFox differs because it is a mature legacy product maintained for an established user base, and because one release contains independently versioned AVR128DA48 and ESP8266 firmware plus an ESP LittleFS image that is not safe to treat as an ordinary firmware update.

FlexFox releases support exactly one hardware target: **Ver 2.1 (Mar 2022)**. Do not generate parallel board variants or imply compatibility with prototypes, earlier PCB revisions, or later unqualified hardware.

Automation is desirable but not mandatory. A release package may initially be assembled manually from frozen deterministic outputs, provided a separate check verifies its structure, manifest, byte sizes, and hashes and records that evidence. A package builder or validator should be added only when its repeat value justifies the maintenance cost.

## Release and embedded versions

Each release records three distinct identities:

- an owner-approved product release label and annotated Git tag such as `vX.Y.Z`;
- the AVR `SW_REVISION` reported by the running AVR;
- the ESP `WIFI_SW_VERSION` reported by the running ESP.

The product release label must not be inferred from either processor's version. The release manifest and live candidate must agree on the combined response `SW_VERSIONS,<ESP>,<AVR>`.

## Package name and contents

Use a release directory and ZIP name that expose all three identities:

```text
FlexFox80-<release>-AVR-<avr-version>-ESP-<esp-version>/
FlexFox80-<release>-AVR-<avr-version>-ESP-<esp-version>-Release-Files.zip
```

The validated ZIP contains exactly:

- `FlexFox80-AVR-<avr-version>.hex` — AVR program flash for Atmel-ICE/UPDI installation;
- `FlexFox80-ESP-<esp-version>.bin` — ESP sketch image written at address `0x00000000`;
- `FlexFox80-LittleFS-<esp-version>.bin` — exact release filesystem baseline, clearly marked as recovery/factory material rather than a routine update;
- `FlexFox80-Release-Info-<release>.json` — machine-readable release manifest;
- `FlexFox80-Checksums-<release>.txt` — SHA-256 checksum list for every packaged file other than the checksum file itself;
- `README-FlexFox80-<release>.txt` — plain-language installation, preservation, compatibility, and rollback warnings.

The AVR HEX and ESP sketch BIN may also be uploaded individually for qualified installers. Keep the LittleFS image inside the complete ZIP unless an explicit recovery workflow requires a standalone asset.

## Files deliberately excluded

- Do not publish a generic AVR EEPROM image. EEPROM contains unit configuration and must be preserved, classified, migrated when required, restored, and verified under [release workflow](RELEASE_WORKFLOW.md).
- Do not publish unit-specific flash, EEPROM, fuse, SSID, event, or settings captures.
- Do not publish fuse writes as an ordinary firmware update. Any future manufacturing/setup package needs separate hardware-revision and confirmation safeguards.
- Do not include Debug output, IDE intermediates, object files, map files, or arbitrary files left in a build directory.

The AVR map, listing, EEPROM object, and resource report remain release evidence and may be retained separately, but they are not installation assets.

## Manifest contract

The manifest format identifier is `flexfox80-release-info-v1`. A conforming manifest has this shape:

```json
{
  "format": "flexfox80-release-info-v1",
  "product": "FlexFox80",
  "release": "vX.Y.Z",
  "channel": "stable",
  "generatedUtc": "2026-07-14T00:00:00.000Z",
  "gitCommit": "40-character release commit",
  "branch": "AVR128DA48",
  "hardwareRevision": "Ver 2.1 (Mar 2022)",
  "avr": {
    "version": "0.201",
    "processor": "AVR128DA48",
    "toolchain": "AVR-GCC 7.3.0",
    "devicePack": "AVR-Dx_DFP 1.9.103",
    "eepromSchemaBytes": 274
  },
  "esp": {
    "version": "2.1",
    "board": "Adafruit Feather HUZZAH ESP8266",
    "clockMHz": 80,
    "flashLayout": "4M1M",
    "core": "ESP8266 2.7.4",
    "webSockets": "2.3.6"
  },
  "compatibility": {
    "combinedVersionReport": "SW_VERSIONS,2.1,0.201",
    "eventFilePolicy": "legacy stored files accepted; clone transfers require matching checksum"
  },
  "files": [
    {
      "fileName": "FlexFox80-AVR-0.201.hex",
      "kind": "avr-flash",
      "sizeBytes": 0,
      "sha256": "64 lowercase hexadecimal characters"
    }
  ]
}
```

Every packaged installation image must have one `files` entry. The package README and checksum list must use the same filenames as the manifest.

## Independent validation

The package validator must fail unless all of the following hold:

1. The manifest format, product, release label, channel, branch, full Git commit, and exact hardware target `Ver 2.1 (Mar 2022)` are present and valid.
2. AVR and ESP versions equal the definitions in the tagged source and compose the manifest's combined version report.
3. The manifest pins the qualified build profiles and the AVR EEPROM schema is exactly 274 bytes (`0x112`).
4. Every manifest and checksum entry names exactly one existing regular file, with matching byte size and SHA-256.
5. The ZIP contains every required file and no unlisted installation image.
6. The AVR HEX parses as valid Intel HEX and contains program flash only; it must not silently include EEPROM, fuses, signatures, or lock bytes.
7. The ESP sketch and LittleFS images match the selected build outputs and expected flash layout.
8. The package README warns that routine AVR installation preserves unit EEPROM and routine ESP installation does not overwrite LittleFS.
9. The manifest commit equals the annotated tag target and is an ancestor of the intended release branch.

After archiving, copy the archived assets into a clean temporary directory and repeat the same checks. A successful copy or upload command is not archive proof.

## Distribution and archive options

Retain in the designated durable release archive:

- the complete validated release ZIP;
- the AVR HEX for qualified UPDI installation;
- the ESP sketch BIN for qualified FTDI installation;
- optionally the manifest and checksum list as standalone convenience assets.

The durable archive may be a GitHub release, an owner-managed release store, or both. Public distribution is not required for this legacy product. The maintainer notes must identify both embedded versions, hardware applicability, important compatibility constraints, completed verification, approved deferrals, and the source tag. If a GitHub release is used, do not substitute generated commit or pull-request notes for that record.
