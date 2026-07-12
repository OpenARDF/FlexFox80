default:
    @just --list

# Show the current branch and concise worktree state.
status:
    git status --short --branch

# Verify that the local machine has the tools needed for Step A1 checks.
doctor:
    ./scripts/repository-doctor.sh

# Verify local links in internal Markdown documentation.
docs-check:
    node ./scripts/check-markdown-links.mjs

# Verify ignore rules, attributes, JSON project files, and diff hygiene.
policy-check:
    ./scripts/check-repository-policy.sh

# Build and run dependency-light host characterization tests.
test:
    ./scripts/run-host-tests.sh
    node ./scripts/check-firmware-contracts.mjs
    node ./scripts/check-eeprom-layout.mjs

# Run all currently available non-firmware checks and host tests.
check: doctor docs-check policy-check test

# Scan repository history using the exact-fingerprint false-positive baseline.
secrets:
    gitleaks detect --source . --no-banner --redact

# Build the AVR Release firmware with the pinned compiler/device pack.
avr-build:
    node ./scripts/build-avr-release.mjs

# Read the attached AVR identity through Atmel-ICE without programming it.
# Entering UPDI programming mode may briefly reset a running transmitter.
avr-probe:
    avrdude -c atmelice_updi -p 128da48 -P usb -n -v

# Probe the running FlexFox through its read-only WiFi/WebSocket path.
wifi-probe:
    node ./scripts/probe-flexfox-wifi.mjs

# Keep the FlexFox WiFi module active with the read-only WebSocket heartbeat.
wifi-monitor:
    FLEXFOX_PROBE_MONITOR=1 node ./scripts/probe-flexfox-wifi.mjs

# Observe reported clock phase without setting time or changing configuration.
wifi-clock-observe:
    node ./scripts/observe-flexfox-clock.mjs

# Exercise RTC setting on an explicitly authorized dummy-loaded test unit.
wifi-clock-sync-test:
    node ./scripts/test-flexfox-clock-sync.mjs

# Qualify fixed Linkbus parser bounds and ID-alias rejection using read-only recovery.
wifi-linkbus-bounds-test:
    node ./scripts/test-flexfox-linkbus-rx-bounds.mjs

# Build the ESP8266 sketch and LittleFS image with the qualified pinned profile.
esp-build:
    node ./scripts/build-esp8266.mjs
