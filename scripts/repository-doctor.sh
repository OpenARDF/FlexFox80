#!/bin/sh

set -eu

repo_root=$(git rev-parse --show-toplevel)
cd "$repo_root"

required_tools="git rg jq node"
optional_tools="just gitleaks shellcheck shfmt"
missing=0

printf '%s\n' "Repository: $repo_root"
printf '%s\n' "Branch: $(git branch --show-current)"

for tool in $required_tools; do
	if command -v "$tool" >/dev/null 2>&1; then
		printf '%-12s %s\n' "$tool" "available"
	else
		printf '%-12s %s\n' "$tool" "MISSING (required for current checks)"
		missing=1
	fi
done

for tool in $optional_tools; do
	if command -v "$tool" >/dev/null 2>&1; then
		printf '%-12s %s\n' "$tool" "available"
	else
		printf '%-12s %s\n' "$tool" "not installed (optional at Step A1)"
	fi
done

if [ "$missing" -ne 0 ]; then
	exit 1
fi

printf '%s\n' "AVR build: deferred until Step A2 pins the compiler and device pack"
printf '%s\n' "ESP build: deferred until Step A2 pins the board core and libraries"
