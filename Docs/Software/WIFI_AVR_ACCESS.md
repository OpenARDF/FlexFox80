# FlexFox WiFi-to-AVR Access

## Purpose

FlexFox does not currently expose a supported wired serial console. Normal interactive access is:

```text
Mac browser or probe -> ESP8266 access point -> HTTP/WebSocket -> 9600-baud Linkbus -> AVR128DA48
```

Atmel-ICE remains the programming and low-level readback path. WiFi is the supported path for observing and controlling the running product.

## Network endpoint

The ESP8266 starts a soft access point in AP+station mode:

- ordinary SSID: `Tx_` followed by the final four bytes of the ESP access-point MAC address;
- master SSID: `Tx_Master`;
- default password: empty (open network);
- ESP address and gateway: `73.73.73.73`;
- subnet: `255.255.255.0`;
- HTTP: port 80;
- WebSocket: port 81.

The access-point address is configurable in running ESP state, so `73.73.73.73` is the source default rather than proof of a particular unit's current value.
Each ESP also retains its MAC-derived `Tx_...` device SSID as its update identity while master mode is temporarily advertising `Tx_Master`.

The HTTP root offers these checked-in pages:

- `/events.html` — event configuration;
- `/test.html` — engineering controls and direct-message field;
- `/radio.html` — live radio controls;
- file upload, download, and deletion pages.

Opening one of the main pages creates `ws://<page-host>:81/`. The WebSocket server starts when a station joins the ESP access point.

## Message path

Browser messages are textual commands such as `SSID` or `SW_VERSIONS`. The ESP handles some locally and translates others into Linkbus frames queued for the AVR. Linkbus uses a 9600-baud UART, acknowledges commands, and retries a pending message after an ACK timeout.

Examples:

| Browser command | ESP behavior | AVR effect |
| --- | --- | --- |
| `SSID` | Reports the ESP access-point name | None |
| `MAC` | Reports the connected client's MAC address | None |
| `SW_VERSIONS` | Reports ESP version and cached AVR version | None |
| `MASTER` | Reports current ESP master/slave state when no value is supplied | None |
| WebSocket connect | Queues `$TEM?` and `$BAT?` | Reads temperature and battery voltage |
| `PASS,$TIM?` | Queues the raw Linkbus query | Reads RTC time and refreshes AVR system time from the RTC |

The ESP translates AVR replies back to WebSocket messages including `TEMP`, `BAT`, `SYNC`, `ERR_CODE`, `STATUS`, and `POWER`.

## Wireless AVR bootloading

### Implementation status — ESP 2.27 / AVR 0.210 / bootloader BL0.3 candidate

The earlier BL0.1 transport and AVR 0.208 application installation have real pilot evidence. BL0.2 is superseded and must not be provisioned to the fleet: review found that reset-vector-last behavior was controlled only by the ESP, cold recovery could skip the normal ESP rail/reset sequence, an invalid staged image after programming began could leave the web recovery service unavailable, and startup accepted an unsafe single-byte serial entry request. BL0.3 protocol 2 moves the commit decision into the resident bootloader and closes those recovery gaps. The first ESP 2.18 / BL0.3 hardware run reached the reset-page commit boundary but timed out there; ESP 2.19 removed the redundant per-page LittleFS state rewrites exposed by that run. ESP 2.22 additionally persists both qualification interruption hooks across the normal ESP power loss, verifies ambiguous staging responses against SSID/version/size/CRC, and allows an optional Moto-side ADB path to synchronize the Atmel-ICE reset test while the Mac route reforms. ESP 2.23 reuses that mature keep-alive path during ordinary LittleFS uploads and bounds interrupted-upload startup recovery to one metadata rename. ESP 2.24 additionally disables implicit LittleFS formatting, checks mount success, and provides a sketch-resident recovery updater after a mount failure or watchdog-reset startup stall. ESP 2.25 preserves those paths and adds strict temperature telemetry validation and explicit unavailable reporting. ESP 2.27 retains that stable line, canonicalizes the MAC-derived device identity, and closes each completed multipart upload connection so multi-file batches cannot exhaust the HTTP client slots. ESP 2.26 remains assigned to the mothballed Fleet Soak experiment and is not a Development candidate.

AVR 0.210 retains AVR 0.209's scheduled-start sleep correction, relocated bootloader handoff, equal-time persistent disable, LED re-arm after Apply, and separate fixed five-minute WiFi maintenance lease. It additionally makes TCB0 the only single-conversion ADC owner, schedules temperature during early startup, bounds each conversion to 10 ms, carries explicit availability instead of a numeric sentinel, applies one inclusive `-20C` through `120C` rule to telemetry and fan control, and keeps the fan on while temperature is unavailable. ESP 2.27 retains the browser `CLEAR`, event-picker, status, Sync, protocol-2 updater, persistent diagnostics, controlled fault-injection hooks, phase-boundary-only recovery state, ESP 2.24 filesystem recovery, and ESP 2.25 temperature validation. It reports `TEMP,NA` rather than forwarding invalid numbers. See [AVR startup temperature validity](Evidence/AVR_TEMPERATURE_VALIDITY_2026-07-19.md).

- a 16 KiB resident Boot section occupies `0x0000–0x3FFF` (`BOOTSIZE=0x20`);
- the FlexFox application is linked at `0x4000` (`CODESIZE=0x00`);
- the bootloader reasserts AVR PA5 (`WIFI_ENABLE`) and releases PA6 (`WIFI_RESET`) immediately after reset;
- ESP 2.27 stages a page-aligned FlexFox product image and dual-slot recovery state in LittleFS before requesting bootloader entry;
- the ESP accepts update start only after the operator enters the final four characters of the unit's MAC-derived `Tx_...` device SSID;
- the AVR application accepts the internal `$UPD,START,SSID;` handoff only while the event, manual transmission, and RF output are idle;
- raw WebSocket `PASS` forwarding cannot send `$UPD` frames and bypass the dedicated update route;
- bootloader traffic uses USART1 PC0/PC1; the candidate default is 38,400 baud, and ESP 2.27 can discover 9,600, 19,200, 38,400, 57,600, or 115,200 baud for qualification and recovery;
- every protocol frame has CRC-16, every written page is immediately read back by the bootloader, and a protocol-2 begin frame binds the session to the complete image length and CRC-32;
- the resident bootloader itself requires the reset-vector page to be erased first, observes every non-reset page in order, validates a FlexFox-specific product trailer and both payload/full-image CRC-32 values, and permits the reset-vector page only as the final commit;
- EEPROM byte 511 is reserved as a pre-erase update marker, closing the power-loss window before the reset-vector page has been invalidated;
- the staged file is retained until the new application reports the manifest's target version;
- alternating AVR red/green means the resident bootloader is active, solid AVR red is a latched protocol/NVM failure, and simultaneous ESP red/blue for 60 seconds begins only after the exact application version is observed and completion is persisted.

Build the complete one-time install and later wireless-update artifacts with:

```text
AVR_TOOLCHAIN_ROOT=Software/AVR128DA48/tmp/avr8-gnu-toolchain-darwin_x86_64 \
AVR_DFP_ROOT=Software/AVR128DA48/tmp/AVR-Dx_DFP/1.9.103 \
just avr-boot-chain-build
```

The ignored output directory `Software/AVR128DA48/tmp/avr-boot-chain/` contains the combined first-install HEX, the page-aligned wireless update BIN, and release metadata with hashes, geometry, versions, baud, and required fuses. The current exact build produces a 5,112-byte 38,400-baud bootloader inside the 16,384-byte reservation and an 85-page, 43,520-byte AVR 0.209 update image. The final 512-byte page is a FlexFox product trailer; recalculate all values from each release build.

`just avr-bootloader-baud-matrix` builds and hashes all five supported UART variants, packages each with the identical application image, and restores the ordinary output to 38,400 baud. A test baud therefore cannot silently contaminate a later default package.

One-time UPDI provisioning is deliberately destructive and double-gated. It captures EEPROM and fuses, erases and verifies the combined image, restores the unit-specific EEPROM with only newly reserved byte 511 forced to erased `0xFF`, writes only `CODESIZE` and `BOOTSIZE`, and independently re-reads the target:

```text
FLEXFOX_UNIT_ID=<fleet-label> \
FLEXFOX_PROVISION_CONFIRM=PROVISION-BOOTLOADER \
FLEXFOX_FUSE_CONFIRM=WRITE-BOOTSIZE-0x20 \
just avr-provision-boot-chain
```

After the matching ESP and boot chain are installed on a qualified pilot, a later update can use the `/avr-update` page or the guarded host workflow:

```text
FLEXFOX_AVR_UPDATE_CONFIRM=UPDATE-AVR-0.210 just wifi-avr-update
```

The interactive workflow reports the unique device SSID and prompts for its final four characters. An explicitly unattended invocation must supply both `FLEXFOX_EXPECTED_DEVICE_SSID=Tx_<eight-hex-characters>` and `FLEXFOX_AVR_SSID_SUFFIX=<last-four>`. The exact expected SSID is checked before staging, preventing a valid image from being placed on the wrong unit when several FlexFoxes share the same default IP address. `just wifi-avr-preflight` reports the connected device identity and update state without staging an image.

The host allows 30 minutes by default for all pages, reset-vector finalization, ESP restart, and application-version confirmation. The ESP intentionally stops HTTP service while it owns the UART and programs the AVR, so status requests may receive no response during otherwise healthy programming. When a tethered Android device is the route, set `FLEXFOX_ADB_SERIAL=<adb-serial>` (and `FLEXFOX_ADB=<path>` if needed); the updater then repeatedly requests the device's advertised SSID after the AP returns, since Android may refuse to autojoin an open network. For a master, retain the exact MAC-derived identity in `FLEXFOX_EXPECTED_DEVICE_SSID` and set `FLEXFOX_RECONNECT_SSID=Tx_Master`; this changes only Android reassociation, not update authorization. The older `FLEXFOX_AVR_QUALIFICATION_ADB_SERIAL` name remains accepted for qualification scripts. A host-side timeout is never permission to remove power: keep the unit powered and inspect `/avr-update/status`. `/avr-update/log` exposes a bounded persistent journal containing staging, detected bootloader identity/baud/diagnostic, verified pages, retries, commit, application verification, and completion. `FLEXFOX_AVR_VERIFY_TIMEOUT_MS` may override the window with an integer from 60,000 through 3,600,000 milliseconds.

The ESP sketch updater independently checks the exact MAC-derived identity before and after reboot and preserves LittleFS. ESP 2.27 corrects that identity to format each of the final four MAC bytes as two uppercase hex digits. For the one-time migration of an older image that omitted leading zeroes, set `FLEXFOX_EXPECTED_PREUPDATE_DEVICE_SSID` to the exact legacy value, keep `FLEXFOX_EXPECTED_DEVICE_SSID` set to the corrected eight-digit value, and set `FLEXFOX_RECONNECT_SSID=Tx_Master` when the unit advertises the master alias. `FLEXFOX_RECONNECT_SSID` requires `FLEXFOX_ADB_SERIAL`; it affects only the Moto reassociation request and does not weaken either device-identity check.

Hardware qualification is host-synchronized so the operator does not have to time an interruption. `FLEXFOX_AVR_QUALIFICATION_ESP_RESTART_PAGE=<page>` makes ESP 2.22 restart itself after that verified page. `FLEXFOX_AVR_QUALIFICATION_AVR_RESET_PAGE=<page>` pauses at an exact verified page while the host resets the AVR through the attached Atmel-ICE, then continues and requires autonomous replay. `FLEXFOX_AVR_QUALIFICATION_ADB_SERIAL=<serial>` optionally reads only the qualification endpoints through a tethered Moto while the Mac route reforms. `FLEXFOX_AVR_QUALIFICATION_EXTERNAL_POWER_LOSS=1` uses the same verified-page pause without injecting an Atmel reset, allowing a separately observed whole-unit power cut. Each requires the matching `FLEXFOX_AVR_QUALIFICATION_CONFIRM` token. `FLEXFOX_AVR_QUALIFICATION_FINAL_READBACK=1` performs an independent post-success read of all Flash, EEPROM, and fuses and compares the complete 16 KiB Boot section, wireless image, recovery marker, `CODESIZE`, and `BOOTSIZE` with the release artifacts. Host status, journal, programmer output, readback binaries, and hashes are retained in the ignored per-run evidence directory.

After installing ESP 2.22 and BL0.3 on the accessible pilot, the unattended bench sequence reduces the automated controls to one command. It waits for the saved network association between runs and performs an uninterrupted update, an ESP restart after page 8, an Atmel-ICE AVR reset near the image midpoint, and exact readback after every run:

```text
FLEXFOX_EXPECTED_DEVICE_SSID=Tx_<unit-specific-eight-hex-characters> \
FLEXFOX_AVR_QUALIFICATION_CONFIRM=QUALIFY-BL0.3 \
just wifi-avr-bootloader-qualification
```

The harness cannot remove all unit power itself. Unit A passed a separately coordinated off/on cycle at the exact verified page-42 pause; Unit B must repeat the required recovery gates before fleet qualification.

Do not provision the fleet merely because both firmware builds pass. Keep the pilot chassis accessible until boot, ordinary RF/event behavior, WiFi power-down/wake, a complete wireless update, and repeated physical power interruption during erase/write/final handoff have all passed.

### Current fleet upload-recovery result — 2026-07-19

During `Tx_C22DD117` provisioning, power was interrupted after `/test.html` had been staged and the prior live file renamed to its backup. The complete 4 MiB flash verified exactly, LittleFS unpacked without corruption, and all unit-specific `.event` and `.me` files remained intact. The old startup path then reproduced an availability failure by trying to reclaim large stale transaction files before starting HTTP, allowing the AVR's ESP-power timeout to recur on every boot.

ESP 2.23 services the existing firmware-update keep-alive path during ordinary uploads and limits startup recovery to restoring one missing live file by metadata rename. Stale-file deletion moves to the next explicitly kept-awake upload transaction. After FTDI installation with the repaired unit-specific filesystem, the standalone ESP reported 2.23 and exact `/test.html`, `/events.html`, and `/radio.html` hashes. Reinstalled in the FlexFox, the unit passed the read-only preflight and full idempotent workflow at ESP 2.23 / BL0.3 / AVR 0.208 with live telemetry. See [ESP interrupted file-upload recovery](Evidence/ESP_INTERRUPTED_FILE_UPLOAD_RECOVERY_2026-07-19.md).

ESP 2.24 generalizes that recovery. It never auto-formats LittleFS, exposes `filesystemMounted`, `recoveryMode`, and a recovery reason in `/firmware/status`, and withholds every filesystem-dependent route and operation when the mount is unavailable. A retained RTC startup marker lets the ESP watchdog escape a synchronous filesystem startup stall on the next boot; recovery then advertises the unique MAC-derived SSID and serves only the sketch-resident ESP updater. Successful host-side ESP installation now requires both the expected sketch identity and a mounted, non-recovery filesystem. Pilot hardware has passed the recovery-only install/restore cycle, the AVR 0.209 wireless update, a 135.817-second no-WebSocket maintenance upload, exact scratch-file cleanup, and return to ordinary automatic WiFi shutdown.

`just esp-filesystem-recovery-build` creates a separate ignored qualification artifact under `Software/Huzzah/tmp/esp-build-filesystem-recovery/`. It deliberately enters the same recovery-only route set without mounting or modifying LittleFS. It must never be used as a production fleet image. The guarded `just esp-filesystem-recovery-qualify` workflow requires the exact MAC-derived device SSID and a separate confirmation phrase, installs only a build whose evidence identifies it as the qualification variant, verifies the recovery reason and route status, and then uses that sketch-resident updater to reinstall and verify the ordinary `just esp-build` artifact. The separate output directory prevents the qualification build from replacing the normal rollout artifact.

`just wifi-maintenance-lease-test` requires an exact SSID and `FLEXFOX_WIFI_LEASE_TEST_CONFIRM='TEST UPDATE WIFI LEASE'`. It queries the live ESP/AVR identity, uploads a random scratch file slowly for at least 125 seconds without a WebSocket heartbeat, reads it back exactly, and deletes it. This is the live regression for surviving the former two-minute cutoff; the dependency-free AVR host test remains the exact proof of the unrenewed 300-second upper bound.

### Current pilot qualification result — 2026-07-19

Unit A (`Tx_7C2D6FD3`, ESP MAC `86:A8:24:2F:96:5B`, Atmel-ICE `J41800053674`) now has ESP 2.22, AVR 0.208, and BL0.3 protocol 2 at 38,400 baud. The protected ESP updater verified the installed 563,888-byte sketch as MD5 `ce7ed4ed788edb346e05f8e87b36047e`, reset uptime, and retained LittleFS protection. A post-update read-only probe returned `SW_VERSIONS,2.22,0.208`, temperature, battery, clock, complete SSID/MAC, and `MASTER,0`.

The exact 43,520-byte AVR image, CRC32 `0xdcf1e479`, passed all Unit A recovery cases with independent final readback:

- an uninterrupted 85-page update completed;
- a persisted one-shot ESP restart immediately before page 8 produced two bootloader sessions and exact final Flash/EEPROM/fuse verification;
- an Atmel-ICE reset immediately after verified page 42 also removed ESP power, then cold replay produced two sessions and exact final verification;
- a real whole-unit power cut immediately after the Moto independently reported `armedPage:42, resetReady:true` produced a journal with the first session verified through page 42, a second cold-boot session, all 85 replayed pages, reset-page-last commit, and `complete version=0.208`;
- the final normalized Flash SHA-256 was `8ca3c299b35ded79fc8e4a9576d5a3af89f7ab7297fa40ef88c59cd366138257`; EEPROM marker byte 511 was `0xFF`; fuses remained SHA-256 `d1edc2ade950e20b08c4fefd45ebbc8af2d6fc36fcad876c9457328c6d2925f5`, with `CODESIZE=0x00` and `BOOTSIZE=0x20`.

The qualification intentionally preserved failed attempts. A RAM-only AVR-reset hook on ESP 2.21 disappeared during the normal handoff. ESP 2.22 persists and atomically consumes that one-shot. Two later operator-timing attempts stopped at the page-42 qualification timeout with the reset vector still protected; exact restaging cleared the diagnostic and allowed the successful retry. Those fail-safe stops are useful recovery evidence, not passes.

Unit A therefore passes the boot-chain interruption and exact-readback gates. This does not yet pass the two-unit, normal event/RF, file-preservation, equal-time Apply, final disposition, or extended fleet-soak gates.

### Earlier pilot qualification result — 2026-07-18

The connected pilot completed both directions of a real wireless application update after the one-time UPDI boot-chain installation:

- AVR 0.204 was updated wirelessly to 0.205, all 83 pages completed, the staged file was removed, and the new application reported version 0.205 over Linkbus;
- AVR 0.205 was then rolled back wirelessly to the intended fleet candidate, AVR 0.204;
- while the rollback was in progress, an Atmel-ICE session interrupted the target after the application reset-vector page had been erased; readback showed the `0x4000` vector page erased and EEPROM update marker byte 511 cleared;
- after the programmer released the target, the resident bootloader and ESP recovery state resumed without restaging or physical intervention, completed all 83 pages, and AVR 0.204 reported its exact version over Linkbus;
- ESP 2.12 retained LittleFS throughout and reported 606,208 bytes free after removing the completed 42,496-byte staged image;
- the pilot's temporary event cancellation was reversed: the restored LittleFS event read back with its original hash, all 512 AVR EEPROM bytes matched the pre-cancellation capture, and live AVR SRAM reported the original finish epoch `1784402100` (`2026-07-18T19:15:00Z`).

This establishes one successful end-to-end update, downgrade, and genuine post-erase interruption recovery. It does not replace repeated power-loss testing at several page positions, normal WiFi wake/power-down regression, ordinary event/RF regression, or a second pilot before fleet rollout.

The exact ESP 2.22 / BL0.3 / AVR 0.208 two-unit procedure, per-unit preservation rules, LED checks, automated interruption gates, disabled-event Apply check, and final disposition are maintained in [WiFi AVR two-unit qualification](Evidence/WIFI_AVR_TWO_UNIT_QUALIFICATION_2026-07-18.md). The record also preserves the successful earlier transport, independent AVR 0.208 readback, and ESP 2.18 commit-boundary failure evidence. Record the remaining hardware observations there rather than treating a quiet updater as complete qualification.

BL0.3 intentionally uses integrity checks and deliberate operator authorization rather than cryptographic firmware signatures. The host verifies the release SHA-256, the ESP and resident bootloader independently validate the complete image CRC-32 and product trailer, and every frame and programmed page is CRC-checked. Entry requires the final four characters of the unique MAC-derived ESP SSID, but that suffix is an operator-error interlock rather than a secret: it is readily visible to the person connected to the unit. This matches the deployment model in which the ESP is normally reachable for only two minutes after power-up, has short range, and is used predominantly in rural locations.

### Feasibility decision

AVR firmware updates over the existing WiFi path are implemented using this transport:

```text
Browser or host -> WiFi -> ESP8266 -> USART1 -> AVR bootloader -> AVR application flash
```

The supported path is the dedicated `/avr-update` transaction. The arbitrary file uploader and `PASS` command are not firmware-update mechanisms.

Microchip documents self-programming for the AVR DA family in [AN3341, Basic Bootloader for the AVR DA MCU Family](https://www.microchip.com/en-us/application-notes/an3341). Its AVR128DA48 UART example uses USART1 on PC0 for transmit and PC1 for receive. FlexFox already uses those same pins and USART for Linkbus, as shown in [`linkbus.h`](../../Software/AVR128DA48/FlexFox80/include/linkbus.h) and [`linkbus.cpp`](../../Software/AVR128DA48/FlexFox80/src/linkbus.cpp). Therefore, no PCB change is required merely to provide a serial data path between the ESP and an AVR-resident bootloader.

The existing hardware cannot instead make the ESP behave like the Atmel-ICE over UPDI without modification. The AVR UPDI pin is routed to programming header P301, not to the ESP, while the ESP is connected to the AVR through the PC0/PC1 UART path. See [`FlexFox80.net`](../../KiCad/FlexFox80.net) nets 62, 96, and 99.

### Required one-time provisioning

The July 2026 target fuse capture records bytes `00 00 F0 FF FF D3 F8 00 00 FF FF FF FF FF FF FF`; byte 8 is the AVR128DA48 `BOOTSIZE` fuse. The [AVR128DA48 data sheet](https://www.microchip.com/content/dam/mchp/documents/MCU08/ProductDocuments/DataSheets/AVR128DA28-32-48-64-Data-Sheet-DS40002183.pdf) defines a value of zero as the entire Flash being the Boot section. The present application is linked at address zero and has no reserved, independently protected bootloader region. See [Mac Atmel-ICE target evidence](Evidence/MAC_ATMEL_ICE_TARGET_EVIDENCE_2026-07-12.md).

Every existing unit would consequently need one physical UPDI provisioning operation before it could receive later wireless AVR updates. That operation must install and verify all of the following as one compatible set:

1. a permanent bootloader in a nonzero Boot section;
2. the matching nonzero `BOOTSIZE` fuse;
3. an application linked after the reserved Boot section;
4. the expected interrupt-vector selection and boot/application handoff behavior;
5. unchanged or deliberately migrated EEPROM and other unit-specific state.

The selected layout reserves 32 512-byte pages (16 KiB), places the application at `0x4000`, and leaves 112 KiB for application code. Exact sizes and section boundaries are checked by the build/package scripts rather than copied from this note. The pinned ESP 2.27 build uses 564,760 sketch bytes and leaves 30,672 bytes of dynamic-memory headroom. IRAM uses 27,676 of 32,768 bytes, leaving 5,092 bytes, and remains the tighter executable margin to check on every release build. The relocated AVR 0.210 application payload is 43,520 bytes; its complete protocol-2 wireless image is 44,032 bytes (86 pages), leaving the resident BL0.3 unchanged.

The qualified ESP profile provides a 1,024,000-byte LittleFS partition with 8 KiB allocation blocks. Unit A previously measured 536 KiB free with a complete protocol-2 image staged; AVR 0.210 increases that staged file by only one 512-byte page. The persistent journal is capped at 32,768 bytes. Before opening the staging file, ESP 2.27 reads `LittleFS.info()`, rounds the declared image size to whole filesystem blocks, and requires four additional free blocks (32 KiB on this profile) for metadata and recovery-state changes. The status endpoint and update page report measured total, used, and free bytes; the same preflight remains mandatory on every unit.

### AVR reset cuts power to the ESP

The important hardware complication is the ESP power-enable circuit. AVR PA5 drives `WIFI_ENABLE`, which drives the shutdown input of LT1763 regulator U306. R310 is a 10 kOhm pull-down from `WIFI_ENABLE` to ground. When the AVR is reset, PA5 becomes high-impedance, R310 disables U306, and the ESP loses power. The AVR also controls the ESP reset line; the ESP does not have an existing reciprocal AVR-reset control.

The implementation does not assume that a live ESP or browser socket survives AVR reset. It deliberately uses persistent ESP state and reset-based bootloader entry.

#### Rejected seamless-session alternative: direct entry without reset

A direct application-to-bootloader jump could preserve the browser session, but it is not used because it carries more untestable C-runtime, stack, vector, and peripheral handoff state. The reset-based implementation accepts a temporary ESP/network interruption and resumes from LittleFS instead.

1. The ESP receives the complete candidate into a temporary LittleFS file, validates it, and atomically promotes it to a staged update.
2. The ESP sends an explicit bootloader-entry command over Linkbus.
3. The AVR refuses entry while RF is active, then forces the transmitter into a safe state, persists the recovery marker, quiesces Linkbus, and deliberately leaves `WIFI_ENABLE` asserted.
4. The AVR issues a software reset. The bootloader detects the deliberate handoff in reset/GPR state, preserves the already-running ESP power/reset outputs only for that live handoff, and otherwise uses the normal cold rail/reset sequence.
5. The ESP and bootloader transfer, write, acknowledge, and verify the complete application.
6. Only after the resident bootloader has accepted the reset page as the final commit may it enter the new application. The ESP then requires the exact reported application version before deleting the staged image and declaring completion.

#### Cold-reset and interrupted-update recovery

A power failure, watchdog reset, brownout, UPDI reset, or unexpected application failure can still reset the AVR and power-cycle the ESP. Recovery therefore cannot depend on preserving the original WiFi session.

Before erasing any application page, persist an update-pending state in locations defined for both processors. On a later reset, the bootloader must:

- assert the RF-safe outputs and re-enable ESP power immediately;
- distinguish a valid ordinary application from an update-pending or invalid application;
- enter the valid application promptly during an ordinary boot;
- wait without a short fixed timeout when an update is pending or the application fails integrity validation;
- tolerate the ESP's full startup and access-point delay;
- accept a resumable or complete retransmission from the image retained in LittleFS;
- keep the bootloader itself immutable throughout recovery.

The ESP must retain the staged candidate and its manifest across its own power loss. The browser may need to reconnect to the re-created access point, but the device must not require an Atmel-ICE merely because the update was interrupted. Persistent state must also distinguish an intentionally requested update from an image that has already been accepted, preventing reset loops and accidental reinstallation.

### Optional hardware improvement

A hardware modification is required if the product requirement is to keep the ESP and browser session alive across a true AVR reset. The simplest candidate is to make `WIFI_ENABLE` default high while the AVR pin is high-impedance, while retaining the AVR's ability to drive the signal low for intentional WiFi shutdown. Reworking R310 from a pull-down to an appropriate logic-supply pull-up is one possible implementation, but it must not be adopted from this software analysis alone.

Changing the default has product-level consequences:

- an unprogrammed, held-in-reset, or crashed AVR could leave the ESP powered;
- battery drain and sleep behavior would change;
- startup UART garbage and the existing ESP-reset/UART-gating logic must be requalified;
- regulator shutdown thresholds, voltage domains, reset timing, and PCB rework practicality must be checked;
- all normal WiFi-off, sleep, wake, clone, and fault-recovery paths require regression testing.

Alternatives include a calculated hold-up network on the regulator shutdown control or a self-hold/OR enable circuit shared by the AVR and ESP. Those options may retain default-off behavior or allow the ESP to hold its own power during an update, but they require an electrical design and fault analysis. Merely adding an ESP-to-AVR reset connection does not solve the existing power interruption by itself. Adding an ESP-to-UPDI connection is unnecessary for the preferred UART-bootloader architecture and would create a separate programming and electrical-protection problem.

The pilot has proved the reset-based handoff and one interrupted-update recovery with the earlier protocol. BL0.3 must repeat those tests, including a true power removal, before deciding whether seamless browser-session survival justifies a board revision or fleet rework.

### Update protocol and safety requirements

The ESP should stage a compact application binary plus a manifest rather than forward an unchecked Intel HEX stream from an open browser connection. At minimum, the manifest and protocol must bind:

- product and processor identity;
- bootloader protocol and minimum bootloader version;
- application start address and maximum length;
- application version and downgrade policy;
- payload length and cryptographic hash;
- the deliberate operator-authorization method;
- EEPROM schema compatibility or a declared migration;
- per-page sequence, retry, and integrity information.

CRC, hashes, and readback detect corruption but do not authenticate the firmware publisher. Publisher authentication is intentionally outside this fleet's requirements. The final-four-character SSID prompt provides a deliberate maintenance-mode condition without requiring access to the PCB-mounted switch. The ESP compares against the MAC-derived device identity even if it is currently advertising `Tx_Master`.

The transfer must use numbered Flash pages with acknowledgements, bounded retries, and final AVR-side readback or whole-image verification. It must never write the Boot section, fuses, lock bits, EEPROM, or User Row unless a separately reviewed provisioning or migration design explicitly requires that region. The current 9600-baud Linkbus rate gives a raw transfer floor of roughly 43 seconds for a 40.7 KB image before framing, erase, write, and verification overhead. A bootloader-specific higher baud rate is reasonable after hardware qualification; correctness and recovery matter more than minimizing update time.

Bootloader entry must be refused during an active or imminent event, RF keying, insufficient supply margin, or an unresolved hardware fault. Both processors must report progress and final identity without treating a lost browser socket as proof of failure or success.

### Pilot and fleet-release checklist

Do not expand beyond the pilot until the project has verified all of the following:

1. The temporary access-point loss and automatic reconnection behavior on every supported operator device.
2. Immediate ESP repowering by the bootloader after software reset, brownout, and external reset.
3. Boot section size, application offset, fuse values, vector behavior, and the combined initial-provisioning artifact by independent readback.
4. Image hash, SSID-suffix authorization, version/downgrade policy, and bootloader compatibility rules.
5. Persistent update-marker locations and their compatibility with the existing 512-byte EEPROM schema.
6. ESP staging-space preflight, atomic file handling, and behavior when LittleFS is full or damaged.
7. Power threshold, brownout behavior, RF-safe state, event exclusion, and watchdog behavior during update.
8. Page retry, resume, final verification, rollback, and invalid-application recovery semantics.
9. Pilot-unit and fault-injection tests covering resets or power loss before staging, during every page phase, after verification, and during first application boot.
10. A one-time UPDI fleet-provisioning and rollback plan for every unit that should support later wireless updates.

This remains a boot-chain, fuse-layout, release-integrity, and recovery project rather than a small extension of the WebSocket command set. BL0.3 is not fleet-qualified until the dedicated automated pilot, true power-interruption, rollback, and two-unit tests are recorded.

## Read-only Mac probe

After joining the FlexFox SSID, run:

```text
just wifi-probe
```

The probe uses Node's built-in HTTP and WebSocket clients. It:

1. verifies the root HTTP page;
2. opens the port-81 WebSocket;
3. sends only `SSID`, `MAC`, `SW_VERSIONS`, `MASTER`, and the `!&` heartbeat;
4. waits for the ESP's automatic live AVR temperature and battery requests;
5. fails unless both ESP identity replies and live AVR replies are observed.

It does not synchronize the clock, change settings, load or execute an event, key the transmitter, save EEPROM, shut off WiFi, or use raw pass-through.

For an extended Android-assisted hardware session, prepare the preferred ADB relay and hold the safe WebSocket open:

```text
just wifi-adb-monitor Tx_<unit-specific-eight-hex-characters>
```

Monitor mode performs the same initial proof, then sends only `!&` every five seconds until interrupted with Ctrl-C. The ESP disconnects a WebSocket after approximately ten seconds without text traffic, so a 30-second heartbeat cannot preserve one continuous socket. Five seconds leaves margin for scheduling delay while reducing the built-in pages' two-second heartbeat rate. This resets the ESP WebSocket activity timer and allows the ESP/AVR keep-alive behavior to remain active while the Moto stays associated with the FlexFox AP.

On an explicitly authorized dummy-loaded unit, the guarded role-assignment regression requires a previously recorded event and restoration role:

```text
FLEXFOX_ROLE_ASSIGNMENT_DRY_RUN=1 \
FLEXFOX_ROLE_ASSIGNMENT_EXPECT_EVENT='Classic 80m Set 1-1' \
FLEXFOX_ROLE_ASSIGNMENT_EXPECT_ROLE=1:0 \
just wifi-role-assignment-test

FLEXFOX_ROLE_ASSIGNMENT_TEST=1 \
FLEXFOX_ROLE_ASSIGNMENT_EXPECT_EVENT='Classic 80m Set 1-1' \
FLEXFOX_ROLE_ASSIGNMENT_EXPECT_ROLE=1:0 \
just wifi-role-assignment-test
```

Substitute the unit's read-back event and role; do not copy the example blindly. The test refuses to run without both expected values, confirms them before mutation, exercises one transmitter assignment per configured role, queries each role's frequency and power, and restores and reloads the expected assignment during normal or handled-error cleanup. It never sends `EXECUTE`, raw `PASS`, time writes, or RF commands. Event-list generation can exceed ten seconds on the deployed ESP, so the harness sends each explicit event request once and allows 30 seconds rather than duplicating the state transition.

`just wifi-linkbus-bounds-test` is a narrow writable-test-unit qualification for the AVR receive parser. It first proves that raw `PASS,$TEM?` and `PASS,$BAT?` produce fresh replies. It then sends two malformed field frames under the unrecognized `ZZZ` message ID and the overlength read-only alias reproducer `$AZRX?`, followed by both recovery queries. It passes only if each dropped frame remains unanswered through the ESP retry delay and fresh temperature and battery replies prove that the queued valid frames were parsed. Requiring both replies after the delay prevents an incidental periodic broadcast from satisfying the check.

The same recipe sends the read-only `$RXW?` probe twice. The legacy decimal ID encoding aliases `RXW` to `TEM` and therefore produces a visible temperature reply for every probe; the collision-free parser instead returns a Linkbus NAK, which the ESP consumes without broadcasting. The test fails only if both short observation windows contain temperature replies, tolerating one incidental periodic update, then requires a fresh battery reply to prove recovery. Do not generalize this recipe into arbitrary `PASS` forwarding; raw pass-through also exposes configuration, RF, reset, clock, EEPROM, and WiFi-shutdown commands.

To observe clock phase without setting time or changing configuration, run:

```text
just wifi-clock-observe
```

The observer sends only the `!&` heartbeat every five seconds and samples the ESP's `SYNC,<epoch>` broadcasts from the AVR. The five-second interval is required by the deployed module's approximately ten-second WebSocket inactivity timeout; a 30-second interval does not preserve one continuous observation. It reports the Mac receive time, target epoch, median offset, and sample spread. A positive offset means the target-reported epoch is behind the Mac; a negative offset means it is ahead. Absolute offset includes WebSocket and USB-tunnel latency, so compare unit medians only through the same network path. See the [wireless time synchronization investigation](Evidence/WIRELESS_TIME_SYNC_INVESTIGATION_2026-07-12.md) for the protocol limitations and multi-unit measurement plan.

On an explicitly authorized dummy-loaded test unit, qualify RTC writes with:

```text
FLEXFOX_CLOCK_SYNC_DRY_RUN=1 just wifi-clock-sync-test
FLEXFOX_ALLOW_CLOCK_SET=1 FLEXFOX_CLOCK_SYNC_TRIALS=10 just wifi-clock-sync-test
```

This state-changing test alternates distinctive `+8`, `-8`, and current-time signatures so each RTC write can be distinguished from the previous value. It requires every signature in the AVR's returned epochs and restores current Mac time after the series or a handled failure. Run it only when brief schedule changes are acceptable. It does not change event files, EEPROM configuration, or RF settings.

To distinguish persistent RTC-edge phase from transient report-delivery delay on the same authorized unit, run:

```text
FLEXFOX_CLOCK_PHASE_DRY_RUN=1 just wifi-clock-phase-test
FLEXFOX_CLOCK_PHASE_TEST=1 just wifi-clock-phase-test
```

This characterization enters clone quiet mode, collects baseline one-shot RTC-edge reports, performs queued `$TIM,...,C;` writes at a controlled Mac phase, and collects three consecutive one-shot edges after every write. It records linear receipt delay and circular modulo-one-second phase statistics in ignored `Software/Huzzah/tmp/clock-phase-latest.json`. It restores current Mac time and requests normal-report resumption on completion or a handled failure. A delayed first report followed by normal later edges identifies a transient observation-path stall; a persistent phase change should remain visible in the following edges. The route still contributes latency, so final product qualification requires two physical units.

Useful overrides:

```text
FLEXFOX_PROBE_DRY_RUN=1 just wifi-probe
FLEXFOX_URL=http://73.73.73.73/ FLEXFOX_PROBE_TIMEOUT_MS=15000 just wifi-probe
FLEXFOX_PROBE_DRY_RUN=1 just wifi-monitor
FLEXFOX_CLOCK_DRY_RUN=1 just wifi-clock-observe
FLEXFOX_CLOCK_SAMPLES=30 FLEXFOX_CLOCK_TIMEOUT_MS=120000 just wifi-clock-observe
FLEXFOX_CLOCK_PHASE_BASELINE=12 FLEXFOX_CLOCK_PHASE_TRIALS=30 FLEXFOX_CLOCK_PHASE_EDGES_PER_WRITE=3 FLEXFOX_CLOCK_PHASE_TEST=1 just wifi-clock-phase-test
```

## Safety classification

### Safe initial observations

- HTTP root and static page retrieval;
- `SSID`, `MAC`, `SW_VERSIONS`, and query-only `MASTER`;
- the automatic temperature and battery queries sent at WebSocket connection;
- WebSocket heartbeat `!&`.

### State-changing operations

- `SYNC` writes the AVR RTC;
- callsign, pattern, frequency, power, modulation, speed, and event commands change live configuration;
- `CLEAR`, `PREP`, `EXECUTE`, and `MASTER,<value>` change event or ESP state;
- `WIFI_OFF` can remove the active access path.

### RF-active or hazardous operations

- `XMIT` begins immediate transmission when interlocks permit;
- `KEY_DOWN` and raw `$KEY,[;` key the transmitter;
- `/radio.html` exposes keying and arbitrary manual Morse input;
- `PASS,<text>` forwards arbitrary text to the AVR Linkbus without command allow-listing.

The current bench unit is connected to a dummy load, but RF-active commands still require an explicit test objective, expected response, stop command, and independent observation. Do not use `PASS` during initial connectivity testing.

## Mac networking

### Preferred Android ADB relay

The default Android-assisted debugging path is the repository-owned ADB relay:

```text
just wifi-adb-probe Tx_<unit-specific-eight-hex-characters>
just wifi-adb-monitor Tx_<unit-specific-eight-hex-characters>
```

Use `Tx_Master` when the target is intentionally advertising the master alias. The recipes require one authorized USB-debuggable Android device, command the Moto to join the complete expected SSID, and fail closed if the ADB device or SSID is ambiguous. USB tethering is not required.

The shared relay helper starts two loopback-only `toybox nc -L` listeners on the Moto. Each accepted connection reaches only `73.73.73.73:80` or `73.73.73.73:81`; `adb forward` exposes those listeners to the Mac as `127.0.0.1:18080` and `127.0.0.1:18081`. The Mac keeps its existing WiFi, default route, DNS, and internet access. No RNDIS interface, `utun`, administrator prompt, or macOS route change is involved. The guarded fleet-upgrade workflow uses this same helper and automatically reassociates the exact target after an ESP restart.

After `just wifi-adb-relay <ssid>`, other host tools can use:

```text
FLEXFOX_URL=http://127.0.0.1:18080/ \
FLEXFOX_WEBSOCKET_URL=ws://127.0.0.1:18081/ \
just wifi-probe
```

Rerun the relay recipe after an ADB-server restart, USB reconnection, phone reboot, or target change. Always require the subsequent probe to return the complete expected `SSID`, `MAC`, `SW_VERSIONS`, `MASTER`, temperature, and battery data before any state-changing operation.

### DroidTether fallback

DroidTether is retained only for an environment where ADB forwarding or Android `toybox nc -L` is unavailable. Its proven simultaneous-connectivity arrangement is:

1. keep the Mac WiFi interface on its normal network so its default route and internet access remain unchanged;
2. associate the Moto with the FlexFox `Tx_...` access point;
3. connect the Moto to the Mac by USB and enable Moto USB tethering;
4. start DroidTether with default routing disabled;
5. add a host-only route for `73.73.73.73` through the `utun` interface created by DroidTether;
6. keep DroidTether running for the entire FlexFox session;
7. run `just wifi-probe`, followed by `just wifi-monitor` for extended work.

The DroidTether `utun` number can change between sessions. Identify the active interface rather than permanently assuming `utun6`. Verify the IPv4 routing table before probing:

- the ordinary `default` destination must still use the Mac WiFi interface and ScharStar 2 gateway;
- `73.73.73.73` must be the narrow host route through the active DroidTether `utun`;
- DroidTether must not install a competing default route.

In the first successful session, the default route remained on `en0` through `10.0.4.1`, while only `73.73.73.73` used `utun6`.

### DroidTether diagnostics learned on the Moto

Before starting DroidTether, the Moto must actually expose the RNDIS USB function. With Android debugging available, verify:

```text
adb shell getprop sys.usb.state
```

The value should include `rndis` (the proven state was `rndis,adb`). A value of only `adb` exposes the Android Debug Bridge interface `ff/42/01`, not RNDIS. DroidTether v0.8.7's broad known-Motorola fallback can misclassify that ADB interface as RNDIS and then misleadingly time out waiting for `INIT_CMPLT`. Confirm the phone's **USB tethering** switch remains enabled before diagnosing the Mac route.

The Moto may refuse to autojoin the open FlexFox AP because Android labels it weak security. Retain ADB alongside RNDIS so the host can make the association without touching the phone. If RNDIS currently owns USB exclusively, stop DroidTether, turn USB tethering off once, leave USB debugging enabled, and approve the Mac authorization prompt. Then verify and reconnect the exact unit with:

```text
adb shell cmd wifi status
adb shell cmd wifi connect-network Tx_<unit-specific-eight-hex-characters> open
adb shell svc usb setFunctions rndis
adb devices -l
```

On the tested Moto, `svc usb setFunctions rndis` retained debugging and re-enumerated as combined RNDIS+ADB. This lets ADB reconnect the saved `Tx_...` network after every ESP restart while DroidTether supplies the Mac route. Read `cmd wifi status` afterward and require the complete expected SSID (and its ESP BSSID when known) before any update. If ADB disappears, return to the one-time tether-off authorization sequence rather than guessing at the attached unit.

Do not assume a fixed DroidTether client subnet. The Moto has assigned `10.154.x.x`, `10.75.18.x`, and `10.233.164.x` leases in successful sessions. Locate the newly created `utun` carrying an IPv4 address, then install the `73.73.73.73` host route through that interface. A helper that recognizes only one historical subnet can stop a fully successful RNDIS/DHCP session and falsely report that no tunnel was created.

A successful path has four separate proofs:

1. the RNDIS handshake reaches data mode;
2. DHCP configures an IPv4 address on the new `utun`;
3. `route -n get 73.73.73.73` names that `utun`, not `en0`;
4. `just wifi-probe` returns HTTP, ESP identity, temperature, and battery data.

Do not change macOS routing, Internet Sharing, the FlexFox AP address, or the ESP firmware merely to make the first smoke test convenient. First prove the default direct path, then document any repeatable dual-network arrangement separately.
