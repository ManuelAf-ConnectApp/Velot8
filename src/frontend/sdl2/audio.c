/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include "audio.h"

#include <stdio.h>
#include <string.h>

enum {
    VELOT8_AUDIO_FREQUENCY = 48000,
    VELOT8_AUDIO_CHUNK_SAMPLES = 480,
    VELOT8_BUZZER_FREQUENCY = 440
};

void velot8_audio_init(Velot8Audio *audio) {
    SDL_AudioSpec desired;
    SDL_AudioSpec obtained;
    (void)memset(audio, 0, sizeof(*audio));
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        (void)fprintf(stderr, "SDL audio disabled: %s\n", SDL_GetError());
        return;
    }
    (void)memset(&desired, 0, sizeof(desired));
    desired.freq = VELOT8_AUDIO_FREQUENCY;
    desired.format = AUDIO_S16SYS;
    desired.channels = 1U;
    desired.samples = VELOT8_AUDIO_CHUNK_SAMPLES;
    audio->device = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
    if (audio->device == 0U || obtained.format != AUDIO_S16SYS || obtained.channels != 1U ||
        obtained.freq <= 0) {
        (void)fprintf(stderr, "SDL audio disabled: %s\n", SDL_GetError());
        if (audio->device != 0U) {
            SDL_CloseAudioDevice(audio->device);
        }
        audio->device = 0U;
        return;
    }
    audio->sample_rate = (uint32_t)obtained.freq;
    audio->enabled = true;
    SDL_PauseAudioDevice(audio->device, 0);
}

size_t velot8_audio_generate_square_wave(int16_t *samples, size_t sample_count,
                                         uint32_t sample_rate, uint32_t *phase) {
    size_t index;
    if (samples == NULL || phase == NULL || sample_rate == 0U) {
        return 0U;
    }
    *phase %= sample_rate;
    for (index = 0; index < sample_count; ++index) {
        samples[index] = *phase < sample_rate / 2U ? 7000 : -7000;
        *phase = (uint32_t)(((uint64_t)*phase + VELOT8_BUZZER_FREQUENCY) % sample_rate);
    }
    return sample_count;
}

void velot8_audio_update(Velot8Audio *audio, const Velot8Emulator *emulator) {
    int16_t samples[VELOT8_AUDIO_CHUNK_SAMPLES];
    if (!audio->enabled) {
        return;
    }
    if (emulator->sound_timer == 0U) {
        SDL_ClearQueuedAudio(audio->device);
        return;
    }
    if (SDL_GetQueuedAudioSize(audio->device) >= sizeof(samples)) {
        return;
    }
    (void)velot8_audio_generate_square_wave(samples, VELOT8_AUDIO_CHUNK_SAMPLES,
                                            audio->sample_rate, &audio->phase);
    if (SDL_QueueAudio(audio->device, samples, (Uint32)sizeof(samples)) != 0) {
        (void)fprintf(stderr, "SDL audio queue: %s\n", SDL_GetError());
        audio->enabled = false;
    }
}

void velot8_audio_destroy(Velot8Audio *audio) {
    if (audio->device != 0U) {
        SDL_CloseAudioDevice(audio->device);
    }
    audio->device = 0U;
    audio->enabled = false;
}
