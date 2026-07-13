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
