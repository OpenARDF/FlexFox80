# AVR startup temperature validity — 2026-07-19

**Status:** ESP 2.25 / AVR 0.210 source, host, exact-build, wireless-install, readback, and three-reset pilot gates pass; cold-power, fan, and event qualification remain

## Reproduction and root cause

Fleet provisioning reproduced `TEMP,230.0C` immediately after reset, followed by normal readings. The maintained AVR source explains that exact transient without requiring a failed temperature sensor:

1. TCB0 already owned a periodic ADC state machine for battery and rail measurements.
2. The foreground `temperatureC()` helper independently changed the ADC MUX, reinitialized the converter, and started a conversion without excluding the TCB0 owner.
3. It did not clear `RESRDY` before waiting. A result-ready flag and `ADC0.RES` value from the previously selected external channel could therefore satisfy the temperature wait immediately.
4. The helper then applied the AVR DA factory temperature calibration equation to that other channel's sample, producing the impossible startup value.
5. It restored only `MUXPOS`, not the periodic owner's complete conversion state.
6. Its timeout counter was `static` and never reset. Repeated calls permanently depleted it, after which the helper returned a numeric `-273` sentinel forever. Telemetry formatted every returned number and the ESP appended `C`, so unavailable state had no representation.

This is an ADC ownership and completion-association defect, not evidence that the factory calibration equation itself is wrong. The maintained equation agrees with Microchip's AVR DA temperature-measurement guidance.

## Correction

- TCB0 is now the only owner of ordinary single ADC conversions. Temperature is a fourth scheduled channel, requested after 10 task ticks at boot and once every 300 ticks thereafter.
- Every conversion clears a prior result-ready value before start. A conversion that does not complete within three TCB0 ticks (10 ms at the documented 300 Hz task rate) is aborted so the ADC scanner cannot remain wedged.
- The timer ISR only records a completed conversion explicitly associated with `ADCTemperature`; calibration and validity work runs in foreground code so the 300 Hz timing path remains lean.
- A temperature is available to callers only after the cached sample passes the shared inclusive `-20C` through `120C` contract. Calibration arithmetic uses a 64-bit intermediate, preventing corrupt calibration words from wrapping into an apparently plausible result.
- The cached numeric value is private. The public getter returns a Boolean availability result and does not modify the caller's output when unavailable; there is no numeric sentinel.
- Invalid samples and timeouts clear availability. Fan control honors the same getter and turns the fan on when temperature is unavailable; the established 30/35 C hysteresis is unchanged for valid samples.
- AVR Linkbus replies use `!TEM,NA` while unavailable. ESP 2.25 strictly accepts only decimal payloads in range, normalizes valid replies, and sends `TEMP,NA` for unavailable, malformed, or out-of-range AVR data. This also protects browsers from older AVR firmware's impossible values.
- `events.html`, `radio.html`, and `test.html` independently reject malformed or out-of-range values, display `Temp: unavailable`, and clear stale temperature text whenever a new WebSocket connection opens.
- The read-only WiFi probe and fleet-upgrade workflow accept only a numeric `-20C` through `120C` response as live-temperature PASS evidence.

## Automated evidence

The dependency-light host suite covers both physical boundaries, strict decimal parsing, `NA`, malformed strings, NaN/infinity/hex/exponent rejection, the AVR DA calibration equation, out-of-range samples, a 12-bit ADC overflow, null output, and corrupt calibration values that could otherwise wrap. Browser tests exercise valid display, unavailable display, out-of-range rejection, and stale-value clearing on connection. Source contracts bind the shared limits to the ADC scheduler, timeout, availability response, and ESP filtering paths.

`just test`, the exact AVR-GCC 7.3.0 / AVR-Dx_DFP 1.9.103 build, and the pinned ESP8266 2.7.4 / WebSockets 2.3.6 build complete with zero warnings. The final artifacts are:

- ESP 2.25 sketch: 564,712 compiled bytes, 568,864-byte `.bin`, SHA-256 `d838e74235a2076ae1d3d5d713437033cd148e4ee3f6e52c8a254b8934d82675`, 30,688 bytes dynamic-memory headroom, and unchanged 27,676/32,768-byte IRAM use;
- relocated AVR 0.210 application payload: 43,520 bytes, CRC32 `0x8e030005`;
- protocol-2 wireless AVR image: 44,032 bytes (86 pages), CRC32 `0xdc09ede4`, SHA-256 `c8f51a83daf33808d3ff9b011b58f20a44eeaaf4d08a7adb72848467fb473eed`; and
- unchanged BL0.3 at 38,400 baud: 5,112 bytes, SHA-256 `6d0997d0732566d66060970d3b7026654d7336aaa00483ae903c49c9e2fb78bc`.

## Pilot evidence

The authorized `Tx_C22DD117` pilot was upgraded over WiFi to ESP 2.25 and AVR 0.210. The ESP sketch updater verified the installed sketch MD5, LittleFS protection, and normal mounted state. The AVR updater reported `complete (86/86)` and the installed application reported version 0.210. An independent Atmel-ICE readback matched the exact padded BL0.3 Boot section and the complete 44,032-byte wireless image.

Three subsequent Atmel-ICE reset/start cycles reported 26.0 C, 27.0 C, and 28.0 C respectively through the raw WebSocket probe, with no impossible numeric value. Every probe also returned the exact SSID and ESP 2.25 / AVR 0.210 versions. The updated `events.html`, `radio.html`, and `test.html` files were atomically installed and hash-verified on the pilot.

The update initially appeared to remain silent after completion because the Moto deliberately did not autojoin the returning open AP. Explicit association immediately exposed the completed result. The AVR updater now accepts `FLEXFOX_ADB_SERIAL` and repeats exact-SSID association during its recovery loop, retaining the older qualification variable as a compatibility alias.

## Remaining pilot gate

Before fleet rollout, complete the environmental and normal-operation checks that cannot be established by reset telemetry alone:

1. repeated whole-unit cold power starts while observing browser and raw WebSocket temperature replies;
2. longer alternating battery and temperature requests to supplement the source/host proof that the old one-way static timeout is gone;
3. 30/35 C fan hysteresis if safe temperature stimulus is available, with fan-on fail-safe retained as source/host proof unless an unobtrusive fault injection is justified; and
4. normal event, RF, WiFi timeout, and sleep/wake smoke checks after the temperature change.
