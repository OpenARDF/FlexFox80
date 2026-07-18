# Fleet Soak Event Suite

This suite exercises at least ten FlexFox units through alternating Sprint and Classic events for 4 hours 41 minutes. Each event transmits for 10–15 minutes and is followed by a 10–15 minute quiet interval. The checked-in generator creates the twelve actual `.event` files after the rollout start time is known, avoiding committed schedules that have already expired.

## Generate the event bundle

Choose a UTC start at least 30 minutes in the future:

```sh
just fleet-soak-events 2026-07-25T13:00:00Z
```

Replace the example date and time with the intended test start. The generator writes a new directory under `Software/Huzzah/tmp/fleet-soak-events/` and refuses to overwrite an existing bundle. Its output contains:

- twelve numbered `.event` files to upload to the master;
- `fleet-soak-schedule.json`, containing the exact schedule, assignments, sizes, SHA-256 hashes, and clone-transfer checksums; and
- `README.txt`, containing a short field reminder.

Only the twelve `.event` files belong on the master. The manifest and README are operator records.

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

The feature is deliberately absent from the normal LittleFS image. Install [`Software/Huzzah/optional/fleet-soak.html`](../../Software/Huzzah/optional/fleet-soak.html) on the master as `/fleet-soak.html` to enable its HTTP and WebSocket controls. Do not install the page on targets; target firmware already contains the narrowly scoped receiver support.

The page:

- validates and transactionally uploads exactly `FS01-Sprint.event` through `FS12-Classic.event`;
- activates the suite only after every file passes the owned-version and four-role checks;
- identifies each attached target by its MAC-derived unique `Tx_` SSID;
- recommends a persistent, unused assignment based first on retained soak `.me` files, then on existing Sprint/Classic assignment evidence when possible;
- transfers only the twelve reserved events and creates matching `.me` files on each target; and
- removes only the twelve reserved `.event` files during cleanup while retaining all `.me` files.

Deleting `/fleet-soak.html` disarms Fleet Soak controls and deactivates the reserved suite on the master, so retained reserved files are no longer scheduled. The ordinary `events.html` clone path does not invoke Fleet Soak provisioning or cleanup.

## Master and target preparation

1. Generate the bundle for the real UTC start and retain `fleet-soak-schedule.json` with the test record.
2. Install `/fleet-soak.html` on the master, open it, select all twelve generated files, and use **Install and activate selected suite**.
3. Arm target provisioning, then connect one target at a time. Confirm or adjust the recommended unique assignment. The Fleet Soak transfer synchronizes the target clock before installing the reserved suite.
4. On every unit, verify that all twelve events are present and that its assignment is the same in every event. Fleet Soak provisioning creates its own reserved `.me` files; ordinary `.me` files are not changed.
5. Confirm the schedule start and finish times, the absence of overlaps, and the intended quiet intervals on representative targets.
6. Connect appropriate antennas or dummy loads and perform the normal RF safety and authorization checks.
7. During the soak, record each unit's event transitions, expected transmission slots, unexpected resets, Wi-Fi availability, clock drift, and final idle state.

After the soak, use **Remove reserved events from master**, arm target cleanup, and reconnect each tested unit. Target cleanup removes exactly the reserved twelve `.event` names and retains their `.me` files for a future soak. Deleting `/fleet-soak.html` from the master then removes the optional control surface.

Do not start the fleet soak solely from the generated manifest. The master event sheet and every target's per-event assignment are the final on-device checks.

## Verification

The host regression test reconstructs a fixed-date bundle and verifies all twelve files, UTC timing, role layouts, frequencies, cycle timing, file framing, filesystem limits, hashes, and clone checksums. It also verifies that the generator rejects an accidental overwrite and a start with insufficient setup time.

The ESP event registry and clone manifest are both bounded at 25 files. `events.html` reports an explicit error if more than 25 `.event` files are present rather than silently presenting a complete-looking list. The qualified ESP 2.13 build uses 53% of sketch flash and 60% of global RAM, leaving 32,548 bytes for stack and heap. The optional page is 11,549 bytes and a generated twelve-event suite is about 23.7 KB raw; allowing one 8 KiB LittleFS block per file still adds only about 120 KiB to the master.

Run it as part of the normal repository checks:

```sh
just test
```

## Hardware qualification before fleet use

The host checks and firmware build do not make this fleet-ready by themselves. Use dummy loads for the first pilot and require these results before installing ESP 2.13 broadly:

1. With `/fleet-soak.html` absent and then present but disarmed, perform an ordinary `events.html` clone and verify byte-for-byte ordinary `.event`/`.me` behavior.
2. Attempt provisioning while the pilot target is running an ordinary event and verify that the target reports the active event and refuses both provisioning and cleanup.
3. Provision one target, interrupt power or Wi-Fi during an early file and again during the last file, and verify that no partial reserved suite is scheduled. Retry to completion and confirm all twelve `.event` and `.me` files.
4. Verify the target receives the assignment shown for its unique `Tx_` SSID, its first future event is programmed into the AVR, and its frequency/slot matches the page.
5. Place unrelated ordinary events and `.me` files on the target, run Fleet Soak cleanup, and prove their hashes are unchanged while only the twelve reserved `.event` files disappear.
6. Reboot master and target in disarmed, provisioned, and cleaned states; verify normal event scheduling and the retained soak assignment evidence.
7. Only after those pilot checks, run the planned multi-unit RF soak and record every target result shown by the page.
