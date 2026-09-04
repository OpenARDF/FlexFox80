# FlexFox80 v2.0.3 Hardware Disposition

The supported target remains FlexFox Ver 2.1 (Mar 2022). This release changes ESP-owned clone synchronization and retains AVR `0.210`, BL0.3, and `radio.html` 0.86.

The evidence-bound ESP 2.35 sketch was installed on both masters and all 15 target units while preserving each unit's AVR application and LittleFS. All 14 hardware-capable targets completed master-observed clone synchronization with clock readback, complete file cycles, role assignment, cleanup, release, and `SLAVE,0`. There were no clone buffer overflows, recovered resets, master watchdog events, or master failures.

November has a preexisting, operator-confirmed Linkbus hardware fault. Its exact sketch and filesystem state passed the fleet update checks, but the failed hardware path made clone qualification impossible. This is an explicit hardware exclusion, not an untested software-capable target.

The release pair is `SW_VERSIONS,2.35,0.210`. The public ESP image hash is `3e3817c2b60e2f8b94d4cb40ff2fcb6360cf5db80404e295af1114d634749acb`; the retained AVR wireless image hash is `90e72a7f8a1a7620dee5465309640f434647f00ebb1d64027c3abc75fef3b74e`; and the embedded `radio.html` 0.86 source hash is `9edadd4b4506f903fa8280d68e0cd8a5318d1d2d3e0375dde10fbbdda382a26a`.
