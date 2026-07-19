#!/bin/sh

set -eu

repo_root=$(git rev-parse --show-toplevel)
manifest="$repo_root/Software/AVR128DA48/tmp/avr-boot-chain/FlexFox80-AVR-Release-Info.json"

if [ ! -f "$manifest" ]; then
	printf '%s\n' "Boot-chain release manifest is missing. Run just avr-boot-chain-build first." >&2
	exit 2
fi
artifact_name=$(jq -er '.initialInstallFile' "$manifest")
application_version=$(jq -er '.applicationVersion' "$manifest")
case "$artifact_name" in
FlexFox80-AVR-First-Install-*.hex) ;;
*)
	printf '%s\n' "Manifest initial-install filename is not a guarded FlexFox HEX name." >&2
	exit 2
	;;
esac
artifact="$repo_root/Software/AVR128DA48/tmp/avr-boot-chain/$artifact_name"

if [ ! -f "$artifact" ]; then
	printf '%s\n' "First-install image is missing. Run just avr-boot-chain-build first." >&2
	exit 2
fi
if ! command -v avrdude >/dev/null 2>&1; then
	printf '%s\n' "avrdude is required." >&2
	exit 2
fi
if [ "${FLEXFOX_PROVISION_CONFIRM:-}" != "PROVISION-BOOTLOADER" ]; then
	printf '%s\n' "Refusing to erase or program the target." >&2
	printf '%s\n' "Set FLEXFOX_PROVISION_CONFIRM=PROVISION-BOOTLOADER after identifying the connected fleet unit." >&2
	exit 2
fi
if [ "${FLEXFOX_FUSE_CONFIRM:-}" != "WRITE-BOOTSIZE-0x20" ]; then
	printf '%s\n' "Refusing to change the permanent boot layout without the fuse confirmation." >&2
	printf '%s\n' "Set FLEXFOX_FUSE_CONFIRM=WRITE-BOOTSIZE-0x20 to authorize CODESIZE=0x00 and BOOTSIZE=0x20." >&2
	exit 2
fi

unit_id=${FLEXFOX_UNIT_ID:-unlabeled}
case "$unit_id" in
*[!A-Za-z0-9._-]*)
	printf '%s\n' "FLEXFOX_UNIT_ID may contain only letters, digits, dot, underscore, and hyphen." >&2
	exit 2
	;;
esac

capture_root="$repo_root/Software/AVR128DA48/tmp/fleet-provision"
mkdir -p "$capture_root"
capture_dir=$(mktemp -d "$capture_root/${unit_id}-XXXXXXXX")
eeprom_before="$capture_dir/eeprom-before.bin"
eeprom_restore="$capture_dir/eeprom-restored.bin"
fuses_before="$capture_dir/fuses-before.bin"
eeprom_after="$capture_dir/eeprom-after.bin"
fuses_after="$capture_dir/fuses-after.bin"

programmer_args="-c atmelice_updi -p 128da48 -P usb -B 10"

printf '%s\n' "Provisioning unit: $unit_id"
printf '%s\n' "Evidence directory: $capture_dir"
printf '%s\n' "Reading identity, EEPROM, and fuses before any write..."
# shellcheck disable=SC2086
avrdude $programmer_args -n -v
# shellcheck disable=SC2086
avrdude $programmer_args -U "eeprom:r:$eeprom_before:r" -U "fuses:r:$fuses_before:r"

if [ "$(wc -c <"$eeprom_before" | tr -d ' ')" != "512" ]; then
	printf '%s\n' "EEPROM capture was not exactly 512 bytes; no write attempted." >&2
	exit 2
fi
if [ "$(wc -c <"$fuses_before" | tr -d ' ')" != "16" ]; then
	printf '%s\n' "Fuse capture was not exactly 16 bytes; no write attempted." >&2
	exit 2
fi

shasum -a 256 "$artifact" "$eeprom_before" "$fuses_before"

if [ "${FLEXFOX_PROVISION_SKIP_IF_CURRENT:-}" = "1" ]; then
	printf '%s\n' "Checking whether the exact boot-chain image is already installed..."
	flash_is_current=false
	# A failed verify means this unit still needs provisioning; it is not a script failure.
	# shellcheck disable=SC2086
	if avrdude $programmer_args -U "flash:v:$artifact:i"; then
		flash_is_current=true
	fi
	fuse7_before=$(od -An -tx1 -j 7 -N 1 "$fuses_before" | tr -d ' \n')
	fuse8_before=$(od -An -tx1 -j 8 -N 1 "$fuses_before" | tr -d ' \n')
	marker_before=$(od -An -tx1 -j 511 -N 1 "$eeprom_before" | tr -d ' \n')
	if $flash_is_current && [ "$fuse7_before" = "00" ] && [ "$fuse8_before" = "20" ] && [ "$marker_before" = "ff" ]; then
		printf '%s\n' "PASS: target already matches the exact boot-chain image, boot fuses, and cleared recovery marker; skipping write."
		exit 0
	fi
fi

cp "$eeprom_before" "$eeprom_restore"
printf '\377' | dd of="$eeprom_restore" bs=1 seek=511 conv=notrunc 2>/dev/null
printf '%s\n' "Erasing and programming the combined bootloader plus relocated AVR $application_version image..."
# shellcheck disable=SC2086
avrdude $programmer_args -e -U "flash:w:$artifact:i"
# shellcheck disable=SC2086
avrdude $programmer_args -U "flash:v:$artifact:i"

printf '%s\n' "Restoring unit-specific EEPROM with reserved update marker byte 511 cleared..."
# shellcheck disable=SC2086
avrdude $programmer_args -U "eeprom:w:$eeprom_restore:r" -U "eeprom:v:$eeprom_restore:r"

printf '%s\n' "Committing the permanent 16 KiB boot layout..."
# AVR128DA48 fuse offsets: CODESIZE=fuse7, BOOTSIZE=fuse8; values are 512-byte pages.
# shellcheck disable=SC2086
avrdude $programmer_args -U fuse7:w:0x00:m -U fuse8:w:0x20:m

printf '%s\n' "Performing independent post-write reads..."
# shellcheck disable=SC2086
avrdude $programmer_args -U "flash:v:$artifact:i" -U "eeprom:r:$eeprom_after:r" -U "fuses:r:$fuses_after:r"
cmp "$eeprom_restore" "$eeprom_after"
fuse7=$(od -An -tx1 -j 7 -N 1 "$fuses_after" | tr -d ' \n')
fuse8=$(od -An -tx1 -j 8 -N 1 "$fuses_after" | tr -d ' \n')
if [ "$fuse7" != "00" ] || [ "$fuse8" != "20" ]; then
	printf '%s\n' "Post-write fuse verification failed: CODESIZE=$fuse7 BOOTSIZE=$fuse8" >&2
	exit 2
fi
shasum -a 256 "$eeprom_after" "$fuses_after"

printf '%s\n' "PASS: flash verified, EEPROM restored except its reserved marker byte, CODESIZE=0x00, BOOTSIZE=0x20."
printf '%s\n' "Keep the chassis accessible until the pilot wireless update and interruption-recovery checklist passes."
