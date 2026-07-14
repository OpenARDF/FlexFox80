# FlexFox80 Release Hardware Checklist

**Checklist date:** 2026-07-14

**Candidate identity:** [Release-candidate snapshot](RELEASE_CANDIDATE_SNAPSHOT_2026-07-14.md)

**Supported hardware target:** FlexFox Ver 2.1 (Mar 2022) only

**Status:** Focused gates pass; remaining broad A8 tests or explicit owner-approved skips are required before official release

## Status meanings

- **Pass:** the frozen source or exact candidate has applicable direct evidence.
- **Partial:** a focused constituent passed, but the complete A8 behavior has not been exercised.
- **Pending:** no sufficient current-candidate evidence has been recorded.
- **Proposed defer:** release-time deferral is reasonable given prior field reliability, but requires explicit owner approval.

## Test setup represented by existing evidence

- AVR128DA48 dummy-loaded FlexFox test unit, silicon revision 1.7.
- Atmel-ICE `J41800053674`, UPDI at 100 kHz, target voltage approximately 3.25–3.31 V.
- Production R4 candidate HUZZAH chip MAC `44:17:93:0f:09:3e`.
- Secondary/source HUZZAH chip MAC `a4:e5:7c:2d:69:ed`.
- Routed ESP HTTP/WebSocket access through Moto USB tethering and DroidTether, with ordinary Mac networking kept separate.
- RF-sensitive operations performed into a dummy load or through the existing zero-power path; evidence states when antenna detect prevented an ordinary start.

## Automated, identity, and startup gates

| Gate | Status | Evidence / remaining action |
| --- | --- | --- |
| Full repository suite | Pass | `just check` passed policy, links, sanitizer-enabled host tests, firmware contracts, and EEPROM layout. |
| AVR deterministic Release build | Pass | Version `0.201`; two AVR-GCC 7.3.0 / DFP 1.9.103 builds, zero warnings, identical artifacts; HEX `89707660…1ba4`. |
| ESP deterministic sketch build | Pass | Version `2.1`; two core-2.7.4/WebSockets-2.3.6 builds, zero warnings, sketch `0749b0e4…bad8`. |
| Combined installed version report | Pending | Program both selected artifacts on a representative paired unit and require `SW_VERSIONS,2.1,0.201`; source/reporting contract already passes. |
| AVR EEPROM ABI | Pass | Linker `.eeprom = 0x112`; fixed-width host and compile-time contracts pass. |
| ESP standalone startup | Pass | Qualified core/library profile repeatedly restored normal LEDs and SSID; 3.x profiles were rejected. |
| Installed HTTP/WebSocket/AVR telemetry | Pass | Both restored R4 test units returned HTTP 200, WebSocket, temperature, battery, versions, role, and advancing epochs. |
| Ordinary reset/power-up | Pass | Repeated AVR/ESP resets and post-program boots retained communications and expected SSIDs. |
| Repeated extended power-cycle campaign | Proposed defer | Existing reset/startup evidence is strong; a longer cold-cycle sample has not been run on the frozen pair. |

## Clone, event-file, and configuration gates

| Gate | Status | Evidence / remaining action |
| --- | --- | --- |
| Normal master-to-target clone | Pass | Successful retry completed with established LED pattern; all nine events and baseline state read back. |
| Clone RTC gate and report cleanup | Pass | Exact epoch readback is required before transfer; successful clones and explicit `$ESP,R;` cleanup restore reports. |
| Mismatched clone checksum | Pass | Controlled `checksum + 1` master was rejected; target retained `NZ0I<<` rather than `R4BAD`. |
| Prior-event retention after rejected clone | Pass | All nine files, roles, assignment, frequencies, and powers remained intact after rejection. |
| Legacy stored-event compatibility | Pass | Host contract accepts ordinary checksum-less files; installed existing LittleFS events loaded normally. |
| Role 0 / role 1 assignment persistence | Pass | Both assignments saved, reloaded, returned correct frequency/power, and original role was restored. |
| Clone retry tail | Proposed defer | `B-CLONE-02` remains: operator estimates retry in 5–10% of clones; one R4 normal attempt stalled and a clean retry passed. |

## Time, RTC, schedule, and sleep gates

| Gate | Status | Evidence / remaining action |
| --- | --- | --- |
| Clone edge alignment and exact RTC readback | Pass | Master next-edge report, target prioritized write, exact epoch gate, and cleanup are qualified. |
| Multi-second RTC edge delay recovery | Pass | Approximately 3.004-second ISR blockage recovered counted edges without a whole-second loss. |
| Missing RTC edge / intervening edge | Pass | Isolated injections returned error 252 without foreground lockup; production image and state were restored. |
| Event boundary predicates | Pass | Directed `start-1` through `finish+1` host cases and exact target installation/telemetry pass. |
| Real scheduled start and finish | Pending | Predicate logic is qualified; an actual scheduled RF event on the frozen candidate has not been recorded. |
| Scheduled sleep and wake | Pending | Atomic wake-time publication passes; the evidence explicitly leaves a complete scheduled sleep/wake cycle to A8. |
| Long-duration RTC drift/aging | Proposed defer | Deliberately postponed; synchronization consistency, not high absolute-time accuracy, was the release priority. |

## RF and event-mode gates

| Gate | Status | Evidence / remaining action |
| --- | --- | --- |
| Immediate start/stop control path | Partial | Zero-power start/stop and acknowledgments pass; antenna detect rejected the ordinary energized start as designed. |
| Classic event RF cycle | Pending | Host schedule logic and clone of Classic files pass; complete on-air slot/ID timing has not been recorded. |
| Sprint event RF cycle | Pending | Mature field behavior exists, but no frozen-candidate A8 trace is recorded. |
| Foxoring event RF cycle | Pending | Mature field behavior exists, but no frozen-candidate A8 trace is recorded. |
| Beacon operation | Pending | No frozen-candidate A8 trace is recorded. |
| Antenna removal/reconnection | Pending | The open antenna-detect path safely rejected start; removal/reconnection recovery itself is untested. |
| Si5351/I2C safe-failure behavior | Partial | RTC/I2C failures and delayed-edge recovery are bounded; complete RF-safe Si5351 failure/recovery remains open. |
| Long-duration event operation | Proposed defer | No current-candidate endurance run; candidate changes are bounded and prior product field reliability is strong. |

## Environment and resilience gates

| Gate | Status | Evidence / remaining action |
| --- | --- | --- |
| Temperature and battery telemetry | Pass | Repeated live readings passed on both restored units. |
| Temperature/fan control behavior | Pending | Telemetry does not establish fan thresholds or extended thermal behavior. |
| ESP reset/recovery | Partial | Standalone and installed reset/SSID recovery pass; interruption during every clone phase is not characterized. |
| Watchdog/reset-cause recovery | Proposed defer | R3 remains planned; no new watchdog policy or reset-cause reporting is included in this candidate. |
| Event/EEPROM preservation during focused faults | Pass | Target programming and fault slices repeatedly preserved/restored full EEPROM and fuses; R4 retained prior LittleFS files. |
| General power-loss transaction testing | Proposed defer | Broader A7 transactional persistence work is explicitly deferred. |

## Rollback and restoration

| Gate | Status | Evidence / remaining action |
| --- | --- | --- |
| Known-good AVR rollback material | Partial | Multiple ignored flash/EEPROM/fuse captures and recovery procedures exist; select the official package. |
| Known-good ESP rollback material | Partial | Verified 4 MiB HUZZAH captures exist; select the official package and record its retained location. |
| Candidate reflash procedure | Pass | Atmel-ICE AVR and FTDI/esptool HUZZAH procedures are documented and repeatedly exercised. |
| Final rollback verification | Pending | A8 requires explicit verification of the selected release rollback, not only existence of prior archives. |
| Test-unit restoration | Pass | Source restored to `MASTER,0` / `Tx_7C2D69ED` / `0:4`; candidate restored to `MASTER,1` / `Tx_Master` / `1:0`, with complete readbacks. |

## Owner decisions required

Before official release, record:

1. which pending RF/event/sleep/thermal tests will be run;
2. which proposed deferrals are approved for this release;
3. the selected rollback archive and its verification result;
4. the release label decision (the embedded identities are AVR `0.201` and ESP `2.1`);
5. approval to fast-forward `AVR128DA48` only after the final clean-checkout gates pass.

Until those decisions are recorded, the candidate is frozen and reproducible but not officially released.
