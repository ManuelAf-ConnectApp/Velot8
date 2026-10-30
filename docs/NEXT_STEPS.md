<!-- SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Next steps

Last updated: 2026-10-29

## Current state

- Local build passes: `make test test-sdl sanitize headless debug release`.
- CHIP-8 scheduler, SDL input/video/audio unit tests and headless tracing are implemented.
- Apache-2.0 licensing, asset metadata, dependency policy and community documents are present.
- Timendus CHIP-8 Test Suite v4.2: `bin/3-corax+.ch8` passed a manual visual check with the `modern` profile on 2026-10-29.
- The suite is not vendored because it is GPL-3.0; execute it from an independently downloaded copy using `make conformance`.

## Before release 0.1.0

1. Run and record external results in [COMPATIBILITY.md](COMPATIBILITY.md):
   - `bin/4-flags.ch8`
   - `bin/5-quirks.ch8`
   - `bin/6-keypad.ch8`
   - Relevant runs with `modern`, `vip` and `chip48` profiles.
2. Manually verify SDL integration: close/Escape, Space pause, Ctrl+R reset, N step, F11 fullscreen, resize/letterboxing and audio.
3. Commit and push all local changes made after the initial candidate commit.
4. Confirm GitHub Actions is green on Linux, macOS and Windows/MSYS2.
5. Follow [RELEASE.md](RELEASE.md): build release artifacts, generate SHA-256 checksums, create annotated tag `v0.1.0`, then publish release notes and artifacts.

## Known limitations

- External ROM compatibility is only partially verified; do not claim general CHIP-8 compatibility.
- Full SDL event-loop and real-device audio integration tests are still manual.
- SCHIP, XO-CHIP, persistent configuration, keyboard remapping and Game Boy are out of scope for 0.1.0.
