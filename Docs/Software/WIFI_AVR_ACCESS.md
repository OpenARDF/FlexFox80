# FlexFox WiFi-to-AVR Access

## Purpose

FlexFox does not currently expose a supported wired serial console. Normal interactive access is:

```text
Mac browser or probe -> ESP8266 access point -> HTTP/WebSocket -> 9600-baud Linkbus -> AVR128DA48
```

Atmel-ICE remains the programming and low-level readback path. WiFi is the supported path for observing and controlling the running product.

## Network endpoint

The ESP8266 starts a soft access point in AP+station mode:

- ordinary SSID: `Tx_` followed by the final four bytes of the ESP access-point MAC address;
- master SSID: `Tx_Master`;
- default password: empty (open network);
- ESP address and gateway: `73.73.73.73`;
- subnet: `255.255.255.0`;
- HTTP: port 80;
- WebSocket: port 81.

The access-point address is configurable in running ESP state, so `73.73.73.73` is the source default rather than proof of a particular unit's current value.

The HTTP root offers these checked-in pages:

- `/events.html` — event configuration;
- `/test.html` — engineering controls and direct-message field;
- `/radio.html` — live radio controls;
- file upload, download, and deletion pages.

Opening one of the main pages creates `ws://<page-host>:81/`. The WebSocket server starts when a station joins the ESP access point.

## Message path

Browser messages are textual commands such as `SSID` or `SW_VERSIONS`. The ESP handles some locally and translates others into Linkbus frames queued for the AVR. Linkbus uses a 9600-baud UART, acknowledges commands, and retries a pending message after an ACK timeout.

Examples:

| Browser command | ESP behavior | AVR effect |
| --- | --- | --- |
| `SSID` | Reports the ESP access-point name | None |
| `MAC` | Reports the connected client's MAC address | None |
| `SW_VERSIONS` | Reports ESP version and cached AVR version | None |
| `MASTER` | Reports current ESP master/slave state when no value is supplied | None |
| WebSocket connect | Queues `$TEM?` and `$BAT?` | Reads temperature and battery voltage |
| `PASS,$TIM?` | Queues the raw Linkbus query | Reads RTC time and refreshes AVR system time from the RTC |

The ESP translates AVR replies back to WebSocket messages including `TEMP`, `BAT`, `SYNC`, `ERR_CODE`, `STATUS`, and `POWER`.

## Read-only Mac probe

After joining the FlexFox SSID, run:

```text
just wifi-probe
```

The probe uses Node's built-in HTTP and WebSocket clients. It:

1. verifies the root HTTP page;
2. opens the port-81 WebSocket;
3. sends only `SSID`, `MAC`, `SW_VERSIONS`, `MASTER`, and the `!&` heartbeat;
4. waits for the ESP's automatic live AVR temperature and battery requests;
5. fails unless both ESP identity replies and live AVR replies are observed.

It does not synchronize the clock, change settings, load or execute an event, key the transmitter, save EEPROM, shut off WiFi, or use raw pass-through.

For an extended hardware session, keep DroidTether running and hold the safe WebSocket open:

```text
just wifi-monitor
```

Monitor mode performs the same initial proof, then sends only `!&` every five seconds until interrupted with Ctrl-C. The ESP disconnects a WebSocket after approximately ten seconds without text traffic, so a 30-second heartbeat cannot preserve one continuous socket. Five seconds leaves margin for scheduling delay while reducing the built-in pages' two-second heartbeat rate. This resets the ESP WebSocket activity timer and allows the ESP/AVR keep-alive behavior to remain active while the Moto stays associated with the FlexFox AP.

`just wifi-linkbus-bounds-test` is a narrow writable-test-unit qualification for the AVR receive parser. It first proves that raw `PASS,$TEM?` and `PASS,$BAT?` produce fresh replies. It then sends two malformed field frames under the unrecognized `ZZZ` message ID and the overlength read-only alias reproducer `$AZRX?`, followed by both recovery queries. It passes only if each dropped frame remains unanswered through the ESP retry delay and fresh temperature and battery replies prove that the queued valid frames were parsed. Requiring both replies after the delay prevents an incidental periodic broadcast from satisfying the check.

The same recipe sends the read-only `$RXW?` probe twice. The legacy decimal ID encoding aliases `RXW` to `TEM` and therefore produces a visible temperature reply for every probe; the collision-free parser instead returns a Linkbus NAK, which the ESP consumes without broadcasting. The test fails only if both short observation windows contain temperature replies, tolerating one incidental periodic update, then requires a fresh battery reply to prove recovery. Do not generalize this recipe into arbitrary `PASS` forwarding; raw pass-through also exposes configuration, RF, reset, clock, EEPROM, and WiFi-shutdown commands.

To observe clock phase without setting time or changing configuration, run:

```text
just wifi-clock-observe
```

The observer sends only the `!&` heartbeat every five seconds and samples the ESP's `SYNC,<epoch>` broadcasts from the AVR. The five-second interval is required by the deployed module's approximately ten-second WebSocket inactivity timeout; a 30-second interval does not preserve one continuous observation. It reports the Mac receive time, target epoch, median offset, and sample spread. A positive offset means the target-reported epoch is behind the Mac; a negative offset means it is ahead. Absolute offset includes WebSocket and USB-tunnel latency, so compare unit medians only through the same network path. See the [wireless time synchronization investigation](Evidence/WIRELESS_TIME_SYNC_INVESTIGATION_2026-07-12.md) for the protocol limitations and multi-unit measurement plan.

On an explicitly authorized dummy-loaded test unit, qualify RTC writes with:

```text
FLEXFOX_CLOCK_SYNC_DRY_RUN=1 just wifi-clock-sync-test
FLEXFOX_ALLOW_CLOCK_SET=1 FLEXFOX_CLOCK_SYNC_TRIALS=10 just wifi-clock-sync-test
```

This state-changing test alternates distinctive `+8`, `-8`, and current-time signatures so each RTC write can be distinguished from the previous value. It requires every signature in the AVR's returned epochs and restores current Mac time after the series or a handled failure. Run it only when brief schedule changes are acceptable. It does not change event files, EEPROM configuration, or RF settings.

Useful overrides:

```text
FLEXFOX_PROBE_DRY_RUN=1 just wifi-probe
FLEXFOX_URL=http://73.73.73.73/ FLEXFOX_PROBE_TIMEOUT_MS=15000 just wifi-probe
FLEXFOX_PROBE_DRY_RUN=1 just wifi-monitor
FLEXFOX_CLOCK_DRY_RUN=1 just wifi-clock-observe
FLEXFOX_CLOCK_SAMPLES=30 FLEXFOX_CLOCK_TIMEOUT_MS=120000 just wifi-clock-observe
```

## Safety classification

### Safe initial observations

- HTTP root and static page retrieval;
- `SSID`, `MAC`, `SW_VERSIONS`, and query-only `MASTER`;
- the automatic temperature and battery queries sent at WebSocket connection;
- WebSocket heartbeat `!&`.

### State-changing operations

- `SYNC` writes the AVR RTC;
- callsign, pattern, frequency, power, modulation, speed, and event commands change live configuration;
- `CLEAR`, `PREP`, `EXECUTE`, and `MASTER,<value>` change event or ESP state;
- `WIFI_OFF` can remove the active access path.

### RF-active or hazardous operations

- `XMIT` begins immediate transmission when interlocks permit;
- `KEY_DOWN` and raw `$KEY,[;` key the transmitter;
- `/radio.html` exposes keying and arbitrary manual Morse input;
- `PASS,<text>` forwards arbitrary text to the AVR Linkbus without command allow-listing.

The current bench unit is connected to a dummy load, but RF-active commands still require an explicit test objective, expected response, stop command, and independent observation. Do not use `PASS` during initial connectivity testing.

## Mac networking

The proven simultaneous-connectivity arrangement is:

1. keep the Mac WiFi interface associated with ScharStar 2 so its normal default route and internet access remain unchanged;
2. associate the Moto with the FlexFox `Tx_...` access point;
3. connect the Moto to the Mac by USB and enable Moto USB tethering;
4. start DroidTether with default routing disabled;
5. add a host-only route for `73.73.73.73` through the `utun` interface created by DroidTether;
6. keep DroidTether running for the entire FlexFox session;
7. run `just wifi-probe`, followed by `just wifi-monitor` for extended work.

The DroidTether `utun` number can change between sessions. Identify the active interface rather than permanently assuming `utun6`. Verify the IPv4 routing table before probing:

- the ordinary `default` destination must still use the Mac WiFi interface and ScharStar 2 gateway;
- `73.73.73.73` must be the narrow host route through the active DroidTether `utun`;
- DroidTether must not install a competing default route.

In the first successful session, the default route remained on `en0` through `10.0.4.1`, while only `73.73.73.73` used `utun6`.

### DroidTether diagnostics learned on the Moto

Before starting DroidTether, the Moto must actually expose the RNDIS USB function. With Android debugging available, verify:

```text
adb shell getprop sys.usb.state
```

The value should include `rndis` (the proven state was `rndis,adb`). A value of only `adb` exposes the Android Debug Bridge interface `ff/42/01`, not RNDIS. DroidTether v0.8.7's broad known-Motorola fallback can misclassify that ADB interface as RNDIS and then misleadingly time out waiting for `INIT_CMPLT`. Confirm the phone's **USB tethering** switch remains enabled before diagnosing the Mac route.

Do not assume a fixed DroidTether client subnet. The Moto has assigned both `10.154.x.x` and `10.75.18.x` leases in successful sessions. Locate the newly created `utun` carrying an IPv4 address, then install the `73.73.73.73` host route through that interface. A helper that recognizes only `10.154.x.x` can stop a fully successful RNDIS/DHCP session and falsely report that no tunnel was created.

A successful path has four separate proofs:

1. the RNDIS handshake reaches data mode;
2. DHCP configures an IPv4 address on the new `utun`;
3. `route -n get 73.73.73.73` names that `utun`, not `en0`;
4. `just wifi-probe` returns HTTP, ESP identity, temperature, and battery data.

Do not change macOS routing, Internet Sharing, the FlexFox AP address, or the ESP firmware merely to make the first smoke test convenient. First prove the default direct path, then document any repeatable dual-network arrangement separately.
