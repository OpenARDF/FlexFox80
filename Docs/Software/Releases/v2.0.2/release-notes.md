# FlexFox80 v2.0.2

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Firmware:** ESP `2.28`, AVR `0.210` (`SW_VERSIONS,2.28,0.210`)

**Resident bootloader:** `BL0.3`

**Source:** `1120ee63804bf6b69dcea89c1c17772236081556`

## Changes since v2.0.1

- Reacquire the bounded five-minute AVR maintenance lease before the wireless update handoff, preventing a missed first handoff from powering down WiFi before the updater can return status or retry.
- Release that maintenance lease on every recoverable updater return so ordinary two-minute WiFi shutdown behavior resumes.
- Record the external-voltage post-standby freshness issue as near-term telemetry work without changing the qualified AVR, RF, event, or sleep behavior in this release.

## Included v2.0.1 corrections

- Apply the selected manual-radio power before RF key-down, even when it matches the cached AVR value.
- Make manual text transmission claim the Morse generator, retain newly queued text, and enable RF.
- Wait for AVR power acknowledgement before enabling the Radio page Key and Send controls.
- Reliably release touch, pointer, mouse, blur, and page-exit keying.
- Prevent stale sockets and delayed startup commands from overriding the active connection.

## Verification

- The full repository suite and complete-history secret scan pass.
- Two pinned AVR-GCC 7.3.0 / AVR-Dx_DFP 1.9.103 build passes are warning-free and byte-identical.
- Two pinned ESP8266 core 2.7.4 / WebSockets 2.3.6 build passes are warning-free and byte-identical.
- The package passed exact checksums, manifest checks, AVR parsing, BL0.3/application/trailer extraction, ESP checksum validation, and LittleFS inventory.
- All 17 FlexFoxes ran the exact ESP `2.28`, AVR `0.210`, BL0.3, and `radio.html` 0.86 candidate during the pre-release rollout; the owner reported successful testing.

The planned final reflash of all 17 units will use the independently verified published v2.0.2 assets. Routine fleet work is wireless for both ESP and AVR; Atmel-ICE is reserved for bootloader provisioning or recovery.
