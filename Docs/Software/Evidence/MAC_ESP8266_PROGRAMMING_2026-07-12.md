# Mac ESP8266 Programming Evidence

**Date:** 2026-07-12

**Device:** Standalone Adafruit HUZZAH ESP8266 breakout historically used for FlexFox programming

**Status:** Migration candidate rejected on hardware; exact pre-change 4 MB image restored and independently verified

## Device identity

The standalone board was connected through an FT232R USB UART adapter at `/dev/cu.usbserial-ABAKJ3Y8`. ESP8266 core 3.1.2's bundled `esptool.py` 3.0 reported:

- chip: ESP8266EX;
- crystal: 26 MHz;
- MAC: `a4:e5:7c:2d:69:ed`;
- flash: 4 MB.

Manual bootloader entry was required:

1. hold GPIO0;
2. press and release RESET;
3. release GPIO0.

Commands had to use `--before no_reset`; otherwise `esptool` could undo the manual bootloader state while opening the FTDI port. A 460800-baud read failed after the baud transition with `Invalid head of packet (0x40)`. All preservation, programming, and verification operations therefore used the stable 115200-baud link.

## Preserved rollback image

Before writing, all 4,194,304 flash bytes were read to the ignored hardware-evidence tree:

```text
Software/Huzzah/tmp/device-backups/huzzah-a4e57c2d69ed-20260712-full.bin
```

Backup SHA-256:

```text
a03834d378a3b39b70e22839a0f3bf276fb8145a86340efe72c57d9cc1e800f6
```

An independent `verify_flash 0x0 <backup>` operation compared all 4 MB against the device and returned `verify OK (digest matched)` before programming. The rollback image is intentionally ignored and is not committed to Git.

## Programmed artifact

The repeatable migration-candidate firmware from `just esp-build` was programmed at address `0x0`:

| Property | Value |
| --- | --- |
| File | `ARDF_Transmitter.ino.bin` |
| Size | 512,448 bytes |
| SHA-256 | `87e46f71595522434985f585ea543af075b034f6293e6e116f60cd53f6df257d` |
| Address | `0x00000000` |

Only the firmware image was written. The existing FlexFox filesystem and settings beginning at `0x300000` were not erased or replaced. `esptool` auto-detected the 4 MB flash, wrote all compressed blocks successfully, and verified the write hash.

After the board reset, a separate manual bootloader entry and `verify_flash 0x0 ARDF_Transmitter.ino.bin` returned:

```text
verify OK (digest matched)
```

## Remaining qualification

The standalone HUZZAH must be installed on the dummy-loaded FlexFox containing the already-qualified AVR clone-sync firmware. Live testing must then prove:

- ordinary broadcasts stop on both master and target during cloning;
- the master emits its one-shot synchronization report after the next RTC edge;
- the target writes and reads back the exact requested epoch;
- both sides resume ordinary reports after success and failure cleanup;
- repeated clone operations have acceptably small phase spread.

## Hardware startup regression and rollback

After installation on the dummy-loaded FlexFox, the migration candidate did not provide an SSID. Both HUZZAH LEDs remained on for roughly five seconds and off for roughly five seconds, instead of the prior rapid paired blinking. The routed read-only WiFi probe timed out. No clone or configuration command reached the unit.

This is a hardware startup regression. The observation does not yet distinguish an ESP8266-core/library compatibility problem, an old-filesystem mount problem, or a Linkbus/startup interaction. Compilation and flash verification alone were insufficient qualification.

The HUZZAH was removed and the complete preserved 4 MB image was restored at address `0x0`. `esptool` verified the write hash, and a separate manual bootloader entry plus full-image `verify_flash` returned:

```text
verify OK (digest matched)
```

After reinstallation, the original LED behavior and FlexFox SSID returned and the unit appeared to function normally.

The operator also clarified that a HUZZAH does not need to be installed on the FlexFox to test basic ESP startup: after leaving programming mode, pressing RESET lets the standalone board boot and advertise its SSID. It cannot communicate with the AVR until installed. Every future ESP candidate must therefore pass this standalone reset/SSID smoke gate before installation.

The ESP8266 core 3.1.2/WebSockets 2.7.2 candidate is hardware-rejected until a characterization build isolates the regression. The mature pre-change image remains the operational baseline.
