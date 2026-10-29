/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef VELOT8_SDL_APP_H
#define VELOT8_SDL_APP_H

#include "velot8/emulator.h"

int velot8_sdl_run(Velot8Emulator *emulator, const char *rom_name, int scale,
                   unsigned cpu_hz);

#endif
