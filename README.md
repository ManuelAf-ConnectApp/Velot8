<!-- SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Velot8

Velot8 is an educational CHIP-8 emulator written in C11. This initial iteration provides a core independent of SDL2, a visible SDL2 frontend, and an original CHIP-8 demo ROM. It is not a complete CHIP-8 implementation yet.

## Status and scope

The baseline CHIP-8 instruction families are implemented: control flow, skips, arithmetic, random, drawing, keypad, timers, BCD and register-memory transfer. The screen is 64×32, programs load at `0x200`, sprite drawing wraps by default, and timers can be advanced independently at 60 Hz. The frontend schedules instructions at 700 Hz and timers at 60 Hz; rendering does not control emulation speed.

The MVP deliberately excludes SCHIP/XO-CHIP, control remapping, ROM compatibility claims, rewind, netplay, shaders and Game Boy emulation. The frontend provides a simple `sound_timer` buzzer. Quirk profiles are selectable from the command line.

## Build and run

The core test and headless binary have no SDL2 dependency:

```sh
make test
make headless
build/headless/velot8-headless --demo
```

The graphical executable needs SDL2 plus `pkg-config`:

```sh
make run                 # builds Debug and opens the original animated demo
make release
build/release/velot8 --rom path/to/program.ch8 --scale 8 --cpu-hz 700
build/headless/velot8-headless --demo --profile vip --trace --steps 20
```

On macOS with Homebrew: `brew install sdl2 pkg-config`. On Debian/Ubuntu, install the distribution SDL2 development package and `pkg-config`; its exact package name may vary. For Windows use an MSYS2/UCRT64 environment with SDL2 and pkg-config installed, then invoke `make`. The CI workflow is configured to build the SDL2 frontend and run tests on Linux, macOS and Windows/MSYS2; its remote runs must be reviewed on GitHub after publishing.

Without arguments, the graphical application opens a `NO ROM` window; `--demo` runs the included real CHIP-8 program, which moves a five-row sprite. No external or commercial ROM is included.

`--profile modern` is the default; `vip` enables the historical shift-source and `I` increment behavior, while `chip48` selects the CHIP-48 B-jump and clipped-drawing behavior. `--trace` writes the PC, opcode, I and V registers before every instruction; use it with `--headless` for reproducible debugging output.

`--cpu-hz` sets the instruction scheduler from 1 to 5000 Hz (default 700); this never changes the independent 60 Hz CHIP-8 timers. The frontend generates a simple 440 Hz square-wave buzzer while `sound_timer` is non-zero. If the host audio device is unavailable, the emulator logs the condition and continues without sound.

`--steps` limits a headless run to 1–100,000,000 instructions (default 120), making traces and regression runs reproducible.

`--version` prints the current release version (`0.1.0`). The release checklist and checksum procedure are in [docs/RELEASE.md](docs/RELEASE.md).

The maintained handoff checklist is [docs/NEXT_STEPS.md](docs/NEXT_STEPS.md).

The SDL-free scheduler bounds host-time catch-up to 100 ms, advances instructions at `--cpu-hz`, and ticks CHIP-8 timers independently at 60 Hz. Pausing the frontend stops calls into this scheduler, so emulated time does not advance while paused.

## Controls

`Escape` exits, `Space` pauses, `N` advances one instruction while paused, `Ctrl+R` resets the loaded ROM, and `F11` toggles fullscreen. CHIP-8 input is mapped as follows:

```text
1 2 3 4     -> 1 2 3 C
Q W E R     -> 4 5 6 D
A S D F     -> 7 8 9 E
Z X C V     -> A 0 B F
```

`R` without Ctrl is therefore the CHIP-8 `D` key; the frontend prioritizes that mapping. On focus loss, all keys are released. The display maintains 2:1 aspect ratio and integer pixel scaling while resized.

## Testing

`make test` checks reset, ROM-loader boundaries, arithmetic, calls/returns, skips, keypad behavior, BCD, register-memory transfer, drawing/collision modes, quirks, scheduler behavior and error boundaries. `make test-sdl` validates all 16 SDL keyboard mappings, framebuffer-to-RGBA conversion and the audio waveform generator without opening a window or audio device. `make sanitize` runs the core tests with AddressSanitizer and UndefinedBehaviorSanitizer on compatible GCC/Clang toolchains. The CI workflow is configured to build Debug/Release SDL2 frontends and run core, SDL-input/video/audio, sanitizer and headless-demo checks on Linux, macOS and Windows/MSYS2. Full SDL event-loop and real-device audio integration tests remain planned.

The external compatibility procedure and its intentionally conservative result matrix live in [docs/COMPATIBILITY.md](docs/COMPATIBILITY.md). External GPL-licensed test ROMs are never bundled with Velot8.

## Architecture

`include/velot8/` is the SDL-free public API. `src/core/` owns CHIP-8 state and execution. `src/frontend/sdl2/` converts its monochrome framebuffer into a persistent RGBA8888 texture and owns wall-clock scheduling, events and window resources. See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## License and dependencies

Velot8 is licensed under the [Apache License 2.0](LICENSE). [NOTICE](NOTICE) identifies the project copyright holder. The project contains no third-party ROMs or bundled third-party code. [DEPENDENCIES.md](DEPENDENCIES.md) and [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) document SDL2 and the policy for any future bundled dependency. The original project image is licensed Apache-2.0 through [its sidecar metadata](images/Velot8.png.license).
