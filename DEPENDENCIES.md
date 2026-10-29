<!-- SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Dependencies

Velot8 has one runtime/build dependency for its graphical frontend:

| Dependency | Scope | License | Bundled? |
|---|---|---|---|
| SDL2 | Windowing, input, rendering and optional audio | zlib | No |

The headless executable and core tests do not require SDL2. The Makefile resolves SDL2 through `pkg-config`; Velot8 does not copy, modify or redistribute SDL2 source or binaries.

## Distribution policy

If a release package bundles SDL2 or another third-party component, maintainers must include that component's exact license text and required notices in the release package, record its version and source URL here, and update `THIRD_PARTY_NOTICES.md` before publishing.
