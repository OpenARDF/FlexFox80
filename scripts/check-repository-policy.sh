#!/bin/sh

set -eu

repo_root=$(git rev-parse --show-toplevel)
cd "$repo_root"

failure=0

expect_ignored() {
	path=$1
	if git check-ignore -q --no-index "$path"; then
		printf '%s\n' "ignored as expected: $path"
	else
		printf '%s\n' "ERROR: expected ignored path is visible: $path" >&2
		failure=1
	fi
}

expect_visible() {
	path=$1
	if git check-ignore -q --no-index "$path"; then
		printf '%s\n' "ERROR: project asset would be ignored: $path" >&2
		failure=1
	else
		printf '%s\n' "visible as expected: $path"
	fi
}

expect_eol() {
	expected=$1
	path=$2
	actual=$(git check-attr eol -- "$path" | sed 's/^.*: eol: //')
	if [ "$actual" = "$expected" ]; then
		printf '%s\n' "eol=$expected: $path"
	else
		printf '%s\n' "ERROR: $path has eol=$actual, expected $expected" >&2
		failure=1
	fi
}

expect_ignored "Software/AVR128DA48/.vs/example.suo"
expect_ignored "Software/AVR128DA48/tmp/verification.log"
expect_ignored "KiCad/FlexFox80-backups/future.zip"
expect_visible "KiCad/Gerber/future-manufacturing.zip"
expect_visible "KiCad/future-symbols.lib"

expect_eol "lf" "README.md"
expect_eol "lf" "Software/Huzzah/ARDF_Transmitter/ARDF_Transmitter.ino"
expect_eol "lf" "KiCad/FlexFox80.kicad_pro"
expect_eol "crlf" "Software/AVR128DA48/FlexFox80/FlexFox80.cppproj"

if
	tracked_conflicts=$(git ls-files -ci --exclude-standard | rg '(^|/)KiCad/Gerber/.*\.zip$|\.lib$' || true)
	[ -n "$tracked_conflicts" ]
then
	printf '%s\n' "ERROR: intentional KiCad assets match ignore rules:" >&2
	printf '%s\n' "$tracked_conflicts" >&2
	failure=1
fi

tracked_generated=$(git ls-files -ci --exclude-standard | wc -l | tr -d ' ')
printf '%s\n' "Tracked files matching generated/IDE ignore rules: $tracked_generated (cleanup deferred to Step A2)"

jq empty KiCad/FlexFox80.kicad_prl KiCad/FlexFox80.kicad_pro
git diff --check
git diff --cached --check

if [ "$failure" -ne 0 ]; then
	exit 1
fi
