# Mac WiFi-to-AVR Probe Evidence — 2026-07-12

## Network arrangement

- Mac WiFi and default route remained on ScharStar 2 through `en0` and gateway `10.0.4.1`.
- The Moto was associated with the FlexFox access point and connected to the Mac through USB tethering.
- DroidTether ran with default routing disabled.
- DroidTether exposed `utun6` with local address `10.154.27.3` and peer `10.154.27.82`.
- The Mac routing table contained only a FlexFox-specific route: `73.73.73.73` through `utun6`.

The ordinary Mac default route remained on `en0`; FlexFox traffic alone used DroidTether.

## Read-only result

`just wifi-probe` completed successfully through the routed Moto connection:

```text
PASS HTTP 200 http://73.73.73.73/
PASS WebSocket connected ws://73.73.73.73:81/
RECV TEMP,31.0C
RECV BAT,12.3V
RECV SYNC,1783873467
RECV SSID,Tx_7C2D69ED
RECV MAC,1A:0D:BB:2E:2C:4C
RECV SW_VERSIONS,2.0,0.200
RECV MASTER,0
PASS WiFi-to-AVR read-only path returned temperature and battery data
```

Additional `SYNC` broadcasts arrived while the socket remained open. The probe did not send `SYNC`; those messages were AVR time reports translated by the ESP.

This proves:

- the routed Mac-to-ESP HTTP path;
- the routed Mac-to-ESP WebSocket path;
- ESP identity and state replies;
- live ESP-to-AVR Linkbus requests;
- live AVR temperature, battery, version, and RTC replies translated back to the WebSocket.

No configuration, EEPROM, RTC, event, WiFi-shutdown, immediate-transmit, or manual-keying command was sent. The transmitter was connected to a dummy load.

## Keep-alive requirement

The ESP WebSocket activity timer requires periodic traffic. Its ten-second socket timeout makes a 30-second continuous-socket heartbeat too slow. `just wifi-monitor` holds the same read-only socket open and sends `!&` every five seconds, leaving margin for scheduling delay. DroidTether must remain running, the Moto must remain associated with the FlexFox AP, and the host route must remain assigned to DroidTether's active `utun` interface during the test session.
