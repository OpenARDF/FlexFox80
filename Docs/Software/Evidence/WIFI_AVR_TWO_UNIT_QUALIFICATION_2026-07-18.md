# WiFi AVR Two-Unit Qualification — 2026-07-18

**Status:** ESP 2.16 / AVR 0.207 candidate built; hardware results pending

## Objective

Qualify the ESP 2.16 / AVR 0.207 / BL0.1 combination on two identified, dummy-loaded FlexFox80 units before expanding the boot chain to the fleet. The test must prove wireless updating, normal transmitter behavior, and persistent event disabling through Apply. It must also prove the operator LED contract:

- slow red means an event will run without another user action;
- fast red means no event can run until the user acts.

Retained future start and finish epochs are not enough to justify slow red after an event has been suspended.

## Frozen candidate

| Item | Value |
| --- | --- |
| Source commit | Commit containing this qualification record |
| ESP version | `2.16` |
| AVR version | `0.207` |
| Bootloader version | `BL0.1` |
| ESP sketch | 557,984 bytes; SHA-256 `6eeb18fede38d705145ab66234a3f8ba4da483ffb5c91bd8153658e7ecd5736a` |
| AVR wireless image | 43,008 bytes; CRC32 `0x34836fc8`; SHA-256 `c80bc7c7893f985bb10ff90a812de977f7f2fd692f37402a878ddae105d36829` |
| AVR first-install HEX | SHA-256 `5b2a542afcbef721a13a84a5b5e9facfd0b4c83236bc869ac0e16a281960262e` |
| AVR release manifest | SHA-256 `883183dcca374817d8a057c3c9845f3e0c43a83a232fa21ec6a9e1cd8a652abb` |
| Boot layout | 2,512-byte bootloader in a 16,384-byte reservation; application at `0x4000` |
| ESP resources | 53% sketch flash; 60% global RAM; 32,548 bytes dynamic-memory headroom; zero warnings |
| AVR resources | 41,886 text + 1,150 data bytes; 1,576 BSS bytes; zero warnings |

The working-tree candidate passed `just check` and `just secrets`; two consecutive exact AVR and ESP builds completed with zero warnings and identical sketch, wireless-image, first-install, and manifest hashes.

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
| Dummy load confirmed | Pending operator observation | Pending operator observation |
| Event files and `.me` assignments captured | Pending | Pending |
| AVR EEPROM and fuses captured if provisioning | Existing record; verify before reuse | Pending if provisioning |
| Boot chain already present | Yes | Pending probe |

Unit A has a special final-state requirement. Do not reinstall its exact original future event. Keep its intended event file installed with a completed finish time so a later power cycle cannot restart the test event. Unit B's captured event files, assignments, master state, frequency, power, and callsign remain unit-specific; never restore them from Unit A's backup.

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

Install ESP 2.16 first. The protected updater must replace only the sketch and preserve LittleFS:

```text
FLEXFOX_UPDATE_CONFIRM='UPDATE FLEXFOX ESP' just wifi-esp-update
```

Pass criteria:

- the updater reports the exact installed sketch MD5 derived from the frozen image;
- uptime restarts and `/firmware/status` reports ESP 2.16;
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
FLEXFOX_AVR_UPDATE_CONFIRM=UPDATE-AVR-0.207 \
FLEXFOX_EXPECTED_DEVICE_SSID=Tx_<unit-specific-eight-hex-characters> \
FLEXFOX_AVR_SSID_SUFFIX=<unit-specific-final-four> \
just wifi-avr-update
```

Use an uninterrupted update on Unit A. On Unit B, first prove one uninterrupted update; a subsequent same-version update may be used for controlled interruption testing. Pass requires all 84 page slots to complete for the 43,008-byte image, the new application to report AVR 0.207, the recovery state to become `complete`, and the staged image to be removed only after version confirmation.

The update handoff suspends RF, stores the current epoch as the completed EEPROM finish, verifies that write, and then enters BL0.1. Consequently, the first stable indication after AVR 0.207 starts should be fast red unless the operator deliberately loads and applies another runnable event.

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

Any slow-red indication while the event is suspended, completed, invalid, or blocked by a persistent sleep override fails AVR 0.207. Any fast-red indication while a valid applied future event will start automatically also fails it.

## Wireless-update recovery

After both uninterrupted updates pass, repeat AVR 0.207 on one accessible pilot and interrupt power after programming has advanced beyond page zero but before completion. Keep the chassis accessible and the exact staged image available.

Pass criteria:

- RF remains safe throughout;
- after power and WiFi return, BL0.1 and ESP recovery state resume without UPDI intervention or restaging;
- progress continues from a safe recoverable state;
- the complete image passes AVR-side page readback and final CRC;
- AVR 0.207 boots and reports through ESP 2.16;
- the staged file is removed only after the reported version matches;
- a subsequent power cycle boots normally with fast red and no RF output.

Do not count a lost browser connection as either failure or success. Use `/avr-update/status`, the final combined version report, LED observation, and RF observation. A failed recovery stops fleet provisioning but should leave the resident bootloader available for diagnosis.

## Two-unit functional run

After both units report `SW_VERSIONS,2.16,0.207`:

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
- confirm `SW_VERSIONS,2.16,0.207`, advancing time, normal temperature/battery telemetry, RF off, and fast red after a final power cycle;
- preserve the per-unit pre-state, updater output, interruption point, final file hashes, and pass/fail results with this record.

## Release boundary

Two passing units materially reduce risk but do not by themselves qualify a fleet rollout. The next gate remains the planned synchronized multi-unit Classic/Sprint rehearsal and extended fleet soak. Any bootloader recovery failure, wrong-unit update, filesystem loss, EEPROM cross-contamination, unexplained RF activity, or incorrect red-LED indication blocks expansion regardless of the other unit's result.
