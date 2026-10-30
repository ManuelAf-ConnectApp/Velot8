<!-- SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Community roadmap

This roadmap is a public backlog, not a promise or release schedule. Work moves
from idea to merge only when its scope, ownership, tests and compatibility impact
are clear.

## Contribution lifecycle

```text
Discussion → Evolution proposal → Needs design → Ready → In progress → Review → Done
```

Use GitHub Discussions for broad ideas and trade-offs. Create an evolution
proposal only after the problem can be described with explicit non-goals and
acceptance criteria. Maintainers apply labels, split oversized work and mark an
issue `good first issue` or `help wanted` only when it is ready to implement.

Do not assign an issue to someone without their confirmation. A contributor
should comment before starting a substantial change so duplicated work can be
avoided.

## Initial backlog

### Ready for contributors

- Extend deterministic opcode and invalid-ROM regression coverage.
- Improve contributor documentation and the independently obtained ROM workflow.
- Add reproducible diagnostic output for framebuffer-based bug reports.

### Needs design

- Persistent configuration and keyboard remapping without coupling the core to
  host configuration APIs.
- An interactive debugger and trace viewer that preserves release hot-loop
  performance.
- Reproducible packaging for Linux, macOS and Windows, including SDL licensing
  and dependency handling.

### Later, separate tracks

- SCHIP support.
- XO-CHIP support.
- A separate Game Boy core, only after CHIP-8 work is stable and scoped as its
  own project track.

## Non-negotiable contribution rules

- Keep the emulator core independent of SDL2 and host services.
- Add deterministic tests for behavioral changes and run the checks in
  [CONTRIBUTING.md](../CONTRIBUTING.md).
- Do not contribute commercial ROMs, BIOS files or assets without a documented
  compatible license.
- Do not present partial test results as broad compatibility claims.

## Maintainer triage

Every accepted evolution proposal should identify its primary area (`core`,
`sdl`, `audio`, `testing`, `tooling`, `docs` or `community`), its release
target if one exists, and its test plan. Pull requests require green CI and a
maintainer review before merging to `main`.
