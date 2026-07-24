# FlexFox80 v2.0.1

**Hardware:** FlexFox Ver 2.1 (Mar 2022) only

**Firmware:** ESP `2.27`, AVR `0.210` (`SW_VERSIONS,2.27,0.210`)

**Source:** `8a23b565821bbd27dca414b8b93809bd9ccc044c`

## Changes

- Apply the selected manual-radio power before RF key-down, even when it matches the cached AVR value.
- Make manual text transmission claim the Morse generator, retain newly queued text, and enable RF.
- Wait for AVR power acknowledgement before enabling the Radio page Key and Send controls.
- Reliably release touch, pointer, mouse, blur, and page-exit keying.
- Prevent stale sockets and delayed startup commands from overriding the active connection.

## Verification

- The full repository suite and complete-history secret scan pass.
- Two pinned AVR-GCC 7.3.0 / AVR-Dx_DFP 1.9.103 builds are warning-free and byte-identical.
- Two pinned ESP8266 core 2.7.4 / WebSockets 2.3.6 builds are warning-free and byte-identical.
- The package and fresh GitHub download passed exact checksums, manifest checks, AVR parsing, BL0.3/application extraction, ESP checksum validation, and LittleFS inventory.

The embedded strings remain ESP `2.27` and AVR `0.210`; use the v2.0.1 manifest and SHA-256 checksums to distinguish these artifacts from v2.0.0.
