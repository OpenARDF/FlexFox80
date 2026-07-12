# Windows ESP8266 Arduino Profile Discovery - 2026-07-12

Request: `FF80-2026-07-12-016`

Host: Windows 11, user `charl`

Branch: `Development_AVR128DA48`

Result: the exact historical Arduino ESP8266 build profile was not recovered from this Windows account. Historical Receiver Development Platform / FlexFox ESP8266 source snapshots and a vendored `arduinoWebSockets` library were recovered from OneDrive-backed trees, but no installed ESP8266 Arduino core, board-manager URL, FQBN/options, cached FlexFox build metadata, LittleFS upload tooling, or retained ESP8266 binary/filesystem artifacts were found.

## Read-only Scope

The discovery pass inspected local files, OneDrive-backed source snapshots, Arduino IDE profile directories, logs, caches, and installed program locations. No Arduino package was installed or updated. No sketch was compiled, uploaded, or otherwise modified.

## Active Arduino Installation

- IDE executable: `C:\Program Files\Arduino IDE\Arduino IDE.exe`
- IDE file version: `2.3.5`
- IDE product version: `2.3.5.0`
- IDE executable size: `176745200` bytes
- IDE executable last write time: `2025-04-02 09:24:38 -0400`
- Bundled CLI: `C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe`
- CLI version output: `arduino-cli  Version: 1.2.0 Commit: 9c495211 Date: 2025-02-24T15:57:34Z`

Active Arduino IDE 2.x config:

- `C:\Users\charl\.arduinoIDE\arduino-cli.yaml` contains `board_manager.additional_urls: []`.
- `C:\Users\charl\.arduinoIDE\recent-sketches.json` contains only `C:\Users\charl\Documents\GitHub\Arducon\Software\Arduino\Arducon`.
- `C:\Users\charl\AppData\Local\Arduino15\packages` contains `arduino:avr` `1.8.6`, Arduino AVR tools, and built-in discovery/monitor tools. No `esp8266` package directory was present.
- `C:\Users\charl\AppData\Local\Arduino15\libraries` contains only built-in Arduino libraries: `Arduino_BuiltIn`, `Ethernet`, `Firmata`, `Keyboard`, `LiquidCrystal`, `Mouse`, `SD`, `Servo`, `Stepper`, and `TFT`.
- `C:\Users\charl\AppData\Local\arduino\sketches\4DDE584CF15F90167ED002D4856EC34A\build.options.json` is for Arducon, not FlexFox or Receiver Development Platform. Its FQBN is `arduino:avr:pro:cpu=16MHzatmega328`, with `sketchLocation` `C:\Users\charl\Documents\GitHub\Arducon\Software\Arduino\Arducon`.

## Historical Source Evidence Found

Two OneDrive-backed historical `ARDF_Transmitter` sketches were found.

Primary copy:

- Path: `C:\Users\charl\OneDrive\Digital Confections\Receiver-Development-Platform\ESP8266\ARDF_Transmitter\ARDF_Transmitter\ARDF_Transmitter.ino`
- Size: `105649` bytes
- Last write time: `2019-04-18 21:53:11 -0400`
- SHA-256: `18BD0AF3205B1F344F3C92B772259310DDE027674B3DAE59207DD0FCCFE35573`
- Evidence lines:
  - line 25: `Hardware Target: Adafruit HUZZAH ESP8266`
  - line 49: `#include <ESP8266WiFi.h>`
  - line 52: `#include <ESP8266WebServer.h>`
  - line 53: `#include <ESP8266WiFiMulti.h>`
  - line 56: `#include "esp8266.h"`
  - line 61: `#include <WebSocketsServer.h>`
  - line 62: `#include <ESP8266WiFiType.h>`

Duplicate OneDrive GitHub copy:

- Path: `C:\Users\charl\OneDrive\Documents\GitHub\Receiver-Development-Platform\ESP8266\ARDF_Transmitter\ARDF_Transmitter\ARDF_Transmitter.ino`
- Size: `102426` bytes
- Last write time: `2019-04-18 22:04:22 -0400`
- Attributes include `ReparsePoint`
- SHA-256: `104E35C14933F93EFF122824969FC69E1B06BA0FFE2A50116AF0F75F3EA3F38D`
- Evidence lines match the primary copy for the HUZZAH target and ESP8266/WebSockets includes listed above.

Older ESP8266 firmware source also exists:

- Path: `C:\Users\charl\OneDrive\Documents\GitHub\Receiver-Development-Platform\Transmitter Project\Source\ESP8266\RDP_WiFi_Firmware.ino`
- Size: `34141` bytes
- Last write time: `2017-06-20 23:11:27 -0400`
- Attributes include `ReparsePoint`
- SHA-256: `5DC2CF216F4F469F6D3B89EFBAF6E8CD29C6941DB9DBB63FEEA7A0C387C78739`
- Evidence lines:
  - line 25: `Hardware Target: Adafruit HUZZAH ESP8266`
  - line 43: `#include <ESP8266WiFi.h>`
  - line 45: `#include "esp8266.h"`

The Receiver Development Platform README was found in both OneDrive-backed trees:

- `C:\Users\charl\OneDrive\Digital Confections\Receiver-Development-Platform\README.md`
- `C:\Users\charl\OneDrive\Documents\GitHub\Receiver-Development-Platform\README.md`
- Size: `3993` bytes
- SHA-256: `4ADF305C8714FA37B3F69E67B5E49B7FC4ACE930F1843E705A98E70A0C9FDE09`
- Evidence line 26 says the ESP8266 project builds under Arduino IDE version `1.8.2 or later`, with board support installed for the Adafruit HUZZAH ESP8266 Breakout board.

## WebSockets Library Evidence Found

Primary historical vendored library:

- Path: `C:\Users\charl\OneDrive\Digital Confections\Receiver-Development-Platform\ESP8266\ARDF_Transmitter\libraries\WebSockets\library.properties`
- Size: `271` bytes
- Last write time: `2019-03-19 19:06:25 -0400`
- SHA-256: `CE35FA49D109F9F72F6E03A4C20E484F09E36A7BC3F41E3B199AD652A8AE1C12`
- `version=2.1.0`
- `url=https://github.com/Links2004/arduinoWebSockets`

Duplicate OneDrive GitHub copy:

- Path: `C:\Users\charl\OneDrive\Documents\GitHub\Receiver-Development-Platform\ESP8266\ARDF_Transmitter\libraries\WebSockets\library.properties`
- Size: `262` bytes
- Last write time: `1979-11-30 00:00:00 -0500`
- Attributes include `ReparsePoint`
- SHA-256: `6BCABC0823540CBE09A418148350E0279E4AEB654A69B2062458BACB663F23B6`
- `version=2.1.0`
- `url=https://github.com/Links2004/arduinoWebSockets`

This is the only specific `arduinoWebSockets` version recovered.

## Requested Profile Fields

1. Arduino IDE and CLI versions:
   - Active IDE: Arduino IDE `2.3.5`.
   - Active bundled CLI: `arduino-cli` `1.2.0`, commit `9c495211`, date `2025-02-24T15:57:34Z`.
   - Historical README requirement: Arduino IDE `1.8.2 or later`.
   - No historical Arduino IDE 1.x installation with exact executable version was found.

2. ESP8266 Arduino core version and package source/additional URL:
   - Not found.
   - Active `arduino-cli.yaml` has no additional board-manager URLs.
   - `C:\Users\charl\AppData\Local\Arduino15\packages` has no `esp8266` vendor/package directory.
   - No `package_esp8266com_index.json`, ESP8266 `boards.txt`, or ESP8266 `platform.txt` from an installed core was found in the inspected Arduino profile locations.

3. Selected FQBN and non-default board options:
   - Not found.
   - Source/README evidence identifies the target board as Adafruit HUZZAH ESP8266 Breakout.
   - No cached FlexFox/ARDF `build.options.json`, compile database, or preferences file proving FQBN or options was found.
   - CPU frequency, flash layout, debug level, lwIP variant, erase behavior, upload speed, port, and programmer values remain unrecovered.

4. `arduinoWebSockets` version, source, and path:
   - Found vendored library version `2.1.0`.
   - Source URL: `https://github.com/Links2004/arduinoWebSockets`.
   - Paths are listed in "WebSockets Library Evidence Found".

5. LittleFS library/core and filesystem upload-tool versions and commands:
   - Not found.
   - The historical `ARDF_Transmitter.ino` copies inspected in this pass do not include `<LittleFS.h>`.
   - No installed `mklittlefs`, `esp8266fs`, LittleFS upload plugin, or retained filesystem upload command was found in the inspected profile and source-cache locations.

6. Sketchbook path and duplicate library-resolution candidates:
   - Historical sketch candidates:
     - `C:\Users\charl\OneDrive\Digital Confections\Receiver-Development-Platform\ESP8266\ARDF_Transmitter`
     - `C:\Users\charl\OneDrive\Documents\GitHub\Receiver-Development-Platform\ESP8266\ARDF_Transmitter`
   - Active Arduino cache points user libraries to `C:\Users\charl\Documents\Arduino\libraries`.
   - `C:\Users\charl\Documents\Arduino` and `C:\Users\charl\OneDrive\Documents\Arduino` were present or plausible sketchbook locations, but no active WebSockets/LittleFS/FlexFox profile was recovered there.
   - Duplicate library candidates are the historical vendored `libraries` directories under both `ARDF_Transmitter` copies plus active built-in libraries under `C:\Users\charl\AppData\Local\Arduino15\libraries`.

7. Cached compile command or build metadata proving FlexFox settings:
   - Not found.
   - The only active Arduino build cache with `build.options.json` was for Arducon AVR, not FlexFox/Receiver Development Platform ESP8266.

8. Retained ESP binary/filesystem artifacts:
   - Not found.
   - Searches found AVR/Atmel Studio outputs and Arduino AVR/WiFi-shield firmware binaries in historical Receiver Development Platform and Arduino source trees, but no retained ESP8266 FlexFox/ARDF `.bin`, LittleFS image, or SPIFFS image with reliable provenance.

## Locations Checked

- `C:\Users\charl\AppData\Local\Arduino15`
- `C:\Users\charl\AppData\Local\arduino`
- `C:\Users\charl\.arduinoIDE`
- `C:\Users\charl\AppData\Roaming\Arduino IDE`
- `C:\Users\charl\AppData\Roaming\arduino-ide`
- `C:\Users\charl\Documents\Arduino`
- `C:\Users\charl\OneDrive\Documents\Arduino`
- `C:\Program Files\Arduino IDE`
- `C:\Program Files\Arduino`
- `C:\Program Files (x86)\Arduino`
- `C:\Users\charl\OneDrive\Digital Confections\Receiver-Development-Platform`
- `C:\Users\charl\OneDrive\Documents\GitHub\Receiver-Development-Platform`
- `C:\Users\charl\OneDrive\Digital Confections\Receiver_Development_Platform`
- `C:\Users\charl\OneDrive\Digital Confections\Receiver_Development_Platform_old`
- `C:\Users\charl\Downloads`
- `C:\Users\charl\Desktop`
- `C:\Users\charl\Documents`
- `C:\Users\charl\OneDrive`
- `C:\ProgramData`
- Windows user profiles under `C:\Users`: `charl`, `Default`, and `Public`

Search terms and artifacts included `ARDF_Transmitter`, `RDP_WiFi_Firmware`, `esp8266`, `WebSocketsServer.h`, `WebSocketsClient.h`, `arduinoWebSockets`, `HUZZAH`, `build.options.json`, `package_esp8266com_index.json`, `boards.txt`, `platform.txt`, `preferences.txt`, `arduino-cli.yaml`, `recent-sketches.json`, `mklittlefs`, `esp8266fs`, `.bin`, `.elf`, `.hex`, `.littlefs`, and `.spiffs`.

## Conclusion

Windows recovered enough evidence to identify the historical ESP8266 target and WebSockets dependency:

- Target: Adafruit HUZZAH ESP8266 Breakout.
- Historical IDE requirement: Arduino IDE `1.8.2 or later`.
- WebSockets dependency: vendored `arduinoWebSockets` `2.1.0`.

Windows did not recover the exact historical build profile. The missing profile data should be sought on the other computer or in external backups/archives before attempting to reconstruct compile/upload commands.
