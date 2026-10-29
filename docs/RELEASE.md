<!-- SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Release procedure

## Version 0.1.0

Velot8 0.1.0 is the first CHIP-8 MVP candidate. It includes the SDL2 frontend, headless mode, original demo ROM, baseline opcode support, configurable quirks, scheduler, optional buzzer and deterministic core/SDL tests.

## Pre-release checklist

1. Run `make clean && make test test-sdl sanitize headless debug release`.
2. Run the graphical demo and verify close, Escape, pause, Ctrl+R, N, F11, resize and audio on a supported host.
3. Run external conformance ROMs following [COMPATIBILITY.md](COMPATIBILITY.md), then record only observed results.
4. Confirm CI is green on Linux, macOS and Windows/MSYS2.
5. Confirm `LICENSE`, `NOTICE`, `DEPENDENCIES.md`, `THIRD_PARTY_NOTICES.md` and every asset's license metadata are current.
6. Build release artifacts per supported platform. If any artifact bundles SDL2, include its exact license/notice under the policy in [DEPENDENCIES.md](../DEPENDENCIES.md).
7. Generate checksums alongside each artifact, for example:

   ```sh
   shasum -a 256 build/release/velot8 > velot8-0.1.0-macos-arm64.sha256
   ```

8. Commit the release changes, create annotated tag `v0.1.0`, push it, and create the GitHub release with the matching notes and checksums.

Do not publish a release until all applicable checklist items have evidence.
