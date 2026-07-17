# FlexFox80 v1.0.0 GitHub Release Withdrawal — 2026-07-17

## Reason

FlexFox80 v1.0.0 contains AVR `0.201`, which is affected by the critical standby RTC-counter regression documented in [AVR sleep/wake RTC counter regression](AVR_SLEEP_WAKE_RTC_COUNTER_REGRESSION_2026-07-17.md). It is unsafe for scheduled operation because a sleeping unit can miss a future event start and advance event time far faster than real time.

## GitHub action

The published GitHub release `v1.0.0` was deleted from `OpenARDF/FlexFox80` on 2026-07-17. Deletion removed the release page and these six downloadable assets:

- `FlexFox80-AVR-0.201.hex`;
- `FlexFox80-Checksums-v1.0.0.txt`;
- `FlexFox80-ESP-2.1.bin`;
- `FlexFox80-LittleFS-2.1.bin`;
- `FlexFox80-Release-Info-v1.0.0.json`; and
- `FlexFox80-v1.0.0-AVR-0.201-ESP-2.1-Release-Files.zip`.

Post-deletion verification returned `release not found`, and the repository's GitHub release list was empty.

## Retained audit marker

The annotated Git tag `v1.0.0` was deliberately retained as historical evidence rather than rewriting Git history:

```text
refs/tags/v1.0.0 tag 1ce3b05909014ae8df50c0d246d042550ef47fb3
```

The tag and historical release record must not be treated as an approved firmware source. AVR `0.201` remains withdrawn from scheduled use. A replacement release must use a separately identified, fully qualified AVR version and new release tag.
