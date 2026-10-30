# SPDX-FileCopyrightText: 2026 Manuel María Alconchel Fernánde
# SPDX-License-Identifier: Apache-2.0

CC ?= cc
CPPFLAGS := -Iinclude -Isrc/demo -Isrc/frontend/sdl2
CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic -MMD -MP
LDFLAGS :=
LDLIBS :=
BUILD_DIR := build
CORE_SOURCES := src/core/cpu.c src/core/emulator.c src/core/scheduler.c
DEMO_SOURCES := src/demo/demo_rom.c
SDL_SOURCES := src/frontend/sdl2/app.c src/frontend/sdl2/audio.c src/frontend/sdl2/video.c src/frontend/sdl2/input.c
SDL_CFLAGS := $(shell pkg-config --cflags sdl2 2>/dev/null)
SDL_LIBS := $(shell pkg-config --libs sdl2 2>/dev/null)
HOST_OS := $(shell uname -s)

PROFILE ?= modern

.PHONY: all debug release headless test test-sdl conformance clean run run-rom format sanitize

all: debug

debug: CFLAGS += -O0 -g
debug: $(BUILD_DIR)/debug/velot8

release: CFLAGS += -O2
release: $(BUILD_DIR)/release/velot8

$(BUILD_DIR)/debug/velot8: src/main.c $(CORE_SOURCES) $(DEMO_SOURCES) $(SDL_SOURCES)
	@test -n "$(SDL_LIBS)" || { echo "SDL2 not found: install it and pkg-config (see README.md)." >&2; exit 1; }
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(SDL_CFLAGS) $(CFLAGS) $^ $(LDFLAGS) $(SDL_LIBS) -o $@

$(BUILD_DIR)/release/velot8: src/main.c $(CORE_SOURCES) $(DEMO_SOURCES) $(SDL_SOURCES)
	@test -n "$(SDL_LIBS)" || { echo "SDL2 not found: install it and pkg-config (see README.md)." >&2; exit 1; }
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(SDL_CFLAGS) $(CFLAGS) $^ $(LDFLAGS) $(SDL_LIBS) -o $@

headless: $(BUILD_DIR)/headless/velot8-headless

$(BUILD_DIR)/headless/velot8-headless: src/main.c $(CORE_SOURCES) $(DEMO_SOURCES)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -O0 -g -DVELOT8_HEADLESS $^ $(LDFLAGS) $(LDLIBS) -o $@

test: $(BUILD_DIR)/test/test_core
	$<

$(BUILD_DIR)/test/test_core: tests/test_core.c $(CORE_SOURCES)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -O0 -g $^ $(LDFLAGS) $(LDLIBS) -o $@

test-sdl: $(BUILD_DIR)/test/test_input $(BUILD_DIR)/test/test_video $(BUILD_DIR)/test/test_audio
	$(BUILD_DIR)/test/test_input
	$(BUILD_DIR)/test/test_video
	$(BUILD_DIR)/test/test_audio

$(BUILD_DIR)/test/test_input: tests/test_input.c src/frontend/sdl2/input.c $(CORE_SOURCES)
	@test -n "$(SDL_LIBS)" || { echo "SDL2 not found: install it and pkg-config (see README.md)." >&2; exit 1; }
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(SDL_CFLAGS) -Umain -DSDL_MAIN_HANDLED $(CFLAGS) -O0 -g $^ $(LDFLAGS) $(SDL_LIBS) -o $@

$(BUILD_DIR)/test/test_video: tests/test_video.c src/frontend/sdl2/video.c $(CORE_SOURCES)
	@test -n "$(SDL_LIBS)" || { echo "SDL2 not found: install it and pkg-config (see README.md)." >&2; exit 1; }
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(SDL_CFLAGS) -Umain -DSDL_MAIN_HANDLED $(CFLAGS) -O0 -g $^ $(LDFLAGS) $(SDL_LIBS) -o $@

$(BUILD_DIR)/test/test_audio: tests/test_audio.c src/frontend/sdl2/audio.c $(CORE_SOURCES)
	@test -n "$(SDL_LIBS)" || { echo "SDL2 not found: install it and pkg-config (see README.md)." >&2; exit 1; }
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(SDL_CFLAGS) -Umain -DSDL_MAIN_HANDLED $(CFLAGS) -O0 -g $^ $(LDFLAGS) $(SDL_LIBS) -o $@

run: debug
	$(BUILD_DIR)/debug/velot8 --demo

run-rom: debug
	@test -n "$(ROM)" || { echo "Usage: make run-rom ROM=path/to/program.ch8" >&2; exit 2; }
	$(BUILD_DIR)/debug/velot8 --rom "$(ROM)"

conformance: debug
	@test -n "$(SUITE_DIR)" || { echo "Usage: make conformance SUITE_DIR=/path/to/suite ROM=bin/3-corax+.ch8 [PROFILE=modern]" >&2; exit 2; }
	@test -n "$(ROM)" || { echo "ROM must be a path relative to SUITE_DIR." >&2; exit 2; }
	@test -f "$(SUITE_DIR)/$(ROM)" || { echo "ROM not found: $(SUITE_DIR)/$(ROM)" >&2; exit 2; }
	$(BUILD_DIR)/debug/velot8 --rom "$(SUITE_DIR)/$(ROM)" --profile "$(PROFILE)"

format:
	@command -v clang-format >/dev/null 2>&1 || { echo "clang-format is not installed" >&2; exit 1; }
	clang-format -i $(CORE_SOURCES) $(DEMO_SOURCES) $(SDL_SOURCES) src/main.c tests/test_core.c include/velot8/*.h src/demo/*.h src/frontend/sdl2/*.h

ifneq (,$(filter MSYS% MINGW%,$(HOST_OS)))
sanitize:
	@echo "Sanitizers are unavailable with the MSYS2 UCRT64 GCC package; skipping."
else
sanitize: $(BUILD_DIR)/sanitize/test_core
	$<

$(BUILD_DIR)/sanitize/test_core: tests/test_core.c $(CORE_SOURCES)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
	$^ $(LDFLAGS) -fsanitize=address,undefined $(LDLIBS) -o $@
endif

clean:
	rm -rf $(BUILD_DIR)
