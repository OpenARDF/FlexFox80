#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
collector="${1:-}"
mode="${2:-}"

if [[ "$collector" != "Development1" && "$collector" != "Development1-KiCad" ]]; then
	printf 'ERROR: collector must be Development1 or Development1-KiCad\n' >&2
	exit 1
fi
if [[ -n "$mode" && "$mode" != "--promotion" ]]; then
	printf 'ERROR: unsupported mode: %s\n' "$mode" >&2
	exit 1
fi

current_branch="$(git -C "$repo_root" branch --show-current)"
if [[ -z "$current_branch" ]]; then
	printf 'ERROR: branch routing cannot be checked from detached HEAD\n' >&2
	exit 1
fi

if git -C "$repo_root" show-ref --verify --quiet "refs/heads/$collector"; then
	collector_ref="$collector"
elif git -C "$repo_root" show-ref --verify --quiet "refs/remotes/origin/$collector"; then
	collector_ref="origin/$collector"
else
	printf 'ERROR: collector branch is unavailable: %s\n' "$collector" >&2
	exit 1
fi

if [[ "$mode" == "--promotion" ]]; then
	if [[ "$current_branch" != "$collector" ]]; then
		printf 'ERROR: promotion preflight must run on %s, not %s\n' \
			"$collector" "$current_branch" >&2
		exit 1
	fi
	if [[ -n "$(git -C "$repo_root" status --porcelain)" ]]; then
		printf 'ERROR: promotion preflight requires a clean worktree\n' >&2
		exit 1
	fi
	if ! git -C "$repo_root" merge-base --is-ancestor main HEAD; then
		printf 'ERROR: %s does not contain the local main baseline\n' "$collector" >&2
		exit 1
	fi
	if git -C "$repo_root" show-ref --verify --quiet refs/remotes/origin/main &&
		[[ "$(git -C "$repo_root" rev-parse main)" != "$(git -C "$repo_root" rev-parse origin/main)" ]]; then
		printf 'ERROR: local main is not synchronized with origin/main\n' >&2
		exit 1
	fi

	while IFS= read -r path; do
		[[ -z "$path" ]] && continue
		if [[ "$collector" == "Development1" ]]; then
			if [[ "$path" == KiCad/* ]]; then
				printf 'ERROR: firmware/software collector contains KiCad path: %s\n' \
					"$path" >&2
				exit 1
			fi
		else
			case "$path" in
			Software/* | Tests/* | Docs/Software/Evidence/* | Docs/Software/Releases/* | Docs/Software/RELEASE_NOTES* | release-notes/* | release-evidence/*)
				printf 'ERROR: KiCad collector contains firmware, test, or release path: %s\n' \
					"$path" >&2
				exit 1
				;;
			esac
		fi
	done < <(git -C "$repo_root" diff --name-only main...HEAD)
else
	if [[ "$current_branch" != "$collector" ]] &&
		! git -C "$repo_root" merge-base --is-ancestor "$collector_ref" HEAD; then
		printf 'ERROR: %s is not based on %s\n' "$current_branch" "$collector" >&2
		exit 1
	fi
fi

printf 'PASS branch route: %s -> %s%s\n' "$current_branch" "$collector" \
	"${mode:+ (promotion)}"
