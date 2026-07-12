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

# Run all currently available non-firmware checks.
check: doctor docs-check policy-check

# Scan repository history using the exact-fingerprint false-positive baseline.
secrets:
    gitleaks detect --source . --no-banner --redact

# Build the AVR Release firmware with the pinned compiler/device pack.
avr-build:
    node ./scripts/build-avr-release.mjs

# Step A2 will replace this explicit boundary with a pinned ESP8266 build.
esp-build:
    @echo "ESP8266 build wrapper is not configured yet; complete Step A2 before firmware changes."
    @exit 2
