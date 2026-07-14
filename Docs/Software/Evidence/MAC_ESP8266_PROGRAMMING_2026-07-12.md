# Mac ESP8266 Programming Evidence

**Date:** 2026-07-12

**Device:** Standalone Adafruit HUZZAH ESP8266 breakout historically used for FlexFox programming

**Status:** Core compatibility isolated; core-2.7.4 clone-sync firmware installed and single-unit protocol qualified; exact rollback retained

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

## Initial rejected artifact

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

## Initial qualification checklist

The initial plan required installation on the dummy-loaded FlexFox and the following live evidence. The later sections record which checks now pass and which still need a second unit:

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

## Core/library characterization matrix

Each candidate used the preserved mature filesystem and the pre-clone source plus only the source-owned WebSocket lifecycle shim unless noted otherwise. Basic startup was tested standalone by pressing RESET and observing LED behavior and SSID advertisement.

| ESP8266 core | WebSockets | Source | Standalone result |
| --- | --- | --- | --- |
| 3.1.2 | 2.7.2 | Pre-clone | Repeated resets, abnormal LEDs, no SSID |
| 3.0.2 | 2.3.6 | Pre-clone | Repeated resets, abnormal LEDs, no SSID |
| 2.7.4 | 2.3.6 | Pre-clone | Normal LEDs and SSID |
| 2.7.4 | 2.3.6 | Clone-sync source | Normal LEDs and SSID |

The 3.1.2 pre-clone serial trace repeatedly emitted `$ESP,0;` and rebooted about every ten seconds while standalone. That rules out the AVR, installation, and clone-sync source as causes of the 3.x failure. Core 2.7.4/WebSockets 2.3.6 is the pinned hardware-compatible environment.

## Final installed candidate

The complete clone-sync source built under core 2.7.4/WebSockets 2.3.6 produced:

| Property | Value |
| --- | --- |
| Size | 503,392 bytes |
| SHA-256 | `3b6b5ad8e20d9662c9ee833f9c8072b955b27f61d895cc9dde95a3d13f4a796e` |
| Address | `0x00000000` |

The write hash and a separate `verify_flash` operation passed. Standalone LEDs and SSID were normal. After installation on the dummy-loaded FlexFox, `just wifi-probe` passed HTTP, WebSocket, preserved SSID/settings, ESP/AVR identity, temperature, battery, and time traffic.

The opt-in single-unit clone-control test then proved:

- ordinary time reports were active before quiet mode;
- `$ESP,C;` suppressed ordinary time reports;
- `$ESP,S;` produced exactly one next-RTC-edge report;
- no second report appeared while quiet remained active;
- `$ESP,R;` restored normal time reports.

The preserved mature 4 MB image remains the rollback baseline. Full master-target cloning still requires a second updated FlexFox.
