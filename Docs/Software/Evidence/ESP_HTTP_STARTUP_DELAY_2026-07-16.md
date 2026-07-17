# ESP HTTP Startup Delay After Wi-Fi Association

**Date:** 2026-07-16

**Path:** B-HTTP-03

**Status:** Live symptom confirmed; prior exact timeout attribution corrected; phase instrumentation and remediation remain planned

## Observed symptom

A phone or computer can associate successfully with a FlexFox access point while `http://73.73.73.73/` remains unreachable. The page can then become available without the user changing browsers or reconnecting. During the ESP 2.5 fleet work, the first successful `/firmware/status` response after one cold startup reported `uptimeMillis: 178332`, showing that the application endpoint did not answer until almost three minutes after the ESP booted.

This is not inherently a Chrome failure. The same separation between Wi-Fi association and HTTP reachability was observed through non-browser host probes.

## Confirmed startup ordering

The Wi-Fi access point and the HTTP application are separate readiness layers.

The qualified ESP8266 core initializes Wi-Fi persistence as enabled. `softAP()` therefore stores the access-point configuration in the SDK's persistent configuration unless the application explicitly disables persistence. The FlexFox sketch does not disable it. This permits the ESP8266 SDK to restore and advertise the previously configured FlexFox access point early in a later boot, before the sketch reaches its HTTP setup. A client can consequently associate and receive network configuration even though no FlexFox TCP listener is ready on port 80.

The sketch starts the HTTP server late in its top-level startup sequence. Before calling `g_http_server.begin()`, it:

1. waits for baud calibration and a time report from the AVR;
2. on a target, searches for `Tx_Master` so automatic cloning retains priority;
3. scans and selects the next scheduled event;
4. loads that event and attempts to program it into the AVR; and
5. only then configures the local soft AP handlers and starts HTTP.

The relevant production paths are `loop()`, `setupWiFiAPConnection()`, `loadActiveEventFile()`, `sendEventToATMEGA()`, `linkbusLoop()`, and `setupHTTP_AP()` in `Software/Huzzah/ARDF_Transmitter/ARDF_Transmitter.ino`.

## Correction to the original timeout attribution

The live 178.332-second observation is valid, but the original source-derived explanation was not. `sendEventToATMEGA()` prequeues all 17 event commands, then waits only about ten seconds for the shared queue to drain. It does not remain inside the function for three three-second acknowledgement windows on every queued command.

If the first command is unanswered, the function can return failure after approximately ten seconds while most of the transaction remains queued. The abandoned backlog can still take roughly 17 times nine seconds to drain if every command is unanswered, and clone keep-alives can join the same queue, but that draining is not by itself proof that `g_http_server.begin()` was delayed for the whole interval.

The almost exact numerical match between 178.332 seconds and the prior arithmetic was therefore coincidental. Do not use it as root-cause evidence. See [ESP-to-AVR Linkbus reliability review](ESP_AVR_LINK_RELIABILITY_REVIEW_2026-07-17.md) for the corrected transaction model.

## Remaining root-cause question

The investigation explains why Wi-Fi can look connected before HTTP is ready. It does not yet identify which startup phase consumed the observed three minutes or prove why AVR acknowledgments were missing on each affected startup.

The next investigation must distinguish at least:

- an ESP-only restart while the AVR remains in a state that cannot answer normal Linkbus commands;
- an asleep, powered-down, or restarting AVR;
- Linkbus having been disabled for ESP serial programming, which the current AVR treats as a power-cycle-only state;
- startup baud-calibration or receive-state loss after the initial time exchange;
- a transport or tether failure that happens to overlap application startup.

Required phase timestamps are ESP reset, first Linkbus wake request, first valid AVR frame, master-search entry/exit, event-transfer entry/exit, `WiFi.softAP()` entry/result, `g_http_server.begin()`, and first accepted HTTP request. Do not describe the AVR or HTTP cause as closed until that instrumentation distinguishes a late server start from a started but unreachable server.

## Planned remediation boundary

The correction should make application readiness independent of a complete AVR event-programming retry train while preserving automatic cloning and Chrome behavior. Candidate designs, in preferred investigation order, are:

1. start a minimal HTTP readiness/status service early and service it cooperatively during AVR synchronization, master discovery, and event loading;
2. promote the full HTTP server earlier only if its event and clone operations can be gated safely until initialization completes;
3. fail event programming as a transaction after the first conclusively failed Linkbus command instead of returning with the remaining commands still queued; and
4. explicitly control soft-AP persistence or advertisement only if doing so does not impair target discovery, automatic cloning, reconnect behavior, or established browser usability.

Starting the full event UI early without state gating is not an acceptable shortcut. The page must not race automatic cloning, expose partially initialized event state, or permit commands that conflict with startup AVR programming.

## Required verification before fleet deployment

Define the startup-readiness time budget before implementation, then cover these cases on a pilot master and target:

- ordinary cold boot with a scheduled event and healthy AVR acknowledgments;
- target boot with no master in range;
- target boot with a master in range and automatic clone attempted;
- no scheduled event;
- ESP-only restart while the AVR remains powered;
- missing, asleep, Linkbus-disabled, and nonresponsive AVR fault cases;
- successful browser reconnect after a temporary outage;
- Chrome plus at least Safari and Firefox-compatible HTTP behavior where available;
- unchanged clone result, next-event reporting, role programming, frequency programming, AVR keep-alive, and RF-safety behavior.

Use host tests for the boot-phase and timeout decisions, a deterministic ESP build, and timestamped hardware observations. Pilot the correction before considering another fleet ESP update.
