# Fleet Soak Event Suite

This suite exercises ten FlexFox units through alternating Sprint and Classic events for 4 hours 41 minutes. Each event transmits for 10–15 minutes and is followed by a 10–15 minute quiet interval. The optional master page creates the twelve actual `.event` files from one operator-supplied local start date/time, avoiding committed schedules that have already expired.

## Optional host-side reference bundle

The master page is the field workflow. For an independent operator record, the checked-in host generator can create the same byte-for-byte suite for a UTC start at least 30 minutes in the future:

```sh
just fleet-soak-events 2026-07-25T13:00:00Z
```

Replace the example date and time with the intended test start. The generator writes a new directory under `Software/Huzzah/tmp/fleet-soak-events/` and refuses to overwrite an existing bundle. Its output contains:

- twelve numbered `.event` files to upload to the master;
- `fleet-soak-schedule.json`, containing the exact schedule, assignments, sizes, SHA-256 hashes, and clone-transfer checksums; and
- `README.txt`, containing a short field reminder.

The page generator is regression-compared byte-for-byte with this host generator. The manifest and README are optional operator records; the field workflow does not require selecting or uploading files manually.

## Schedule

The start offsets below are relative to the selected UTC start. There are six Sprint and six Classic events.

| Sequence | Kind | Start offset | Duration | Quiet interval after |
| ---: | --- | ---: | ---: | ---: |
| 1 | Sprint | 0 min | 10 min | 10 min |
| 2 | Classic | 20 min | 12 min | 12 min |
| 3 | Sprint | 44 min | 15 min | 15 min |
| 4 | Classic | 74 min | 10 min | 10 min |
| 5 | Sprint | 94 min | 12 min | 12 min |
| 6 | Classic | 118 min | 15 min | 15 min |
| 7 | Sprint | 148 min | 10 min | 10 min |
| 8 | Classic | 168 min | 12 min | 12 min |
| 9 | Sprint | 192 min | 15 min | 15 min |
| 10 | Classic | 222 min | 10 min | 10 min |
| 11 | Sprint | 242 min | 12 min | 12 min |
| 12 | Classic | 266 min | 15 min | — |

The last event finishes 281 minutes, or 4 hours 41 minutes, after the first start. Events do not overlap.

## Assignment plan

Role and slot indices are zero-based in the firmware. Use the same assignment for a unit in all twelve events:

| Unit | Assignment | Sprint role | Classic role | Frequency |
| ---: | --- | --- | --- | --- |
| 1 | `0:0` | Slow fox | Classic fox | 3.520 MHz |
| 2 | `0:1` | Slow fox | Classic fox | 3.520 MHz |
| 3 | `0:2` | Slow fox | Classic fox | 3.520 MHz |
| 4 | `0:3` | Slow fox | Classic fox | 3.520 MHz |
| 5 | `0:4` | Slow fox | Classic fox | 3.520 MHz |
| 6 | `1:0` | Fast fox | Alternate Classic fox | 3.560 MHz |
| 7 | `1:1` | Fast fox | Alternate Classic fox | 3.560 MHz |
| 8 | `1:2` | Fast fox | Alternate Classic fox | 3.560 MHz |
| 9 | `1:3` | Fast fox | Alternate Classic fox | 3.560 MHz |
| 10 | `1:4` | Fast fox | Alternate Classic fox | 3.560 MHz |
| Optional | `2:0` | Spectator | Spectator | 3.540 MHz |
| Optional | `3:0` | Beacon | Beacon | 3.600 MHz |

All twelve generated events use that same four-role layout. This makes each target's single assignment valid for both formats and makes over-the-air monitoring predictable. The two five-slot fox roles exercise ten primary units; spectator and beacon assignments permit two additional units.

The source templates also retain their normal power settings: Sprint roles are 300 mW and Classic roles are 5000 mW. Verify that the frequencies and power are appropriate for the test location, authorization, antennas, loads, and monitoring plan before enabling RF.

## Optional Fleet Soak page

The feature is deliberately absent from the normal LittleFS image. Install [`Software/Huzzah/optional/fleet-soak.html`](../../Software/Huzzah/optional/fleet-soak.html) on the master as `/fleet-soak.html` to enable its HTTP and WebSocket controls. Do not install the page on targets; ESP 2.26 target firmware contains the narrowly scoped protocol-2 receiver and abort support.

The page:

- accepts one local first-event start date/time, synchronizes the master clock, generates, validates, and transactionally uploads exactly `FS01-Sprint.event` through `FS12-Classic.event`;
- activates the suite only after every file passes the owned-version and four-role checks;
- identifies each attached target by its MAC-derived unique `Tx_` SSID;
- assigns newly attached foxes strictly by connection order: Slow 1 through Slow 5, then Fast 1 through Fast 5;
- transfers only the twelve reserved events and creates matching `.me` files on each target; and
- removes only the twelve reserved `.event` files during cleanup while retaining all `.me` files;
- aborts the master through an acknowledged AVR suspension before removing its reserved files; and
- authorizes abort targets by their exact rostered `Tx_` SSID, requiring each target to acknowledge AVR suspension before its reserved files are removed.

The target commits all twelve assignment `.me` files before promoting any staged `.event` file into the active reserved namespace. A failed or interrupted transfer therefore cannot activate a partially assigned suite.

Deleting `/fleet-soak.html` disarms Fleet Soak controls and deactivates the reserved suite on the master, so retained reserved files are no longer scheduled. The ordinary `events.html` clone path does not invoke Fleet Soak provisioning or cleanup.

## Master and target preparation

1. Install `/fleet-soak.html` on the master and open it.
2. Enter the local first-event start date/time, at least 30 minutes in the future, and use **Prepare master and arm fox provisioning**. The page synchronizes the master clock, generates and installs the twelve-event suite, activates it, resets the run roster, and arms provisioning.
3. Follow the page's prominent **TURN ON NOW** instruction and power one target at a time in this exact order: Slow 1, Slow 2, Slow 3, Slow 4, Slow 5, Fast 1, Fast 2, Fast 3, Fast 4, Fast 5. The page holds the current role on failure and advances only after explicitly confirming that the named fox is complete and can be turned off. The Fleet Soak transfer synchronizes each target clock before installing its reserved suite.
4. On every unit, verify that all twelve events are present and that its assignment is the same in every event. Fleet Soak provisioning creates its own reserved `.me` files; ordinary `.me` files are not changed.
5. Confirm the schedule start and finish times, the absence of overlaps, and the intended quiet intervals on representative targets.
6. Connect appropriate antennas or dummy loads and perform the normal RF safety and authorization checks.
7. During the soak, record each unit's event transitions, expected transmission slots, unexpected resets, Wi-Fi availability, clock drift, and final idle state.

After the soak, use **Remove reserved events from master**, arm target cleanup, and reconnect each tested unit. Target cleanup removes exactly the reserved twelve `.event` names and retains their `.me` files for a future soak. The page records each successful cleanup against the ten-fox provisioning roster. After all ten foxes report successful cleanup and the master has no reserved events, the page arms **Delete Fleet Soak page from master**. That button uses the existing file manager with the fixed `/fleet-soak.html` path, confirms deletion, and returns to the master home page; it cannot select any other file.

To stop early, use **Abort Fleet Soak**. This one button first requires the master AVR to acknowledge the established operator-stop command, then removes only the master's reserved soak events and arms target abort mode. Follow the page's **TURN ON FOR ABORT** instruction for every fox recorded in the run roster. An exact-SSID browser authorization is required before a target receives the abort session. Each target enters clone quiet, refuses abort if an ordinary event is known to be active, requires the AVR to acknowledge suspension, and only then removes its twelve reserved `.event` files. A failure retains that fox in the abort roster for retry. When the master and every required target have confirmed completion, the page-removal button is armed exactly as it is after normal cleanup.

If network control is unavailable, removing power remains the immediate RF stop. The software abort is complete only when the page confirms the master and every required target; do not infer completion from silence alone.

Do not start the fleet soak solely from the page's schedule summary or an optional host manifest. The master event sheet and every target's per-event assignment are the final on-device checks.

## Verification

The host regression test reconstructs a fixed-date bundle and verifies all twelve files, UTC timing, role layouts, frequencies, cycle timing, file framing, filesystem limits, hashes, and clone checksums. It also verifies that the generator rejects an accidental overwrite and a start with insufficient setup time.

The ESP event registry and clone manifest are both bounded at 25 files. `events.html` reports an explicit error if more than 25 `.event` files are present rather than silently presenting a complete-looking list. The ESP 2.26 build uses 566,256 sketch bytes and 51,664 bytes of global RAM, leaving 30,256 bytes for stack and heap; IRAM remains unchanged at 27,676 of 32,768 bytes. The self-contained optional page is 38,832 bytes and a generated twelve-event suite is about 23.7 KB raw; allowing one 8 KiB LittleFS block per file still adds less than 140 KiB to the master.

Run it as part of the normal repository checks:

```sh
just test
```

## Hardware qualification before fleet use

The host checks and firmware build do not make this fleet-ready by themselves. Use dummy loads for the first pilot and require these results before installing ESP 2.26 broadly:

1. With `/fleet-soak.html` absent and then present but disarmed, perform an ordinary `events.html` clone and verify byte-for-byte ordinary `.event`/`.me` behavior.
2. Attempt provisioning while the pilot target is running an ordinary event and verify that the target reports the active event and refuses both provisioning and cleanup.
3. Provision one target, interrupt power or Wi-Fi during an early file and again during the last file, and verify that no partial reserved suite is scheduled. Retry to completion and confirm all twelve `.event` and `.me` files.
4. Verify the target receives the assignment shown for its unique `Tx_` SSID, its first future event is programmed into the AVR, and its frequency/slot matches the page.
5. Place unrelated ordinary events and `.me` files on the target, run Fleet Soak cleanup, and prove their hashes are unchanged while only the twelve reserved `.event` files disappear.
6. Reboot master and target in disarmed, provisioned, and cleaned states; verify normal event scheduling and the retained soak assignment evidence.
7. Abort while a reserved event is active and verify RF stops before the reserved files disappear. Repeat with an ordinary event active and verify abort is refused and the ordinary event resumes after clone quiet.
8. Attach an unrostered target during abort and verify that the page sends no stop or deletion authorization. Interrupt one rostered abort and verify that it remains incomplete and retryable.
9. Complete abort for the master and every rostered target, verify the page-removal button becomes enabled, and prove that it removes only `/fleet-soak.html` while retaining `.me` files.
10. Only after those pilot checks, run the planned multi-unit RF soak and record every target result shown by the page.
