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

### Implementation status — ESP 2.16 / AVR 0.207 / bootloader BL0.1

The bootloader transport is source-, build-, and pilot-hardware-qualified, but the ESP 2.16 / AVR 0.207 pair is source- and build-qualified only and is not yet fleet-qualified. AVR 0.207 retains the 0.203 scheduled-start sleep correction and 0.204 relocated bootloader handoff, capability query, and guarded update entry. It restores the operator LED contract after a runtime suspend and makes equal start/finish epochs a valid persistent disabled state: slow red means an event will run without further action, while fast red means no event can run until the user acts. It reuses SignalSlinger's tested AVR DA page protocol and reset-vector-last recovery model, adapted for the FlexFox power topology:

ESP 2.16 retains the browser `CLEAR` correction introduced in 2.14 and adds an explicit persistent-disable path. In `events.html`, Apply sends equal start/finish epochs through the normal full event transaction; AVR 0.207 saves both values, stops RF, and reports no event to run. The guarded `$UPD,START,SSID;` AVR-update handoff remains stronger for firmware work: it independently suspends RF, stores the current epoch as the completed finish, verifies that EEPROM write, and only then resets into the resident bootloader.

- a 16 KiB resident Boot section occupies `0x0000–0x3FFF` (`BOOTSIZE=0x20`);
- the FlexFox application is linked at `0x4000` (`CODESIZE=0x00`);
- the bootloader reasserts AVR PA5 (`WIFI_ENABLE`) and releases PA6 (`WIFI_RESET`) immediately after reset;
- ESP 2.16 stages a page-aligned application image and dual-slot recovery state in LittleFS before requesting bootloader entry;
- the ESP accepts update start only after the operator enters the final four characters of the unit's MAC-derived `Tx_...` device SSID;
- the AVR application accepts the internal `$UPD,START,SSID;` handoff only while the event, manual transmission, and RF output are idle;
- raw WebSocket `PASS` forwarding cannot send `$UPD` frames and bypass the dedicated update route;
- bootloader traffic uses USART1 PC0/PC1 at the hardware-qualified 9600 baud with CRC-16 on every frame and AVR readback CRC on every page;
- the application reset-vector page is erased first and written last, so interruption leaves the immutable bootloader in control;
- EEPROM byte 511 is reserved as a pre-erase update marker, closing the power-loss window before the reset-vector page has been invalidated;
- the staged file is retained until the new application reports the manifest's target version.

Build the complete one-time install and later wireless-update artifacts with:

```text
AVR_TOOLCHAIN_ROOT=Software/AVR128DA48/tmp/avr8-gnu-toolchain-darwin_x86_64 \
AVR_DFP_ROOT=Software/AVR128DA48/tmp/AVR-Dx_DFP/1.9.103 \
just avr-boot-chain-build
```

The ignored output directory `Software/AVR128DA48/tmp/avr-boot-chain/` contains the combined first-install HEX, the page-aligned wireless update BIN, and release metadata with hashes, geometry, versions, and required fuses. The current exact build produces a 2,512-byte bootloader inside the 16,384-byte reservation and a 43,008-byte AVR 0.207 update image; recalculate these values from each release build.

One-time UPDI provisioning is deliberately destructive and double-gated. It captures EEPROM and fuses, erases and verifies the combined image, restores the unit-specific EEPROM with only newly reserved byte 511 forced to erased `0xFF`, writes only `CODESIZE` and `BOOTSIZE`, and independently re-reads the target:

```text
FLEXFOX_UNIT_ID=<fleet-label> \
FLEXFOX_PROVISION_CONFIRM=PROVISION-BOOTLOADER \
FLEXFOX_FUSE_CONFIRM=WRITE-BOOTSIZE-0x20 \
just avr-provision-boot-chain
```

After the matching ESP and boot chain are installed on a qualified pilot, a later update can use the `/avr-update` page or the guarded host workflow:

```text
FLEXFOX_AVR_UPDATE_CONFIRM=UPDATE-AVR-0.207 just wifi-avr-update
```

The interactive workflow reports the unique device SSID and prompts for its final four characters. An explicitly unattended invocation must supply both `FLEXFOX_EXPECTED_DEVICE_SSID=Tx_<eight-hex-characters>` and `FLEXFOX_AVR_SSID_SUFFIX=<last-four>`. The exact expected SSID is checked before staging, preventing a valid image from being placed on the wrong unit when several FlexFoxes share the same default IP address. `just wifi-avr-preflight` reports the connected device identity and update state without staging an image.

The host allows 30 minutes by default for all 84 pages, reset-vector finalization, ESP restart, and application-version confirmation. The ESP intentionally stops HTTP service while it owns the UART and programs the AVR, so status requests may receive no response during otherwise healthy programming. A host-side timeout is never permission to remove power: keep the unit powered and inspect `/avr-update/status`. The host reports a persisted page diagnostic as soon as HTTP returns. `FLEXFOX_AVR_VERIFY_TIMEOUT_MS` may override the window with an integer from 60,000 through 3,600,000 milliseconds.

Do not provision the fleet merely because both firmware builds pass. Keep the pilot chassis accessible until boot, ordinary RF/event behavior, WiFi power-down/wake, a complete wireless update, and repeated physical power interruption during erase/write/final handoff have all passed.

### Pilot qualification result — 2026-07-18

The connected pilot completed both directions of a real wireless application update after the one-time UPDI boot-chain installation:

- AVR 0.204 was updated wirelessly to 0.205, all 83 pages completed, the staged file was removed, and the new application reported version 0.205 over Linkbus;
- AVR 0.205 was then rolled back wirelessly to the intended fleet candidate, AVR 0.204;
- while the rollback was in progress, an Atmel-ICE session interrupted the target after the application reset-vector page had been erased; readback showed the `0x4000` vector page erased and EEPROM update marker byte 511 cleared;
- after the programmer released the target, the resident bootloader and ESP recovery state resumed without restaging or physical intervention, completed all 83 pages, and AVR 0.204 reported its exact version over Linkbus;
- ESP 2.12 retained LittleFS throughout and reported 606,208 bytes free after removing the completed 42,496-byte staged image;
- the pilot's temporary event cancellation was reversed: the restored LittleFS event read back with its original hash, all 512 AVR EEPROM bytes matched the pre-cancellation capture, and live AVR SRAM reported the original finish epoch `1784402100` (`2026-07-18T19:15:00Z`).

This establishes one successful end-to-end update, downgrade, and genuine post-erase interruption recovery. It does not replace repeated power-loss testing at several page positions, normal WiFi wake/power-down regression, ordinary event/RF regression, or a second pilot before fleet rollout.

The exact ESP 2.16 / AVR 0.207 two-unit procedure, candidate hashes, per-unit preservation rules, LED checks, interruption gate, disabled-event Apply check, and final disposition are prepared in [WiFi AVR two-unit qualification](Evidence/WIFI_AVR_TWO_UNIT_QUALIFICATION_2026-07-18.md). Record the hardware observations there rather than treating a successful updater message as complete qualification.

BL0.1 intentionally uses integrity checks and deliberate operator authorization rather than cryptographic firmware signatures. The host verifies the release SHA-256, the ESP verifies the complete uploaded CRC-32, and every bootloader frame and programmed page is CRC-checked. Entry requires the final four characters of the unique MAC-derived ESP SSID, but that suffix is an operator-error interlock rather than a secret: it is readily visible to the person connected to the unit. This matches the deployment model in which the ESP is normally reachable for only two minutes after power-up, has short range, and is used predominantly in rural locations.

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

The selected layout reserves 32 512-byte pages (16 KiB), places the application at `0x4000`, and leaves 112 KiB for application code. Exact sizes and section boundaries are checked by the build/package scripts rather than copied from this note. The pinned ESP 2.12 build leaves 36,776 bytes of dynamic-memory headroom and 5,092 bytes of IRAM headroom; IRAM remains the tighter margin and must be checked on every release build.

The qualified ESP profile provides a 1,024,000-byte LittleFS partition with 8 KiB allocation blocks. The current factory image measures 688,128 bytes free. A staged 42,496-byte AVR image plus its recovery state leaves 638,976 bytes free; even the transient old-image-plus-replacement case leaves 589,824 bytes. Before opening the staging file, ESP 2.12 reads `LittleFS.info()`, rounds the declared image size to whole filesystem blocks, and requires four additional free blocks (32 KiB on this profile) for metadata and recovery-state changes. The status endpoint and update page report measured total, used, and free bytes.

### AVR reset cuts power to the ESP

The important hardware complication is the ESP power-enable circuit. AVR PA5 drives `WIFI_ENABLE`, which drives the shutdown input of LT1763 regulator U306. R310 is a 10 kOhm pull-down from `WIFI_ENABLE` to ground. When the AVR is reset, PA5 becomes high-impedance, R310 disables U306, and the ESP loses power. The AVR also controls the ESP reset line; the ESP does not have an existing reciprocal AVR-reset control.

The implementation does not assume that a live ESP or browser socket survives AVR reset. It deliberately uses persistent ESP state and reset-based bootloader entry.

#### Rejected seamless-session alternative: direct entry without reset

A direct application-to-bootloader jump could preserve the browser session, but it was not selected for BL0.1 because it carries more untestable C-runtime, stack, vector, and peripheral handoff state. The reset-based implementation accepts a temporary ESP power cycle and resumes from LittleFS instead.

1. The ESP receives the complete candidate into a temporary LittleFS file, validates it, and atomically promotes it to a staged update.
2. The ESP sends an explicit bootloader-entry command over Linkbus.
3. The AVR refuses entry while RF is active or power conditions are unsuitable, then forces the transmitter into a safe state, quiesces Linkbus, disables interrupts, and deliberately leaves `WIFI_ENABLE` asserted.
4. The AVR transfers control to the bootloader without issuing a processor reset. The bootloader establishes its own stack, vector selection, peripheral state, and USART protocol before altering application Flash.
5. The ESP and bootloader transfer, write, acknowledge, and verify the complete application.
6. Only after successful final verification does the AVR reset or otherwise enter the new application. A final reset may reboot the ESP and drop the browser connection, but the programming transaction is already complete.

Execution does not have to begin at reset for code in the AVR Boot section to program the Application section. This direct handoff must nevertheless be treated as boot-chain code: it needs explicit assembly/linker review, generated-code inspection, and connected-target fault testing. It must not depend on ordinary C/C++ startup state accidentally remaining valid across the handoff.

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

The pilot has now proved the selected reset-based handoff and one interrupted-update recovery. Repeat interruption and normal power-management testing before deciding whether seamless browser-session survival justifies a board revision or fleet rework.

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

This remains a boot-chain, fuse-layout, release-integrity, and recovery project rather than a small extension of the WebSocket command set. BL0.1 is not fleet-qualified until the dedicated pilot and rollback tests are recorded.

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

For an extended hardware session, keep DroidTether running and hold the safe WebSocket open:

```text
just wifi-monitor
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

The proven simultaneous-connectivity arrangement is:

1. keep the Mac WiFi interface associated with ScharStar 2 so its normal default route and internet access remain unchanged;
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

Do not assume a fixed DroidTether client subnet. The Moto has assigned both `10.154.x.x` and `10.75.18.x` leases in successful sessions. Locate the newly created `utun` carrying an IPv4 address, then install the `73.73.73.73` host route through that interface. A helper that recognizes only `10.154.x.x` can stop a fully successful RNDIS/DHCP session and falsely report that no tunnel was created.

A successful path has four separate proofs:

1. the RNDIS handshake reaches data mode;
2. DHCP configures an IPv4 address on the new `utun`;
3. `route -n get 73.73.73.73` names that `utun`, not `en0`;
4. `just wifi-probe` returns HTTP, ESP identity, temperature, and battery data.

Do not change macOS routing, Internet Sharing, the FlexFox AP address, or the ESP firmware merely to make the first smoke test convenient. First prove the default direct path, then document any repeatable dual-network arrangement separately.
