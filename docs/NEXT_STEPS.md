<!-- SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Next steps

Last updated: 2026-10-30

## Current state

- Local build passes: `make test test-sdl sanitize headless debug release`.
- CHIP-8 scheduler, SDL input/video/audio unit tests and headless tracing are implemented.
- Apache-2.0 licensing, asset metadata, dependency policy and community documents are present.
- GitHub Actions passes on Linux, macOS and Windows/MSYS2.
- Timendus CHIP-8 Test Suite v4.2: Corax+ (`modern`) plus flags, quirks and keypad checks (`modern`, `vip`, `chip48`) passed manual verification on 2026-10-29 and 2026-10-30.
- The suite is not vendored because it is GPL-3.0; execute it from an independently downloaded copy using `make conformance`.

## Before release 0.1.0

1. Manually verify SDL integration: close/Escape, Space pause, Ctrl+R reset, N step, F11 fullscreen, resize/letterboxing and audio.
2. Follow [RELEASE.md](RELEASE.md): build release artifacts, generate SHA-256 checksums, create annotated tag `v0.1.0`, then publish release notes and artifacts.

## Known limitations

- External ROM compatibility is only partially verified; do not claim general CHIP-8 compatibility.
- Full SDL event-loop and real-device audio integration tests are still manual.
- SCHIP, XO-CHIP, persistent configuration, keyboard remapping and Game Boy are out of scope for 0.1.0.
