#!/bin/sh

set -eu

repo_root=$(git rev-parse --show-toplevel)
port=${FLEXFOX_ESP_PORT:-/dev/cu.usbserial-ABAKJ3Y8}
firmware=${FLEXFOX_FIRMWARE_BIN:-$repo_root/Software/Huzzah/tmp/esp-build/ARDF_Transmitter.ino.bin}
esptool=$repo_root/Software/Huzzah/tmp/arduino/data/packages/esp8266/hardware/esp8266/2.7.4/tools/esptool/esptool.py

if [ "${FLEXFOX_PROGRAM_CONFIRM:-}" != "PROGRAM FLEXFOX ESP" ]; then
	printf '%s\n' "Set FLEXFOX_PROGRAM_CONFIRM='PROGRAM FLEXFOX ESP' to authorize sketch programming" >&2
	exit 2
fi

case "$port" in
/dev/cu.usbserial-* | /dev/tty.usbserial-*) ;;
*)
	printf 'Refusing unexpected serial device: %s\n' "$port" >&2
	exit 2
	;;
esac

for required in "$port" "$firmware" "$esptool"; do
	if [ ! -e "$required" ]; then
		printf 'Required path does not exist: %s\n' "$required" >&2
		exit 2
	fi
done

printf 'Serial port: %s\n' "$port"
printf 'Sketch: %s\n' "$firmware"
shasum -a 256 "$firmware"

python3 "$esptool" \
	--chip esp8266 \
	--port "$port" \
	--baud 115200 \
	--before no_reset \
	--after no_reset \
	write_flash \
	--flash_mode dio \
	--flash_freq 40m \
	--flash_size 4MB \
	0x0 "$firmware"

python3 "$esptool" \
	--chip esp8266 \
	--port "$port" \
	--baud 115200 \
	--before no_reset \
	--after no_reset \
	verify_flash \
	0x0 "$firmware"

printf '%s\n' "PASS sketch write and independent readback verification completed"
printf '%s\n' "Press RESET to boot normally; this procedure did not erase or write LittleFS."
