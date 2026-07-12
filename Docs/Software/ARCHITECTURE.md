# FlexFox80 Software Architecture

## Product-level design

FlexFox80 divides its software between two processors:

1. An Adafruit Huzzah ESP8266 provides Wi-Fi, the HTTP/WebSocket user interface, event-file storage, and master/clone coordination.
2. An AVR128DA48 provides deterministic event timing, Morse generation, transmitter control, hardware monitoring, power management, and sleep/wake behavior.

This split keeps the real-time and RF-safety path on the AVR. Once configured, an event can continue after the ESP8266 is powered down.

```text
Browser / phone / laptop
          |
          | HTTP and WebSocket
          v
ESP8266 Huzzah
  - web UI
  - LittleFS event files
  - event selection and editing
  - master/clone file distribution
          |
          | Linkbus, 9600 baud, USART
          v
AVR128DA48
  - authoritative active settings
  - DS3231 time and event scheduling
  - transmit-cycle and Morse timing
  - antenna and RF-output protection
  - Si5351, DAC, ADC, fan, LEDs, sleep
          |
          v
80 m RF hardware
```

## Source layout

| Area | Purpose |
| --- | --- |
| `Software/Huzzah/ARDF_Transmitter/ARDF_Transmitter.ino` | ESP8266 startup, Wi-Fi, HTTP, WebSockets, Linkbus orchestration, master/clone coordination, and top-level state machine |
| `Software/Huzzah/ARDF_Transmitter/Event.*` | Event-file model, parsing, validation, role/slot assignments, and LittleFS persistence |
| `Software/Huzzah/ARDF_Transmitter/Transmitter.*` | Shared protocol constants and small transmitter-settings model used by the ESP8266 |
| `Software/Huzzah/ARDF_Transmitter/data/` | Web pages, default settings, and example event files uploaded to LittleFS |
| `Software/AVR128DA48/FlexFox80/main.cpp` | AVR top-level state, interrupt handlers, event lifecycle, command handling, hardware monitoring, and sleep control |
| `Software/AVR128DA48/FlexFox80/driver_isr.cpp` | USART interrupt service routines and Linkbus receive parser |
| `Software/AVR128DA48/FlexFox80/src/linkbus.cpp` | Framed ESP8266-to-AVR communications and buffering |
| `Software/AVR128DA48/FlexFox80/src/serialbus.cpp` | Local text-command communications and buffering |
| `Software/AVR128DA48/FlexFox80/src/eeprommanager.cpp` | AVR nonvolatile settings layout, initialization, reads, and writes |
| `Software/AVR128DA48/FlexFox80/src/transmitter.cpp` | Frequency, power, antenna inhibit, Si5351 keying, DAC, and final-stage control |
| `Software/AVR128DA48/FlexFox80/src/ds3231.cpp` | RTC access, epoch conversion, square-wave setup, and calibration |
| `Software/AVR128DA48/FlexFox80/src/morse.cpp` | Incremental Morse state machine used from the timer interrupt |
| `Software/AVR128DA48/FlexFox80/src/i2c.cpp` | Low-level TWI0/TWI1 operations and timeouts |
| `Software/AVR128DA48/FlexFox80/src/si5351.cpp` | Clock-generator configuration and register access |
| `Software/AVR128DA48/FlexFox80/src/tcb.cpp` | Timer setup, millisecond delay support, and peripheral timeout counters |
| `Software/AVR128DA48/FlexFox80/src/adc.cpp` | ADC setup and temperature conversion |
| `Software/AVR128DA48/FlexFox80/src/binio.cpp` | Digital-input sampling and debouncing |
| `Software/AVR128DA48/FlexFox80/src/leds.cpp` | LED patterns and visual Morse enunciation |

## ESP8266 responsibilities

### Startup and persistent files

The ESP8266 starts its serial interface, LEDs, and LittleFS. It loads `defaults.txt`, creates the transmitter settings object, and allocates Linkbus buffers. Event definitions and per-transmitter assignments are stored in LittleFS as `.event` and `.me` files.

An event file describes:

- event identity and version;
- band and antenna assignment;
- callsign and callsign speed;
- start and finish timestamps;
- one or more transmitter roles;
- role frequency, power, code speed, and ID interval;
- pattern, on-time, off-time, and delay for each transmitter slot.

The corresponding `.me` file records the role and slot assigned to the individual transmitter.

### Master and clone behavior

The ESP8266 can act as a master or clone. A master exposes the configuration access point and can distribute event files and assignments to clones over WebSockets. A clone connects to the master, receives updated files, selects its assigned role, and then configures its local AVR.

### Configuration state machine

The long-running HTTP/WebSocket loop services browser clients and advances `g_ESP_Comm_State`. State transitions cover:

- loading and enumerating event files;
- serving event data to browsers;
- saving edits;
- selecting roles;
- transferring files to clones;
- sending an event to the AVR;
- starting transmission;
- requesting shutdown or communications disable.

The ESP calls `yield()` in long-running paths so the ESP8266 software watchdog and network stack can run.

## Linkbus contract

Linkbus is an ASCII framed protocol. Messages generally use:

```text
$ID,field1,field2;
$ID?
!ID,reply;
```

The ESP sends configuration commands for time, start/finish epochs, callsign, pattern, code speeds, transmit intervals, frequency, and power. The AVR reports acknowledgements, errors, status, time, battery, temperature, and software version.

On the AVR:

- USART receive interrupts parse a message into one of three receive buffers.
- The foreground loop consumes full buffers in `handleLinkBusMsgs()`.
- A configuration bitmask tracks receipt of the essential event parameters.
- `GO` commands prepare, activate, suspend, or immediately start an event.
- `PRM` persists the current settings to EEPROM.

The Linkbus protocol is a critical compatibility boundary. Changes should be tested against both processors together.

## AVR startup flow

The AVR startup sequence is approximately:

1. Initialize the MCU, 24 MHz clock, timers, interrupts, GPIO, ADC/DAC, serial buses, sleep controller, and brownout support.
2. Force transmitter power toward a safe state, enable the main rails, and start the fan.
3. Initialize EEPROM defaults if needed, then load nonvolatile settings.
4. Probe the ESP8266, power it back down temporarily, and record a hardware error if absent.
5. Initialize the Si5351 transmitter path with the RF clock disabled.
6. Initialize the DS3231 and its one-second square wave, with retries.
7. Set the system epoch from the RTC.
8. Determine whether a stored event is current or scheduled.
9. Enable the ESP8266 after a short delay and enter the foreground loop.

Hardware-error bits represent missing RTC, Si5351, Wi-Fi, 12 V supply, or FET bias. The current implementation actively sets the first three during startup.

## AVR time domains and interrupts

### DS3231 one-second interrupt

The DS3231 square wave enters through a PORTA interrupt. It:

- advances the C library system clock;
- moves the signed on/off-air countdown toward zero;
- marks a second transition;
- evaluates sleep wake-up conditions;
- runs one-second event and Wi-Fi tasks while awake.

### TCB0 real-time interrupt

TCB0 runs the high-frequency control path. It:

- debounces the button and antenna input;
- recognizes single, double, and long presses;
- advances the Morse generator;
- keys or unkeys the Si5351 output;
- moves between on-air and off-air portions of a cycle;
- schedules low-power sleep during long off-air periods;
- performs periodic ADC conversions.

This ISR owns much of the behavior users perceive as transmitter timing. Changes to it have a high regression cost.

### Other timers

- TCB1 supplies the utility millisecond delay counter and LED timing.
- TCB2 is a high-priority timeout source for I2C and serial waits.
- TCB3 is initialized for additional periodic work.

## Event lifecycle

The active event is represented by global start/finish epochs, pattern and callsign strings, Morse speeds, transmit durations, cycle delay, frequency, power, event kind, and fox assignment.

Before activation the AVR checks that:

- a start time is present;
- finish follows start;
- on-air duration is nonzero;
- delay is not longer than the nominal cycle;
- a pattern is present;
- pattern speed is valid;
- callsign speed and interval are valid when a callsign is present.

If the event has started, the AVR calculates its position in the repeating cycle from the start epoch. It either begins transmitting with the correct remaining duration or waits for the next slot. If the event is in the future, it can shut down peripherals and sleep until shortly before the start.

During transmission, the pattern repeats at its configured speed. The callsign is inserted according to the ID interval, with logic intended to allow it to finish cleanly before returning to the pattern or off-air interval.

At event finish, RF is disabled, the event is marked stopped, and the ESP8266 can be awakened to locate the next scheduled event.

## RF and hardware safety

The RF path has several layers:

- `powerToTransmitter()` controls final drain voltage.
- `keyTransmitter()` enables or disables the Si5351 clock output.
- `inhibitRFOutput()` forces final drain voltage off when the antenna is absent.
- transmit power is converted to one of 16 characterized DAC settings;
- initialization disables the RF clock before marking the transmitter initialized;
- sleep powers down the transmitter, ESP8266, main rails, fan, and communications.

The antenna state is debounced and monitored continuously. Disconnecting it inhibits output independently of the current requested final-stage state.

The ADC periodically samples external battery, regulated 12 V, and PA voltage. The CPU temperature controls the fan with hysteresis: on at 35 C and off at 30 C.

## Persistence

The AVR EEPROM holds the last active configuration. Most writes compare the requested value with EEPROM first to reduce wear. On startup, several numeric values are clamped to acceptable ranges.

The layout defines guard words between values, but the current code does not validate those guards. The only overall version/validity marker is `EEPROM_INITIALIZED_FLAG`.

The ESP8266 LittleFS is the higher-level source for event files. The AVR EEPROM is the immediate source used to resume or run the most recently transferred event when appropriate.

## Build and repository state

The AVR project targets AVR128DA48 using Atmel Studio 7, AVR-GCC 7.3.0, and the Microchip AVR-Dx device pack. The checked-in generated Makefile contains Windows-specific absolute paths.

The ESP8266 source is an Arduino sketch using ESP8266 Wi-Fi, WebSocket, and LittleFS libraries. No `arduino-cli` or PlatformIO configuration is currently checked in.

There is no repository-level test runner, CI workflow, or portable build wrapper. Generated AVR objects, dependency files, binaries, map/listing files, and IDE state are tracked in Git. These constraints are important when interpreting historical diffs or preparing reproducible validation.
