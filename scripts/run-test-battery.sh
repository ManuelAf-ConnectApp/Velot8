#!/bin/sh
# SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde
# SPDX-License-Identifier: Apache-2.0

# Run the automated Velot8 release-test battery.
#
# Optional environment variables:
#   SUITE_DIR=/path/to/Timendus/chip8-test-suite
#   STEPS=100000
#
# The external suite is deliberately not downloaded or vendored by this script.
# Its visual and keypad outcomes remain manual conformance checks.

set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
PROJECT_DIR=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
STEPS=${STEPS:-100000}

case "$STEPS" in
    ''|*[!0-9]*)
        printf '%s\n' 'STEPS must be a positive integer.' >&2
        exit 2
        ;;
esac

if [ "$STEPS" -eq 0 ]; then
    printf '%s\n' 'STEPS must be a positive integer.' >&2
    exit 2
fi

cd "$PROJECT_DIR"

printf '%s\n' '== Local build and automated tests =='
make test test-sdl sanitize headless debug release

printf '%s\n' '== Headless demo =='
./build/headless/velot8-headless --demo

if [ -z "${SUITE_DIR:-}" ]; then
    printf '%s\n' '== External ROM smoke checks skipped =='
    printf '%s\n' 'Set SUITE_DIR to a separately downloaded Timendus chip8-test-suite checkout to run them.'
    exit 0
fi

if [ ! -d "$SUITE_DIR/bin" ]; then
    printf 'SUITE_DIR does not contain bin/: %s\n' "$SUITE_DIR" >&2
    exit 2
fi

printf '%s\n' '== External ROM smoke checks =='
for rom in 4-flags 5-quirks 6-keypad; do
    if [ ! -f "$SUITE_DIR/bin/$rom.ch8" ]; then
        printf 'Missing ROM: %s\n' "$SUITE_DIR/bin/$rom.ch8" >&2
        exit 2
    fi
    for profile in modern vip chip48; do
        printf 'Running %s with %s...\n' "$rom" "$profile"
        ./build/headless/velot8-headless \
            --rom "$SUITE_DIR/bin/$rom.ch8" \
            --profile "$profile" \
            --steps "$STEPS"
    done
done

printf '%s\n' '== Manual conformance still required =='
printf '%s\n' 'Use make conformance SUITE_DIR=... ROM=bin/4-flags.ch8 PROFILE=modern'
printf '%s\n' 'and record the visible result plus keypad interactions in docs/COMPATIBILITY.md.'
