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

`just wifi-linkbus-bounds-test` is a narrow writable-test-unit qualification for the AVR receive parser. It first proves that raw `PASS,$TEM?` produces a fresh reply, then sends two malformed field frames under the unrecognized `ZZZ` message ID and the overlength read-only alias reproducer `$AZRX?`, followed by the same temperature query after each rejection. It passes only if the malformed frame remains unanswered through the ESP retry delay and a fresh temperature reply proves that the next valid frame was parsed. Rejected frames produce no acknowledgment, so the ESP retries them under its existing three-attempt policy before releasing the queued query; the test allows 15 seconds for that approximately nine-second cycle. Do not generalize this recipe into arbitrary `PASS` forwarding; raw pass-through also exposes configuration, RF, reset, clock, EEPROM, and WiFi-shutdown commands.

Useful overrides:

```text
FLEXFOX_PROBE_DRY_RUN=1 just wifi-probe
FLEXFOX_URL=http://73.73.73.73/ FLEXFOX_PROBE_TIMEOUT_MS=15000 just wifi-probe
FLEXFOX_PROBE_DRY_RUN=1 just wifi-monitor
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

Do not change macOS routing, Internet Sharing, the FlexFox AP address, or the ESP firmware merely to make the first smoke test convenient. First prove the default direct path, then document any repeatable dual-network arrangement separately.
