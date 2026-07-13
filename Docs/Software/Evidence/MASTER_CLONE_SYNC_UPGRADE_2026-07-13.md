# Master Clone-Synchronization Firmware Upgrade — 2026-07-13

## Status

The installed master passes its startup/SSID, HTTP, WebSocket, live AVR telemetry, preserved-role, and clone quiet/one-shot/resume gates. Its AVR now runs the edge-aligned boot correction; the candidate flash, migrated configured EEPROM, and unchanged fuses are independently verified byte-for-byte. The post-program boot plus five resets stayed within a 39.5 ms median range with no integral-second state change. A corrected master-target clone then crossed the exact RTC-readback gate, transferred all nine files, cleaned up normally, and placed the target approximately 0.48–0.51 seconds behind the master. A post-clone master reset changed its median by only -7.5 ms. Repeated clone and drift measurements remain pending.

## ESP8266 preservation and programming

The standalone master HUZZAH was accessed through the FT232R adapter. Its identity was:

- ESP8266EX, 26 MHz crystal;
- MAC `44:17:93:0f:09:3e`;
- chip ID `0x000f093e`;
- 4 MB detected flash.

A complete 4,194,304-byte rollback read was captured by esptool at 57,600 baud and retained only beneath the ignored `Software/Huzzah/tmp/device-backups/` directory:

- SHA-256: `91afb7bebcc9cc56af06dd6211bc2f8cd40933681edd3a15fb6237cd7ba81ec2`.

Because the board's FLASH button is defective and required manual shorting, the operator chose to skip a separate full-device `verify_flash` pass before programming. The complete read and hash are rollback evidence, but are not described as an independently verified pre-write image.

Only the firmware region at `0x00000000` was written. The retained filesystem and master-role configuration were not erased. The compatible clone-sync artifact was built with ESP8266 core 2.7.4 and WebSockets 2.3.6:

- firmware bytes: 503,392;
- SHA-256: `3b6b5ad8e20d9662c9ee833f9c8072b955b27f61d895cc9dde95a3d13f4a796e`.

Esptool verified the written data hash. After RESET without GPIO0 held, the standalone HUZZAH returned to its normal LED behavior and advertised its preserved SSID. It cannot exercise AVR communications until reinstalled.

## AVR identity and rollback evidence

The Atmel-ICE and target reported:

- Atmel-ICE serial `J41800053674`, firmware 1.42;
- target voltage 3.28 V;
- AVR128DA48 signature `1E 97 08`, silicon revision 1.7;
- 100 kHz UPDI clock.

Two independent pre-write reads were byte-identical for each memory:

| Memory | Bytes | SHA-256 |
| --- | ---: | --- |
| Existing flash | 69,576 | `3f51603b3eb133b45e796d32bf72ccc6ea7f4c4d5890f58d978c04a3f3b22b49` |
| Existing EEPROM | 512 | `a0ab17cbd544e4969f788f03ce836ef97741c8cf5f2641ce4cbfb7bebd968ace` |
| Fuses | 16 | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` |

Raw unit-specific images remain ignored and are not committed.

## Legacy EEPROM layout discovery

The master EEPROM's late fields are coherent only under the historical 268-byte Release layout that stored five `Fox_t` values and one `Event_t` value as one byte each. Representative decoded values include:

- event type 3;
- primary/beacon frequency 3.600 MHz;
- low, medium, and high frequencies 3.530, 3.550, and 3.570 MHz;
- RF power 3,000 mW;
- voltage threshold 3.8 V;
- event start 2026-07-11 07:51 EDT;
- event finish 2026-07-12 10:51 EDT.

Reading the same bytes at the corrected 274-byte offsets produces implausible values. Restoring this raw image unchanged after installing corrected firmware would therefore shift and corrupt the interpreted late settings.

This finding does not contradict the tracked Debug ELF's 274-byte DWARF layout. It establishes that configured field units may retain EEPROM last written by a historical 268-byte Release build, regardless of a later flash image's ABI.

## TDD migration utility

The failing test was added first. It initially stopped with `ERR_MODULE_NOT_FOUND` because the migration implementation did not yet exist. The completed utility is `scripts/migrate-eeprom-enum-layout.mjs`, covered by `Tests/Host/eeprom_enum_layout_migration_test.mjs` and the ordinary `just test` gate.

The utility:

- requires an explicit `--from legacy-268` declaration;
- requires exactly 512 input bytes and refuses in-place or existing-output writes;
- validates the initialization flag and plausible legacy enum, event, frequency, master, and voltage fields;
- widens only the six historical enum fields, setting their new high bytes to zero;
- moves every later schema field to its corrected offset;
- preserves bytes 274–511 exactly;
- rejects images that cannot be confidently classified.

Two migrations from the independent master EEPROM reads produced the same 512-byte output:

- migrated EEPROM SHA-256: `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c`.

The earlier test unit's original EEPROM did not pass the legacy-layout plausibility gate and was not migrated. It requires separate diagnosis rather than forced conversion.

## AVR build and programming

Source commit `ae1af3e42567782a6cd5d728af1c1c95345f9160` was built twice with AVR-GCC 7.3.0 and `AVR-Dx_DFP` 1.9.103. Both runs were warning-free and produced the same HEX and EEP. The linker map reports `.eeprom = 0x112` (274 bytes).

| Artifact | SHA-256 |
| --- | --- |
| Release HEX | `85fa56ee168ff3e90fa7594d6bb066db999b758e312808dfe44fcca902769cb0` |
| EEPROM initializer | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| Expected raw flash | `878cc77f088ac89c4424fe0c83fed3527856550914e907b074507f1fdf29d72e` |

The AVR was explicitly erased, the Release flash was programmed, and the migrated configured EEPROM was written. A fresh verify-only pass checked all 41,770 flash bytes and all 512 EEPROM bytes. Independent post-operation reads then established:

| Memory | Expected SHA-256 | Post-read SHA-256 | Result |
| --- | --- | --- | --- |
| Programmed flash | `878cc77f088ac89c4424fe0c83fed3527856550914e907b074507f1fdf29d72e` | `878cc77f088ac89c4424fe0c83fed3527856550914e907b074507f1fdf29d72e` | byte-identical |
| Migrated EEPROM | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

## Installed qualification

With the HUZZAH reinstalled and the Mac routed only to `73.73.73.73` through the Moto/DroidTether tunnel, the repository's read-only WiFi probe reported:

- HTTP 200 and a connected WebSocket;
- SSID `Tx_Master`;
- ESP MAC `22:C8:8E:CF:AB:84`;
- software versions `2.0,0.200`;
- `MASTER,1`;
- live 30.0 C, 11.6 V, and two-second clock reports from the AVR.

The bounded `wifi-clone-control-test` then passed:

1. baseline clock reports active;
2. ordinary reports suppressed in clone quiet mode;
3. exactly one clock report emitted at the next RTC edge;
4. no additional clock report while quiet;
5. normal reports resumed after clone cleanup.

The control test does not write RTC, EEPROM, event, RF, or filesystem state.

## Edge-aligned AVR boot upgrade and reset qualification

The reset-isolation experiment subsequently showed that the earlier master image could acquire a nearly one-second AVR system-time phase shift across reset even though its RTC and communications remained operational. Commit `730b63109eb5212bde8320b9720fe13d135c6206` replaces the arbitrary-phase boot RTC read/set sequence with the existing next-edge `syncSystemTimeToRTC()` helper. It does not change the square-wave ISR, clone protocol, ordinary clock writes, event scheduling, EEPROM schema, or ESP firmware.

Two pinned AVR-GCC 7.3.0 / AVR-Dx_DFP 1.9.103 Release builds completed with zero warnings and identical output:

| Artifact | SHA-256 |
| --- | --- |
| Release HEX | `130147e4182a30897a170c16481530ddb37ebd419271ac33c3df35316cbf853b` |
| EEPROM initializer | `c8dc188f9317e79d57b2852dc509c41481951eb974b68baa1e34f53d7cef7906` |
| Expected raw flash | `629b6b5ce564da965ba9977fae6d0653ac74b1e100980859452a7888a35a85ab` |

Fresh pre-write reads matched the already-qualified master state: raw flash `878cc77f088ac89c4424fe0c83fed3527856550914e907b074507f1fdf29d72e`, configured EEPROM `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c`, and fuses `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126`. Programming used an explicit erase, wrote and verified the candidate flash twice, restored and verified all 512 EEPROM bytes twice, and did not write fuses. Independent post-operation reads established:

| Memory | Expected SHA-256 | Independent readback SHA-256 | Result |
| --- | --- | --- | --- |
| Candidate flash binary | `629b6b5ce564da965ba9977fae6d0653ac74b1e100980859452a7888a35a85ab` | `629b6b5ce564da965ba9977fae6d0653ac74b1e100980859452a7888a35a85ab` | byte-identical |
| Migrated configured EEPROM | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | `5ad612a6aa41ae86de821ba4b701a7072aaeebb942747e2562040d08c22d610c` | byte-identical |
| Preserved fuses | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | `837b85bfd32b26ed1cc534c6f1970b7d0ef3ce36a4b3b71612602170f1301126` | byte-identical |

The reprogrammed master returned as `Tx_Master`, ESP MAC `22:C8:8E:CF:AB:84`, software `2.0,0.200`, and `MASTER,1`, with passing HTTP, WebSocket, temperature, battery, and live clock reports. Its post-program boot and five subsequent Atmel-ICE-induced AVR resets each received 12 ordinary clock reports through the same observation path:

| Boot | Median offset | Mean offset | Sample standard deviation | Minimum | Maximum | Spread |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Post-program baseline | +649.5 ms | +667.50 ms | 71.02 ms | +601 ms | +859 ms | 258 ms |
| Reset 1 | +639.0 ms | +668.42 ms | 65.87 ms | +593 ms | +791 ms | 198 ms |
| Reset 2 | +649.5 ms | +664.92 ms | 71.42 ms | +593 ms | +817 ms | 224 ms |
| Reset 3 | +678.5 ms | +694.83 ms | 86.17 ms | +592 ms | +898 ms | 306 ms |
| Reset 4 | +665.5 ms | +687.42 ms | 78.60 ms | +609 ms | +899 ms | 290 ms |
| Reset 5 | +644.5 ms | +671.67 ms | 106.86 ms | +592 ms | +988 ms | 396 ms |

Across the six boot medians, the mean was +654.42 ms, the median was +649.5 ms, the sample standard deviation was 14.75 ms, and the complete range was 39.5 ms. No reset produced an integral-second state change. Together with the target's 81 ms six-boot range, this qualifies the edge-aligned correction against the reproduced reset defect on both units. Individual report spreads still include the non-real-time WiFi/tunnel observation path and are not physical RTC-edge timestamps.

## Two-unit qualification

Before the edge-aligned boot correction, the updated clone-control master and target completed two end-to-end clone cycles after two operator-requested target resets. Each cycle reached event-file transfer and normal slave release, which is unreachable in the updated target state machine unless the clone-specific RTC epoch readback exactly matches the requested epoch and the Linkbus ACK completes without NAK or timeout.

A 12-sample read-only series from each unit through the same Moto/DroidTether path found the target approximately 537 ms behind the master by medians and 564.5 ms by means. This passes the first immediate multi-second-outlier check and is consistent with the expected half-second DS3231 write/edge relationship. See [Two-unit clone synchronization qualification](TWO_UNIT_CLONE_SYNC_2026-07-13.md) for raw statistics, limits, and next measurements.

After both AVRs received the edge-aligned image, a fresh clone crossed the exact RTC-readback gate and transferred all nine event files with their existing `CHECK,1013` records and `EOF` markers. It then emitted `SLAVE,NMF`, `SUE,No events scheduled to run`, `SLAVE,0`, and resumed ordinary clock reports. The earlier six-file stall did not recur in this attempt.

The post-clone master median was +693.0 ms. Two target series measured +1172.0 and +1199.0 ms, placing the target 479 and 506 ms behind the master by medians. Their corresponding mean differences were 514.42 and 493.42 ms. The operator then reset the master to wake its WiFi; its closing series measured +685.5 ms median, only -7.5 ms from its pre-reset value, with a +669.17 ms mean and 61.47 ms sample standard deviation. This independently exercises the corrected boot path after cloning and shows no integral-second reset shift.

Repeated clone phase-spread, physical edge/transmission timing, cleanup-failure, and multi-day drift measurements remain open.

The field synchronization bug is not yet described as fixed.
