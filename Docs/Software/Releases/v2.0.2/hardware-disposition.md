# FlexFox80 v2.0.2 Hardware Disposition

The supported target remains FlexFox Ver 2.1 (Mar 2022). This maintenance release changes the ESP-owned wireless AVR handoff timeout behavior and retains AVR `0.210`, BL0.3, and the qualified manual-radio implementation in `radio.html` 0.86.

Before release packaging, the exact ESP `2.28` sketch hash `b851c87004302c908783f6746c91de91a82a6a0dadb8f53546de8ea27feef226`, AVR `0.210` wireless image hash `90e72a7f8a1a7620dee5465309640f434647f00ebb1d64027c3abc75fef3b74e`, and `radio.html` 0.86 hash `9edadd4b4506f903fa8280d68e0cd8a5318d1d2d3e0375dde10fbbdda382a26a` were installed across the complete 17-unit fleet. The two physical `Tx_Master` units were tracked separately by their MAC-derived identities, so the shared visible name did not reduce the device count. The owner reported successful manual-radio power/keying tests and normal fleet operation.

The release pair remains `SW_VERSIONS,2.28,0.210`. The planned post-publication reflash is a final consistency pass from the durable release archive, not a missing candidate qualification gate. It will force the wireless AVR application update even where the version already reports `0.210`, update ESP wirelessly, preserve LittleFS during routine sketch installation, and independently verify each unit before it is counted complete.

The post-standby external-voltage freshness issue is deferred as telemetry-only near-term work. It does not change RF power selection or event transmission in v2.0.2.
