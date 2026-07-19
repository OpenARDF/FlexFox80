# WiFi AVR Two-Unit Qualification — 2026-07-18

**Status:** Unit A passes ESP 2.22 / BL0.3 interruption and exact-readback gates; ESP 2.23 supersedes 2.22 after a separately recovered ordinary-file-upload interruption; remaining normal-operation, second-unit, and soak gates are unchanged

## Objective

Qualify the ESP 2.22 / AVR 0.208 / BL0.3 protocol-2 combination on two identified, dummy-loaded FlexFox80 units before expanding the boot chain to the fleet. The test must prove wireless updating, autonomous recovery from ESP restart and AVR reset, true power-loss recovery, exact post-update Flash/EEPROM/fuse readback, normal transmitter behavior, persistent event disabling through Apply, and unambiguous physical update feedback. It must also prove the operator LED contract:

- slow red means an event will run without another user action;
- fast red means no event can run until the user acts.

Retained future start and finish epochs are not enough to justify slow red after an event has been suspended.

## Frozen candidate

| Item | Value |
| --- | --- |
| Source commit | Commit containing this qualification record |
| ESP version | `2.22` |
| AVR version | `0.208` |
| Bootloader version | `BL0.3`, protocol 2, 38,400 baud candidate |
| ESP sketch | 559,728 compiled bytes; 563,888-byte `.bin`; SHA-256 `c42a1d1a7b4ee994a445cb14ef925d79d7ad2dc0c3fd65250f0732f0fcb3dcf8`; installed MD5 `ce7ed4ed788edb346e05f8e87b36047e` |
| AVR wireless image | 43,520 bytes; CRC32 `0xdcf1e479`; SHA-256 `c27442a55db0bd27a09129ce43fb58b36acca4cd16890e8a71b8d013feddfe81` |
| AVR first-install HEX | SHA-256 `5b84d8b95942c58cef7dd3ddb54b7dc3db894847035327de285d0898ff453660` |
| AVR release manifest | SHA-256 `2a507075e7d20b97e475f86d4740371555b3239ca2f02e2015b07a68522d7e13` |
| Boot layout | 5,112-byte bootloader in a 16,384-byte reservation; application at `0x4000`; SHA-256 `6d0997d0732566d66060970d3b7026654d7336aaa00483ae903c49c9e2fb78bc` |
| ESP resources | 53% sketch flash; 62% global RAM; 30,944 bytes dynamic-memory headroom; 5,092 bytes IRAM headroom; zero warnings |
| AVR resources | 41,894 text + 1,150 data bytes; 1,576 BSS bytes; zero warnings |

The original ESP 2.18 / BL0.3 / AVR 0.208 candidate passed the source/build gates but failed its first protocol-2 hardware run at the final commit boundary. ESP 2.19 removed the redundant per-page LittleFS recovery-state rewrite exposed by that run and retained periodic journal checkpoints plus atomic phase-boundary state. ESP 2.22 persists both one-shot qualification hooks across the normal ESP power loss, makes ambiguous staging responses identity/size/CRC-verifiable, and supports Moto-side qualification synchronization. It passed `just check`, a zero-warning ESP build, protected installation, and all Unit A boot-chain recovery gates.

ESP 2.23 now supersedes that sketch for deployment. A fleet unit lost power between the ordinary `/test.html` backup and promotion renames; exact flash extraction proved that all unit data and both complete transaction copies survived, but synchronous startup cleanup prevented HTTP from becoming available before the AVR removed ESP power. ESP 2.23 reuses the qualified keep-alive path during ordinary uploads and limits startup recovery to one metadata rename. The repaired unit passed standalone exact-file verification and the combined ESP 2.23 / BL0.3 / AVR 0.208 fleet gate after reinstall. See [ESP interrupted file-upload recovery](ESP_INTERRUPTED_FILE_UPLOAD_RECOVERY_2026-07-19.md).

BL0.2 is retained only as historical evidence and must not be installed on additional units. Review found four fleet-significant gaps: reset-last was not enforced by the resident bootloader, a persistent POR/BOR recovery request could preserve the wrong ESP power state, a corrupt staged image after erase could suppress the recovery web service, and a single serial `U` could enter the startup boot path. BL0.3 enforces an ordered image-bound session, product trailer, payload/full-image CRC, bootloader-owned reset-page commit, normal cold ESP power sequencing, recoverable invalid-image service, and a CRC-framed `UBL2!` serial entry.

## Unit identities and starting state

Do not proceed from a handwritten chassis label alone. Read each identity from the running device and, when UPDI provisioning is required, from the connected AVR. The two units share the same default IP address, so update them sequentially and reconnect the Moto to the intended SSID before every host operation.

| Field | Unit A | Unit B |
| --- | --- | --- |
| Bench role | Atmel-ICE pilot | Second pilot |
| Device SSID | `Tx_7C2D6FD3` | Pending readback |
| ESP MAC | `86:A8:24:2F:96:5B` | Pending readback |
| AVR serial | Pending retained provisioning record | Pending readback |
| Initial `SW_VERSIONS` | `2.15,0.204` after precursor ESP installation | Pending readback |
| Initial `MASTER` state | `0` | Pending readback |
| Dummy load confirmed | Yes; operator confirmed before both firmware writes | Pending operator observation |
| Event files and `.me` assignments captured | Pending | Pending |
| AVR EEPROM and fuses captured if provisioning | Existing record; verify before reuse | Pending if provisioning |
| Boot chain already present | Yes | Pending probe |

Unit A has a special final-state requirement. Do not reinstall its exact original future event. Keep its intended event file installed with a completed finish time so a later power cycle cannot restart the test event. Unit B's captured event files, assignments, master state, frequency, power, and callsign remain unit-specific; never restore them from Unit A's backup.

### Unit A AVR 0.207 precursor result and release-blocking finding

Unit A was identified live as `Tx_7C2D6FD3`, ESP MAC `86:A8:24:2F:96:5B`, `MASTER,0`, initially reporting `SW_VERSIONS,2.15,0.204`, 11.5 V battery, and 31-33 C temperature. The operator confirmed the dummy load and confirmed RF had stopped after an explicit browser `CLEAR` before either firmware write.

The protected ESP updater accepted the frozen 557,984-byte sketch, returned HTTP 200, rebooted into ESP 2.16, proved the uptime reset, and continued to report LittleFS protection. Pre-update hashes of the unit's `.event` and `.me` files were not captured, so this run does not satisfy the file-preservation gate even though no filesystem update was requested.

The AVR preflight identified the same complete SSID and staged no image. The subsequent identity-locked AVR transaction validated the 43,008-byte image with CRC32 `0x34836fc8`, left 536 KiB free while staged, and visibly progressed through red/green AVR page activity. The original host verifier expired after ten minutes while programming was still active; the operator did not interrupt power. The device subsequently reported `phase=complete`, `nextPage=84`, `pageCount=84`, `staged=false`, target version `0.207`, and 598,016 LittleFS bytes free. This was a host false negative, not a device failure. The host default verification window is therefore 30 minutes for subsequent qualification runs, and persisted programming diagnostics are surfaced immediately when HTTP service returns.

An independent WebSocket probe then reported `SW_VERSIONS,2.16,0.207`, the same SSID and MAC, advancing clock reports, 11.5 V battery, 33 C temperature, and live AVR communication. After leaving `radio.html`, Unit A was physically power-cycled with no browser `CLEAR`; RF remained off and the AVR returned to fast red. A second independent probe again reported `SW_VERSIONS,2.16,0.207`, and `/avr-update/status` still reported complete, 84/84, and no staged image.

This passed the Unit A transport installation, installed-version verification, staged-image cleanup, and immediate post-power-cycle RF/LED safety for AVR 0.207. It did not qualify the successor candidate.

Live `events.html` testing then exposed two issues. Android's native date/time chooser closed when the AVR's approximately one-second `EVENT_DATA` response replaced the selected row; `events.html` 0.5.9 retained the focused editor and the operator confirmed that the chooser remained open. A future event Apply completed and RF remained off, but both AVR LEDs stayed dark because the shared indicator timeout had expired before the transaction. Equal-time Apply still persisted the disabled schedule, produced fast red plus the expected dummy-load green indication, and kept RF off, but the page displayed `Undefined status - update file system` for AVR status `0xFA`.

Those findings supersede AVR 0.207 as the fleet candidate. AVR 0.208 re-arms the mature LED state machine after every successful event Apply. `events.html` 0.6.0 maps `0xFA` to `No event will run`, preserves the focused date/time editor, and restores horizontal centering of the Sync control. Unit A must receive 0.208 and repeat the LED/schedule contract before its candidate installation gate can pass.

### Unit A AVR 0.208 application result

Unit A staged the exact 43,008-byte AVR 0.208 image with CRC32 `0x6dc14ab5` under ESP 2.16 and BL0.1. The Moto/USB route disappeared during programming, so the host could not read the final HTTP record. The operator nevertheless observed AVR and ESP return to their normal application patterns for approximately one minute before the ordinary display/WiFi timeout made both dark. A brief PCB-switch press did not wake WiFi because, in that timed-out state, the first release only re-arms the AVR LED display; it is not a WiFi wake command.

Rather than infer success from those indications, an Atmel-ICE session independently read Flash, all 512 EEPROM bytes, and all 16 fuse bytes without writing them. The application region matched the frozen AVR 0.208 image exactly: 42,770 programmed bytes plus 238 expected erased padding bytes, corresponding to SHA-256 `1856561ecb1782f39efb628005fd0bcb899fbc79de74e5dea41246401f02b49f`. The update marker at EEPROM byte 511 was erased (`0xFF`), and the boot-layout fuses remained `CODESIZE=0x00`, `BOOTSIZE=0x20`. This definitively passes the Unit A 0.208 application-installation gate despite the lost network return path.

The same EEPROM capture showed start `1784406000` and finish `1784406043`. The updater deliberately replaced finish with its safe handoff time, so the event is expired and cannot run but is no longer the equal-time disabled representation. Unit A must repeat equal-time Apply under 0.208 and receive an independent equality readback before that separate gate passes.

### Unit A ESP 2.18 / BL0.3 commit-boundary failure

Unit A was positively reidentified as `Tx_7C2D6FD3`, ESP MAC `86:A8:24:2F:96:5B`, ESP 2.16, and AVR 0.208 before any write. The protected sketch updater installed the exact ESP 2.18 image and proved the rebooted 563,312-byte sketch MD5 plus `filesystemProtected=true`. Atmel-ICE `J41800053674` then identified an AVR128DA48 revision 1.7 at 3.24-3.28 V. A complete pre-write Flash/EEPROM/fuse capture showed the installed BL0.1 9,600-baud bootloader, application 0.208, `CODESIZE=0x00`, and `BOOTSIZE=0x20`. The guarded one-time provisioning script installed and independently verified BL0.3 plus the relocated application, restored all 512 EEPROM bytes, and verified the unchanged fuses.

The unattended qualification run staged the exact 43,520-byte image with CRC32 `0xdcf1e479`, measured 536 KiB LittleFS free, entered BL0.3, and kept AVR red/green alternating while ESP blue remained solid. It did not return HTTP or commit the reset page before the host's 30-minute deadline. This is a real candidate failure, not the earlier ten-minute host false negative.

After approximately 16 minutes, a read-only Atmel-ICE capture intentionally reset the stalled target and preserved the following evidence:

- the 5,112-byte Boot section prefix exactly matched BL0.3 SHA-256 `6d0997d0732566d66060970d3b7026654d7336aaa00483ae903c49c9e2fb78bc`;
- all 512 bytes at application address `0x4000` were erased (`0xFF`), proving bootloader-owned reset-last safety remained intact;
- every returned byte from `0x4200` through the final non-erased byte matched the wireless update image;
- the final product-trailer page at `0xE800` contained the exact `FF80UPD2` geometry, payload CRC, and `0.208` version record;
- all fuses still matched SHA-256 `d1edc2ade950e20b08c4fefd45ebbc8af2d6fc36fcad876c9457328c6d2925f5`.

The capture therefore places the stall after the final non-reset page was available and before reset-page commit. ESP 2.18 rewrote the dual-slot LittleFS recovery state and flushed a journal entry after every verified page even though protocol 2 deliberately discards that progress and replays from page 1 after every restart. A failed phase-boundary CRC or UART response remains possible until `/avr-update.log` is retrieved, but the redundant final per-page state write is the narrowest source-side failure surface consistent with the capture and LED state.

The pilot was recovered without losing its unit configuration by rerunning the guarded combined-image provisioner. Flash verified twice, the current 512-byte EEPROM image was restored byte-for-byte, and the fuses remained correct. ESP later displayed irregular recovery blinking, then the simultaneous red/blue completion indication, then red-only; HTTP journal retrieval and an exact wireless readback are still required before that recovery can count as a pass.

ESP 2.19 removed all per-page recovery-state rewrites, retained atomic persistence at phase boundaries, and recorded page checkpoints only every eight pages plus the final non-reset page. This is safe because the resident bootloader remains the progress authority and every recovered protocol-2 session starts again at page 1. ESP 2.22 retains that correction and supplies the persistent qualification hooks used by the completed Unit A sequence below.

BL0.1's page traffic and the normal ESP patterns were too ambiguous to serve as a physical completion contract. BL0.2 introduced alternating AVR red/green during bootloader work and latched solid red on error, but is now superseded for the safety reasons above. BL0.3 retains those indications and adds a read-only diagnostic command. ESP 2.22 records the bootloader identity, detected baud, diagnostic, periodic page checkpoints, recovery, reset-page commit, version verification, and completion in `/avr-update.log`. Only after it observes the exact target AVR version, persists `complete`, and removes the staged image does it blink ESP red and blue together for 60 seconds. That simultaneous pattern is the definitive physical all-done indication; the HTTP/host result and independent readback must still agree.

### Unit A ESP 2.22 / BL0.3 recovery result

The protected sketch updater replaced ESP 2.21 with the exact 563,888-byte ESP 2.22 image, verified installed MD5 `ce7ed4ed788edb346e05f8e87b36047e`, proved the uptime reset, and retained `filesystemProtected=true`. A read-only probe then returned `SW_VERSIONS,2.22,0.208`, SSID `Tx_7C2D6FD3`, MAC `86:A8:24:2F:96:5B`, `MASTER,0`, 31 C temperature, 11.7 V battery, and advancing clock reports. Staging the exact 43,520-byte AVR image left 536 KiB free in LittleFS.

Four independent Unit A runs now establish the protocol-2 boot chain:

1. An uninterrupted update completed all 85 pages and passed exact Atmel-ICE readback.
2. A persisted ESP self-restart immediately before page 8 produced journal evidence of the one-shot restart and a complete replay; exact bootloader/application, EEPROM-marker, and fuse verification passed.
3. An Atmel-ICE reset immediately after verified page 42 also removed ESP power. The journal recorded `qualification-avr-reset-armed page=42`, `qualification-avr-reset-ready page=42`, two `bootloader-session-begun` records, and `complete version=0.208`; exact final readback passed.
4. A real whole-unit power cut was made only after an independent Moto-side watcher returned `{"armedPage":42,"resetReady":true}`. WiFi disappearance independently proved the unit was off. After power returned, the journal retained the first session through page 42, began a second cold-boot session, replayed all 85 pages, verified the reset page last, and completed 0.208. The final normalized Flash SHA-256 was `8ca3c299b35ded79fc8e4a9576d5a3af89f7ab7297fa40ef88c59cd366138257`; EEPROM marker byte 511 was `0xFF`; fuses remained SHA-256 `d1edc2ade950e20b08c4fefd45ebbc8af2d6fc36fcad876c9457328c6d2925f5`, with `CODESIZE=0x00` and `BOOTSIZE=0x20`.

The ignored per-run evidence directories are timestamped `2026-07-19T00-51-38.143Z`, `2026-07-19T01-08-46.557Z`, `2026-07-19T01-33-51.608Z`, and `2026-07-19T02-11-43.047Z` under `Software/AVR128DA48/tmp/avr-update-evidence/`. The final whole-unit power-loss video is `/private/tmp/flexfox-verified-page42-power-loss-retry.mp4` on the qualification Mac and is not a repository artifact.

Failed attempts were retained and inspected. ESP 2.21 stored the AVR-reset qualification page only in RAM, so the ordinary AVR handoff erased the hook before programming. ESP 2.22 moved both one-shot hooks into the dual-slot state and clears each atomically before the injected interruption. Two operator-timing attempts later reached `qualification-reset-timeout` at `0x9400`; BL0.3 kept the reset vector uncommitted, the exact image staged, and the recovery service available. Exact restaging cleared each diagnostic without UPDI recovery. These are fail-safe stop results, not qualification passes.

Unit A therefore passes the uninterrupted, ESP-restart, AVR-reset, true-power-loss, final-version, and exact-readback gates. Unit B and the normal file/event/RF/LED gates remain mandatory before fleet expansion.

## Pre-test gates

Complete these gates separately for each unit:

1. Connect the RF output to an appropriate dummy load and confirm that no antenna is connected.
2. Join the unit's MAC-derived `Tx_...` SSID. If it advertises `Tx_Master`, retrieve and record the separate MAC-derived device SSID before authorizing an update.
3. Establish the scoped Moto/DroidTether route and run `just wifi-probe`.
4. Run `just wifi-avr-preflight`. Record the MAC-derived device SSID and AVR update state; the preflight must report that no image was staged.
5. Record SSID, ESP MAC, `MASTER` state, `SW_VERSIONS`, temperature, battery voltage, visible LED state, and whether RF is active.
6. Download or otherwise hash-record every `.event` and `.me` file. Record the active event and role independently rather than inferring them from filenames.
7. If one-time UPDI provisioning is needed, retain that unit's complete 512-byte EEPROM and fuse capture. Do not use another unit's EEPROM image.
8. Refuse firmware work while an event or manual transmission is active. A future event may be deliberately suspended for the test, but its disposition must be recorded.

The read-only probe must pass immediately before any state-changing command. A missing Moto/ADB device, stale route, timeout, wrong SSID, unexpected version, active clone, or active firmware transaction stops the sequence without staging an image.

The host staging workflow uses the same fixed-length, ESP8266-compatible multipart framing as the qualified sketch updater. A generic streaming `FormData` upload is not used: it timed out through the Moto tunnel before bootloader entry during Unit A preparation, while the incomplete staging file remained non-live.

## Candidate installation

### ESP sketch

Install ESP 2.23 first. The protected updater must replace only the sketch and preserve LittleFS:

```text
FLEXFOX_UPDATE_CONFIRM='UPDATE FLEXFOX ESP' just wifi-esp-update
```

Pass criteria:

- the updater reports the exact installed sketch MD5 derived from the frozen image;
- uptime restarts and `/firmware/status` reports ESP 2.23;
- `filesystemProtected` remains true;
- all pre-test `.event` and `.me` hashes remain unchanged;
- `just wifi-probe` still receives live AVR temperature and battery data.

### One-time boot-chain provisioning, only where absent

Use the existing guarded UPDI workflow. Set a unique fleet label for the physical unit and verify its captured EEPROM/fuses before authorizing the destructive operation:

```text
FLEXFOX_UNIT_ID=<unit-specific-label> \
FLEXFOX_PROVISION_CONFIRM=PROVISION-BOOTLOADER \
FLEXFOX_FUSE_CONFIRM=WRITE-BOOTSIZE-0x20 \
just avr-provision-boot-chain
```

Pass criteria include independent readback of the combined initial image, the unit's restored EEPROM with only reserved byte 511 erased, `BOOTSIZE=0x20`, `CODESIZE=0x00`, and all other fuses unchanged.

### Wireless AVR update

Reconnect to and verify the intended unit before every invocation. Supply the final four characters of the MAC-derived device SSID; do not use `Master` or digits copied from another unit.

```text
FLEXFOX_AVR_UPDATE_CONFIRM=UPDATE-AVR-0.208 \
FLEXFOX_EXPECTED_DEVICE_SSID=Tx_<unit-specific-eight-hex-characters> \
FLEXFOX_AVR_SSID_SUFFIX=<unit-specific-final-four> \
just wifi-avr-update
```

Use an uninterrupted update on Unit A. On Unit B, first prove one uninterrupted update; subsequent same-version updates may be used for controlled interruption testing. Pass requires all 85 page slots to complete for the 43,520-byte image, the new application to report AVR 0.208, the recovery state to become `complete`, and the staged image to be removed only after version confirmation.

The update handoff suspends RF, stores the current epoch as the completed EEPROM finish, verifies that write, and then enters BL0.3. During the transaction AVR red/green must alternate; solid AVR red indicates a stopped error. After the exact application version is observed and completion is committed, ESP red/blue must blink together for 60 seconds. The stable application indication after that should be fast red unless the operator deliberately loads and applies another runnable event.

For the Atmel-ICE pilot, enable `FLEXFOX_AVR_QUALIFICATION_FINAL_READBACK=1`. The host must save and verify the complete 128 KiB Flash capture, all 512 EEPROM bytes, all 16 fuse bytes, exact 16 KiB Boot section, exact 43,520-byte wireless image at `0x4000`, marker byte 511=`0xFF`, `CODESIZE=0x00`, and `BOOTSIZE=0x20` before it prints PASS.

## LED and schedule contract

Run this sequence on both units using a temporary short event that cannot reach its start time during setup:

1. Install and apply an event with a valid start at least ten minutes in the future. Confirm RF remains off and slow red begins.
2. Record that no additional user action is required for the event to start.
3. Use the browser `CLEAR` operation. Confirm RF remains off and the indication changes to fast red even though the future epochs remain in RAM.
4. Confirm that waiting without further action does not re-enable the event.
5. Power-cycle once as an explicit user action. Before the scheduled start, confirm the EEPROM-backed future event is enabled again and slow red returns.
6. In `events.html`, select the future event and set finish earlier than start. Confirm the picker clamps finish to exactly start, the status says **Disabled** in red, and the selected-row action remains **Apply**.
7. Press Apply. Require a successful complete Linkbus event transaction with no `EVENT_NOT_CONFIGURED` error, RF off, and fast red.
8. Power-cycle and confirm that the equal EEPROM-backed start/finish state remains unable to run and fast red returns after startup. On the Atmel-ICE pilot, independently read back the two persisted event epochs and require exact equality.
9. Repeat the browser check with both fields stale: select finish before changing start and confirm both fields advance to the same current-minute value and remain disabled.

Any slow-red indication while the event is suspended, completed, invalid, or blocked by a persistent sleep override fails AVR 0.208. Any fast-red indication while a valid applied future event will start automatically also fails it. A successful Apply that leaves both red indications dark also fails 0.208, even when the LED display period had expired before Apply.

## Wireless-update recovery

After the uninterrupted update passes, run these same-version recovery cases on the Atmel-ICE pilot. Use separate updates so every injected failure has an independent journal and final readback:

1. ESP self-restart after an early verified page, armed with `FLEXFOX_AVR_QUALIFICATION_ESP_RESTART_PAGE` and `ARM-ESP-RESTART`.
2. Atmel-ICE AVR reset after a middle verified page, armed with `FLEXFOX_AVR_QUALIFICATION_AVR_RESET_PAGE` and `ARM-AVR-RESET`; the host must observe the exact pause, execute and record the reset, release the pause, and require autonomous replay.
3. A real removal of unit power after programming is known to have advanced beyond page zero but before reset-page commit. This is the only case that cannot be simulated by software or Atmel-ICE and requires the operator to remove and restore power.
4. If the first three pass, one final uninterrupted same-version update with exact readback establishes the bench disposition.

Pass criteria:

- RF remains safe throughout;
- after restart/power and WiFi return, BL0.3 and ESP recovery state resume without UPDI programming or restaging;
- protocol 2 deliberately starts a new session and replays every non-reset page rather than trusting volatile bootloader progress;
- the complete image passes bootloader-owned page readback, product-trailer validation, payload CRC, and full-image CRC before reset-page commit;
- AVR 0.208 boots and reports through ESP 2.22;
- the staged file is removed only after the reported version matches;
- a subsequent power cycle boots normally with fast red and no RF output.

Do not count a lost browser connection as either failure or success. Use `/avr-update/status`, the final combined version report, LED observation, and RF observation. A failed recovery stops fleet provisioning but should leave the resident bootloader available for diagnosis.

## Two-unit functional run

After both units report `SW_VERSIONS,2.22,0.208`:

1. Assign different valid roles and frequencies so transmissions can be distinguished on the dummy loads or monitor receiver.
2. Clone or install the same short future Classic event on both units and preserve their distinct `.me` assignments.
3. Confirm slow red before start, start timing, the expected on/off cycle for both roles, exact finish, and fast red after finish.
4. Repeat with a short Sprint event, again using distinct roles/frequencies.
5. During an idle interval, prove that a browser heartbeat keeps WiFi available and that removing the heartbeat allows the normal two-minute WiFi shutdown.
6. Wake or power-cycle each unit and confirm its device identity, assignments, event files, temperature, battery, clock progression, and combined version report.
7. Power-cycle both after the final completed event. Neither may transmit or show slow red.

Record actual UTC start/finish times, observed RF windows, LED transitions, reconnect time, any retry, and any difference between the two units. A retry is evidence to preserve, not a reason to silently restart the test.

## Final disposition

Before removing either unit from the bench:

- delete only temporary test `.event` files;
- retain ordinary and Fleet Soak `.me` files unless a separately authorized cleanup requires otherwise;
- restore each unit's own callsign, master state, frequency, power, and role assignment;
- leave Unit A's intended event installed with a completed finish time, not its exact original future schedule;
- apply Unit B's separately recorded disposition without copying Unit A's files;
- confirm `SW_VERSIONS,2.22,0.208`, advancing time, normal temperature/battery telemetry, RF off, and fast red after a final power cycle;
- preserve the per-unit pre-state, updater output, interruption point, final file hashes, and pass/fail results with this record.

## Release boundary

Two passing units materially reduce risk but do not by themselves qualify a fleet rollout. The next gate remains the planned synchronized multi-unit Classic/Sprint rehearsal and extended fleet soak. Any bootloader recovery failure, wrong-unit update, filesystem loss, EEPROM cross-contamination, unexplained RF activity, or incorrect red-LED indication blocks expansion regardless of the other unit's result.
