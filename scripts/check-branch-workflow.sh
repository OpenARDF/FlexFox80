#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
collector="${1:-}"
mode="${2:-}"

if [[ "$collector" != "Development1" && "$collector" != "Development1-KiCad" ]]; then
	printf 'ERROR: collector must be Development1 or Development1-KiCad\n' >&2
	exit 1
fi
if [[ -n "$mode" && "$mode" != "--promotion" && "$mode" != "--sync" ]]; then
	printf 'ERROR: unsupported mode: %s\n' "$mode" >&2
	exit 1
fi

current_branch="$(git -C "$repo_root" branch --show-current)"
if [[ -z "$current_branch" ]]; then
	printf 'ERROR: branch routing cannot be checked from detached HEAD\n' >&2
	exit 1
fi

main_ref="origin/main"
collector_ref="origin/$collector"

# The Justfile fetches before invoking this script. Always validate against the
# fetched remote tips so a stale local collector cannot pass a routing gate.
for required_ref in "$main_ref" "$collector_ref"; do
	if git -C "$repo_root" show-ref --verify --quiet "refs/remotes/$required_ref"; then
		continue
	fi
	printf 'ERROR: fetched remote branch is unavailable locally: %s\n' "$required_ref" >&2
	exit 1
done

if [[ "$mode" == "--promotion" ]]; then
	if [[ "$current_branch" != "$collector" ]]; then
		printf 'ERROR: promotion preflight must run on %s, not %s\n' \
			"$collector" "$current_branch" >&2
		exit 1
	fi
	if [[ "$(git -C "$repo_root" rev-parse HEAD)" != "$(git -C "$repo_root" rev-parse "$collector_ref")" ]]; then
		printf 'ERROR: local %s does not match %s\n' "$collector" "$collector_ref" >&2
		exit 1
	fi
	if [[ -n "$(git -C "$repo_root" status --porcelain)" ]]; then
		printf 'ERROR: promotion preflight requires a clean worktree\n' >&2
		exit 1
	fi
	if ! git -C "$repo_root" merge-base --is-ancestor "$main_ref" HEAD; then
		printf 'ERROR: %s does not contain the fetched main baseline\n' "$collector" >&2
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
	done < <(git -C "$repo_root" diff --name-only "$main_ref"...HEAD)
elif [[ "$mode" == "--sync" ]]; then
	if [[ "$current_branch" == "$collector" || "$current_branch" == "main" ]]; then
		printf 'ERROR: synchronization must run on a focused topic branch, not %s\n' \
			"$current_branch" >&2
		exit 1
	fi
	collector_oid="$(git -C "$repo_root" rev-parse "$collector_ref")"
	main_oid="$(git -C "$repo_root" rev-parse "$main_ref")"
	read -r _ first_parent second_parent extra_parent < <(
		git -C "$repo_root" rev-list --parents -n 1 HEAD
	)
	# Exact parents make the synchronization auditable: no extra topic commits,
	# rebases, or squash-equivalent content can masquerade as baseline ancestry.
	if [[ -n "${extra_parent:-}" || "${first_parent:-}" != "$collector_oid" ||
		"${second_parent:-}" != "$main_oid" ]]; then
		printf 'ERROR: sync HEAD must merge %s first and %s second\n' \
			"$collector_ref" "$main_ref" >&2
		exit 1
	fi
	if ! git -C "$repo_root" diff --quiet || ! git -C "$repo_root" diff --cached --quiet; then
		printf 'ERROR: synchronization checkout must be clean\n' >&2
		exit 1
	fi
else
	if ! git -C "$repo_root" merge-base --is-ancestor "$main_ref" "$collector_ref"; then
		printf 'ERROR: %s does not contain the fetched main baseline; synchronize it before topic work\n' \
			"$collector_ref" >&2
		exit 1
	fi
	if [[ "$current_branch" == "$collector" ]]; then
		if [[ "$(git -C "$repo_root" rev-parse HEAD)" != "$(git -C "$repo_root" rev-parse "$collector_ref")" ]]; then
			printf 'ERROR: local %s does not match %s\n' "$collector" "$collector_ref" >&2
			exit 1
		fi
	elif ! git -C "$repo_root" merge-base --is-ancestor "$collector_ref" HEAD; then
		printf 'ERROR: %s is not based on %s\n' "$current_branch" "$collector" >&2
		exit 1
	fi
fi

printf 'PASS branch route: %s -> %s%s\n' "$current_branch" "$collector" \
	"${mode:+ (${mode#--})}"
