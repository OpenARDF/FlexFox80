# ESP HTTP Startup Delay After Wi-Fi Association

**Date:** 2026-07-16

**Path:** B-HTTP-03

**Status:** Characterized from live timing and source inspection; AVR-ACK trigger and remediation remain planned

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

## Why the delay can approach three minutes

The current target-side worst-case startup path closely explains the live 178.332-second observation:

| Startup work | Current bound or retry cost |
| --- | ---: |
| Search for `Tx_Master` when it is unavailable | approximately 15.25 seconds (`tries > 60` at 250 ms intervals) |
| Send the 17 queued AVR event-configuration commands without acknowledgments | up to approximately 153 seconds (three 3-second acknowledgment windows per command) |
| Final event-programming confirmation after a Linkbus timeout | up to 10 seconds |
| Approximate combined delay before HTTP setup | 178.25 seconds |

The retry countdown is aligned to integer `millis() / 1000` boundaries, so individual runs need not equal that arithmetic exactly. Nevertheless, the source-derived budget and the observed 178.332-second first response agree closely enough to identify the delayed pre-HTTP AVR programming path rather than browser startup as the controlling mechanism.

Normal acknowledged AVR communication should drain the event queue much faster. Reaching the long bound implies that acknowledgments were unavailable for much or all of the boot-time event transfer.

## Remaining root-cause question

The investigation explains both why Wi-Fi can look connected before HTTP is ready and why the unavailable interval can last several minutes. It does not yet prove why AVR acknowledgments were missing on each affected startup.

The next investigation must distinguish at least:

- an ESP-only restart while the AVR remains in a state that cannot answer normal Linkbus commands;
- an asleep, powered-down, or restarting AVR;
- Linkbus having been disabled for ESP serial programming, which the current AVR treats as a power-cycle-only state;
- startup baud-calibration or receive-state loss after the initial time exchange;
- a transport or tether failure that happens to overlap application startup.

Do not describe the AVR cause as closed until timestamped Linkbus or equivalent phase instrumentation shows which condition occurred.

## Planned remediation boundary

The correction should make application readiness independent of a complete AVR event-programming retry train while preserving automatic cloning and Chrome behavior. Candidate designs, in preferred investigation order, are:

1. start a minimal HTTP readiness/status service early and service it cooperatively during AVR synchronization, master discovery, and event loading;
2. promote the full HTTP server earlier only if its event and clone operations can be gated safely until initialization completes;
3. fail event programming as a transaction after the first conclusively failed Linkbus command instead of spending the full retry allowance on every remaining queued command; and
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
