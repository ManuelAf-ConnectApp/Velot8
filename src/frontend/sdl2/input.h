/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef VELOT8_SDL_INPUT_H
#define VELOT8_SDL_INPUT_H

#include <SDL.h>

#include "velot8/emulator.h"

void velot8_input_handle_key(Velot8Emulator *emulator, SDL_Keycode key, bool pressed);
void velot8_input_release_all(Velot8Emulator *emulator);

#endif
