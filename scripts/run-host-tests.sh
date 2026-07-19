#!/bin/sh

set -eu

repo_root=$(git rev-parse --show-toplevel)
build_dir="$repo_root/Software/AVR128DA48/tmp/host-tests"
cxx=${CXX:-c++}

if ! command -v "$cxx" >/dev/null 2>&1; then
	printf '%s\n' "Host C++ compiler not found: $cxx" >&2
	exit 2
fi

mkdir -p "$build_dir"

common_flags="-std=c++17 -Wall -Wextra -Werror -pedantic"
sanitizer_flags=""
if [ "${HOST_TEST_SANITIZERS:-1}" = "1" ]; then
	sanitizer_flags="-fsanitize=address,undefined -fno-omit-frame-pointer"
fi

printf '%s\n' "Host compiler: $($cxx --version | head -n 1)"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/AVR128DA48/FlexFox80/include" \
	"$repo_root/Software/AVR128DA48/FlexFox80/src/CircularStringBuff.cpp" \
	"$repo_root/Tests/Host/avr_circular_buffer_characterization_test.cpp" \
	-o "$build_dir/avr-circular-buffer-tests"

ASAN_OPTIONS="${ASAN_OPTIONS:+$ASAN_OPTIONS:}allocator_may_return_null=1" \
	"$build_dir/avr-circular-buffer-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/AVR128DA48/FlexFox80/include" \
	"$repo_root/Tests/Host/avr_bounded_text_copy_test.cpp" \
	-o "$build_dir/avr-bounded-text-copy-tests"

"$build_dir/avr-bounded-text-copy-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/AVR128DA48/FlexFox80/include" \
	"$repo_root/Tests/Host/avr_linkbus_rx_bounds_test.cpp" \
	-o "$build_dir/avr-linkbus-rx-bounds-tests"

"$build_dir/avr-linkbus-rx-bounds-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/AVR128DA48/FlexFox80/include" \
	"$repo_root/Tests/Host/avr_rtc_edge_tracker_test.cpp" \
	-o "$build_dir/avr-rtc-edge-tracker-tests"

"$build_dir/avr-rtc-edge-tracker-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/AVR128DA48/FlexFox80/include" \
	"$repo_root/Tests/Host/avr_rtc_sync_guard_test.cpp" \
	-o "$build_dir/avr-rtc-sync-guard-tests"

"$build_dir/avr-rtc-sync-guard-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/AVR128DA48/FlexFox80/include" \
	"$repo_root/Tests/Host/avr_event_schedule_state_test.cpp" \
	-o "$build_dir/avr-event-schedule-state-tests"

"$build_dir/avr-event-schedule-state-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/AVR128DA48/FlexFox80/include" \
	"$repo_root/Tests/Host/avr_wifi_power_lease_test.cpp" \
	-o "$build_dir/avr-wifi-power-lease-tests"

"$build_dir/avr-wifi-power-lease-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/AVR128DA48/FlexFox80/include" \
	"$repo_root/Tests/Host/avr_temperature_contract_test.cpp" \
	-o "$build_dir/avr-temperature-contract-tests"

"$build_dir/avr-temperature-contract-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/AVR128DA48/bootloader/include" \
	-I "$repo_root/Software/AVR128DA48/FlexFox80/include" \
	"$repo_root/Tests/Host/avr_boot_update_session_test.cpp" \
	-o "$build_dir/avr-boot-update-session-tests"

"$build_dir/avr-boot-update-session-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/Huzzah/ARDF_Transmitter" \
	"$repo_root/Tests/Host/esp_role_assignment_bounds_test.cpp" \
	-o "$build_dir/esp-role-assignment-bounds-tests"

"$build_dir/esp-role-assignment-bounds-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/Huzzah/ARDF_Transmitter" \
	"$repo_root/Tests/Host/esp_filesystem_startup_policy_test.cpp" \
	-o "$build_dir/esp-filesystem-startup-policy-tests"

"$build_dir/esp-filesystem-startup-policy-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/Huzzah/ARDF_Transmitter" \
	"$repo_root/Tests/Host/esp_event_file_integrity_test.cpp" \
	-o "$build_dir/esp-event-file-integrity-tests"

"$build_dir/esp-event-file-integrity-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/Huzzah/ARDF_Transmitter" \
	"$repo_root/Tests/Host/esp_event_data_read_guard_test.cpp" \
	-o "$build_dir/esp-event-data-read-guard-tests"

"$build_dir/esp-event-data-read-guard-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/Huzzah/ARDF_Transmitter" \
	"$repo_root/Tests/Host/esp_clone_event_manifest_test.cpp" \
	-o "$build_dir/esp-clone-event-manifest-tests"

"$build_dir/esp-clone-event-manifest-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/Huzzah/ARDF_Transmitter" \
	"$repo_root/Tests/Host/esp_fleet_soak_test.cpp" \
	-o "$build_dir/esp-fleet-soak-tests"

"$build_dir/esp-fleet-soak-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/Huzzah/ARDF_Transmitter" \
	"$repo_root/Tests/Host/esp_clone_keepalive_schedule_test.cpp" \
	-o "$build_dir/esp-clone-keepalive-schedule-tests"

"$build_dir/esp-clone-keepalive-schedule-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/Huzzah/ARDF_Transmitter" \
	"$repo_root/Tests/Host/esp_linkbus_command_transaction_test.cpp" \
	-o "$build_dir/esp-linkbus-command-transaction-tests"

"$build_dir/esp-linkbus-command-transaction-tests"

# shellcheck disable=SC2086 # Flags are intentionally expanded into individual compiler arguments.
"$cxx" $common_flags $sanitizer_flags \
	-I "$repo_root/Software/Huzzah/ARDF_Transmitter" \
	"$repo_root/Tests/Host/esp_firmware_update_integrity_test.cpp" \
	-o "$build_dir/esp-firmware-update-integrity-tests"

"$build_dir/esp-firmware-update-integrity-tests"
