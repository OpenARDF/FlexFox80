# FlexFox80 v2.0.1 Hardware Disposition

The supported target remains FlexFox Ver 2.1 (Mar 2022). This one-commit maintenance release follows the fully qualified v2.0.0 fleet baseline and changes only manual-radio AVR handling, ESP command ordering, and `radio.html`.

The focused `radio_html_test.mjs` regression covers power acknowledgement, reconnect ordering, Send RF, and mouse/touch/pointer key-release behavior. The full AVR/ESP host characterization and firmware-contract suite also passes.

No new fleet programming or representative-device installation was performed during the v2.0.1 packaging run. The owner requested direct GitHub release of exact pushed commit `8a23b56`; the release checklist records the installed-version-report gate as an explicit scope skip. The v2.0.0 fleet evidence remains the hardware baseline, and v2.0.0 remains the supported rollback point.
