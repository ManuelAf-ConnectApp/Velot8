<!-- SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Compatibility matrix

Velot8 currently targets baseline CHIP-8 only. It does not claim compatibility with every CHIP-8 program, SCHIP or XO-CHIP. Results must be recorded only after a named ROM version has been run and visually verified.

## Built-in deterministic coverage

`make test` covers the implemented baseline opcode families, loader limits, stack limits, keypad behavior, drawing modes and quirk profiles. `make test-sdl` validates the frontend's 16-key mapping. These are the only checks currently marked as passing.

| Profile | Core unit tests | SDL input test | External ROM suite |
|---|---:|---:|---:|
| `modern` | Pass | Pass | Partial pass (Corax+) |
| `vip` | Pass (quirk coverage) | Pass | Not run |
| `chip48` | Pass (quirk coverage) | Pass | Not run |

## External conformance suite

The recommended external suite is [Timendus CHIP-8 test suite](https://github.com/Timendus/chip8-test-suite), release v4.2 (commit `cb24d55`), by Timendus and contributors. It is GPL-3.0-only and includes visual/manual checks for opcodes, flags, quirks and keypad behavior.

The suite is **not bundled, redistributed or modified** by Velot8. Download it independently from its upstream release and retain its `LICENSE`. This keeps Velot8 release artifacts limited to Apache-2.0 project material while allowing maintainers to use the suite for development.

Run an individual downloaded ROM with:

```sh
make conformance SUITE_DIR=/path/to/chip8-test-suite ROM=bin/3-corax+.ch8 PROFILE=modern
```

Useful baseline candidates are `bin/3-corax+.ch8`, `bin/4-flags.ch8`, `bin/5-quirks.ch8`, and `bin/6-keypad.ch8`. Record the exact suite version, profile, command, observed screen result and date in a pull request before changing any `Not run` status above.

## Observed external results

| Suite version | ROM | Profile | Result | Verification | Date |
|---|---|---|---|---|---|
| Timendus v4.2 | `bin/3-corax+.ch8` | `modern` | Pass | Manual visual check; no anomaly reported | 2026-10-29 |

## External smoke checks

On 2026-10-30, the Timendus suite checkout `742e9ea` was cloned outside this
repository and not vendored. Each of `bin/4-flags.ch8`, `bin/5-quirks.ch8` and
`bin/6-keypad.ch8` ran headlessly for 100,000 instructions with the `modern`,
`vip` and `chip48` profiles. Every run completed without an emulation error and
updated the framebuffer.

This only checks that the ROMs execute safely; it is not a conformance result.
The visible screen outcomes and keypad interactions still need manual
verification before changing the compatibility table above.

## Known gaps

- External ROM validation is incomplete: only Corax+ has been recorded so far, for
  the `modern` profile.
- SCHIP/XO-CHIP tests are out of scope.
- Visual, audio and timing outcomes need manual or future automated integration verification.
