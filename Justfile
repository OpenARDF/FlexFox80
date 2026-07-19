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
    node ./scripts/check-firmware-source-headers.mjs
    node ./scripts/check-release-checklist.mjs --file ./Docs/Software/release-checklist-template.json --phase template

# Build and run dependency-light host characterization tests.
test:
    ./scripts/run-host-tests.sh
    node --check ./scripts/qualify-flexfox-esp-filesystem-recovery.mjs
    node --check ./scripts/test-flexfox-wifi-maintenance-lease.mjs
    node --check ./scripts/qualify-flexfox-avr-bootloader.mjs
    node --check ./scripts/update-flexfox-avr-over-wifi.mjs
    node ./Tests/Host/eeprom_enum_layout_migration_test.mjs
    node ./Tests/Host/avr_update_image_format_test.mjs
    node ./Tests/Host/avr_readback_normalization_test.mjs
    node ./Tests/Host/esp_root_page_test.mjs
    node ./Tests/Host/esp_firmware_update_page_test.mjs
    node ./Tests/Host/wifi_avr_update_identity_test.mjs
    node ./Tests/Host/flexfox_http_test.mjs
    node ./Tests/Host/events_html_test.mjs
    node ./Tests/Host/fleet_soak_event_generator_test.mjs
    node ./Tests/Host/fleet_soak_page_test.mjs
    node ./Tests/Host/fleet_soak_protocol_test.mjs
    node ./Tests/Host/fleet_upgrade_workflow_test.mjs
    node ./Tests/Host/release_checklist_validator_test.mjs
    node --check ./scripts/upgrade-flexfox-fleet-unit.mjs
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

# Build a non-destructive ESP image that deliberately serves only filesystem recovery routes.
esp-filesystem-recovery-build:
    FLEXFOX_ESP_FILESYSTEM_RECOVERY_QUALIFICATION=1 node ./scripts/build-esp8266.mjs

# Install the recovery-only ESP image on an authorized pilot, then restore the normal image through it.
esp-filesystem-recovery-qualify:
    node ./scripts/qualify-flexfox-esp-filesystem-recovery.mjs

# Build the bootloader-capable application at the permanent 0x4000 app address.
avr-relocated-build:
    FLEXFOX_AVR_APP_START=0x4000 node ./scripts/build-avr-release.mjs

# Build the resident AVR bootloader for the 16 KiB BOOT section.
avr-bootloader-build:
    node ./scripts/build-avr-bootloader.mjs

# Build both halves and create the one-time initial provisioning image.
avr-boot-chain-build: avr-bootloader-build avr-relocated-build
    node ./scripts/package-avr-boot-chain.mjs

# Build qualification artifacts at every supported bootloader UART rate, then restore 38400 as the default.
avr-bootloader-baud-matrix: avr-relocated-build
    node ./scripts/build-avr-bootloader-baud-matrix.mjs

# One-time destructive fleet provisioning; requires both explicit confirmation variables.
avr-provision-boot-chain:
    ./scripts/provision-flexfox-avr-boot-chain.sh

# Read the attached AVR identity through Atmel-ICE without programming it.
# Entering UPDI programming mode may briefly reset a running transmitter.
avr-probe:
    avrdude -c atmelice_updi -p 128da48 -P usb -B 10 -n -v

# Probe the running FlexFox through its read-only WiFi/WebSocket path.
wifi-probe:
    node ./scripts/probe-flexfox-wifi.mjs

# Keep the FlexFox WiFi module active with the read-only WebSocket heartbeat.
wifi-monitor:
    FLEXFOX_PROBE_MONITOR=1 node ./scripts/probe-flexfox-wifi.mjs

# Prove an ESP-managed upload survives beyond the former two-minute AVR cutoff.
wifi-maintenance-lease-test:
    node ./scripts/test-flexfox-wifi-maintenance-lease.mjs

# Observe reported clock phase without setting time or changing configuration.
wifi-clock-observe:
    node ./scripts/observe-flexfox-clock.mjs

# Exercise RTC setting on an explicitly authorized dummy-loaded test unit.
wifi-clock-sync-test:
    node ./scripts/test-flexfox-clock-sync.mjs

# Separate RTC-edge phase from ordinary report latency on an authorized dummy-loaded unit.
wifi-clock-phase-test:
    node ./scripts/test-flexfox-clock-phase.mjs

# Qualify fixed Linkbus parser bounds and ID-alias rejection using read-only recovery.
wifi-linkbus-bounds-test:
    node ./scripts/test-flexfox-linkbus-rx-bounds.mjs

# Qualify clone quiet/edge/resume controls on an authorized dummy-loaded unit.
wifi-clone-control-test:
    node ./scripts/test-flexfox-clone-controls.mjs

# Exercise ESP role assignment and fail-safe restoration on an authorized dummy-loaded unit.
wifi-role-assignment-test:
    node ./scripts/test-flexfox-role-assignment.mjs

# Install and verify a sketch through the protected WiFi updater (explicit confirmation required).
wifi-esp-update:
    node ./scripts/update-flexfox-esp-over-wifi.mjs

# Stage, authorize with the unique ESP SSID suffix, program, and version-verify AVR firmware.
wifi-avr-update:
    node ./scripts/update-flexfox-avr-over-wifi.mjs

# Identify the exact FlexFox and AVR update state without staging an image.
wifi-avr-preflight:
    FLEXFOX_AVR_UPDATE_DRY_RUN=1 node ./scripts/update-flexfox-avr-over-wifi.mjs

# Run the unattended pilot sequence: normal update, ESP restart, AVR reset, and exact Atmel-ICE readback.
wifi-avr-bootloader-qualification:
    node ./scripts/qualify-flexfox-avr-bootloader.mjs

# Install and hash-verify events.html or another named LittleFS web file (explicit confirmation required).
wifi-web-deploy:
    node ./scripts/deploy-flexfox-web-file.mjs

# Read-only identity, artifact, relay, and per-file upgrade preflight for one fleet unit.
fleet-upgrade-preflight unit ssid:
    FLEXFOX_UNIT_ID="{{unit}}" FLEXFOX_SSID="{{ssid}}" FLEXFOX_FLEET_UPGRADE_DRY_RUN=1 node ./scripts/upgrade-flexfox-fleet-unit.mjs

# Upgrade and independently verify one opened fleet unit; requires the top-level confirmation variable.
fleet-upgrade-unit unit ssid:
    FLEXFOX_UNIT_ID="{{unit}}" FLEXFOX_SSID="{{ssid}}" node ./scripts/upgrade-flexfox-fleet-unit.mjs

# Generate the alternating 12-event, ten-unit fleet soak bundle for an explicit UTC start.
fleet-soak-events start:
    node ./scripts/generate-flexfox-fleet-soak-events.mjs --start "{{start}}"

# Build the ESP8266 sketch and LittleFS image with the qualified pinned profile.
esp-build:
    node ./scripts/build-esp8266.mjs

# Program and independently verify only the ESP sketch region (explicit confirmation required).
esp-program-sketch:
    ./scripts/program-flexfox-esp-sketch.sh

# Validate a release-specific checklist at the requested phase.
release-checklist file phase:
    node ./scripts/check-release-checklist.mjs --file "{{file}}" --phase "{{phase}}"
