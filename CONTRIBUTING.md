<!-- SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Contributing to Velot8

Velot8 is an educational C11 CHIP-8 emulator. Keep the core independent of SDL2 and host services; platform integration belongs in `src/frontend/`.

Before proposing a change, run:

```sh
make test
make sanitize
make headless
make test-sdl
build/headless/velot8-headless --demo
```

Use fixed-width integer types, validate untrusted ROM input and add a deterministic test for every corrected behavior. Do not add commercial ROMs, BIOS files, copyrighted game assets or third-party code without a documented compatible license.

By submitting a contribution, you agree that it is made available under the Apache License 2.0. Do not submit code or assets unless you have the right to license them under those terms.

For security-sensitive reports, follow [SECURITY.md](SECURITY.md) rather than opening a public issue.
