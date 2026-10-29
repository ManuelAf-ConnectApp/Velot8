/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef VELOT8_SDL_VIDEO_H
#define VELOT8_SDL_VIDEO_H

#include <SDL.h>

#include "velot8/emulator.h"

typedef struct Velot8Video {
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Velot8Video;

int velot8_video_init(Velot8Video *video, SDL_Window *window);
void velot8_video_destroy(Velot8Video *video);
void velot8_video_convert_framebuffer(const Velot8Emulator *emulator,
                                      uint32_t *pixels, size_t pixel_count);
void velot8_video_render(Velot8Video *video, const Velot8Emulator *emulator);

#endif
