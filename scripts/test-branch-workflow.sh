#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
test_root="$(mktemp -d "${TMPDIR:-/tmp}/flexfox80-branch-workflow.XXXXXX")"
trap 'rm -rf "$test_root"' EXIT

expect_failure() {
	if "$@" >/dev/null 2>&1; then
		printf 'ERROR: command unexpectedly passed:' >&2
		printf ' %q' "$@" >&2
		printf '\n' >&2
		exit 1
	fi
}

test_repo="$test_root/repository"
git init -q -b main "$test_repo"
git -C "$test_repo" config user.name "FlexFox80 Branch Test"
git -C "$test_repo" config user.email "branch-test@example.invalid"
mkdir -p "$test_repo/scripts"
cp "$repo_root/scripts/check-branch-workflow.sh" "$test_repo/scripts/"

printf 'baseline\n' >"$test_repo/README.md"
git -C "$test_repo" add README.md scripts/check-branch-workflow.sh
git -C "$test_repo" commit -q -m "Create baseline"
git -C "$test_repo" branch Development1
git -C "$test_repo" update-ref refs/remotes/origin/main main
git -C "$test_repo" update-ref refs/remotes/origin/Development1 Development1

git -C "$test_repo" switch -q Development1
"$test_repo/scripts/check-branch-workflow.sh" Development1 --promotion >/dev/null
git -C "$test_repo" switch -q -c codex/topic
"$test_repo/scripts/check-branch-workflow.sh" Development1 >/dev/null

git -C "$test_repo" switch -q main
printf 'new main baseline\n' >>"$test_repo/README.md"
git -C "$test_repo" add README.md
git -C "$test_repo" commit -q -m "Advance main"
git -C "$test_repo" update-ref refs/remotes/origin/main main

git -C "$test_repo" switch -q Development1
expect_failure "$test_repo/scripts/check-branch-workflow.sh" Development1
expect_failure "$test_repo/scripts/check-branch-workflow.sh" Development1 --promotion
git -C "$test_repo" switch -q -c codex/sync-development1
expect_failure "$test_repo/scripts/check-branch-workflow.sh" Development1 --sync
git -C "$test_repo" merge -q --no-ff --no-edit origin/main
"$test_repo/scripts/check-branch-workflow.sh" Development1 --sync >/dev/null

# Extra commits after the exact collector/main merge must not pass as a sync.
git -C "$test_repo" commit -q --allow-empty -m "Unexpected extra commit"
expect_failure "$test_repo/scripts/check-branch-workflow.sh" Development1 --sync

printf 'PASS branch workflow regression\n'
