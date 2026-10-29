/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef VELOT8_SDL_AUDIO_H
#define VELOT8_SDL_AUDIO_H

#include <SDL.h>

#include "velot8/emulator.h"

typedef struct Velot8Audio {
    SDL_AudioDeviceID device;
    uint32_t sample_rate;
    uint32_t phase;
    bool enabled;
} Velot8Audio;

void velot8_audio_init(Velot8Audio *audio);
size_t velot8_audio_generate_square_wave(int16_t *samples, size_t sample_count,
                                         uint32_t sample_rate, uint32_t *phase);
void velot8_audio_update(Velot8Audio *audio, const Velot8Emulator *emulator);
void velot8_audio_destroy(Velot8Audio *audio);

#endif
