/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include "video.h"

#include <stdio.h>

int velot8_video_init(Velot8Video *video, SDL_Window *window) {
    video->renderer = SDL_CreateRenderer(window, -1,
                                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (video->renderer == NULL) {
        video->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (video->renderer == NULL) {
        (void)fprintf(stderr, "SDL renderer: %s\n", SDL_GetError());
        return -1;
    }
    video->texture = SDL_CreateTexture(video->renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_STREAMING, VELOT8_SCREEN_WIDTH,
                                       VELOT8_SCREEN_HEIGHT);
    if (video->texture == NULL) {
        (void)fprintf(stderr, "SDL texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(video->renderer);
        video->renderer = NULL;
        return -1;
    }
    return 0;
}

void velot8_video_destroy(Velot8Video *video) {
    if (video->texture != NULL) {
        SDL_DestroyTexture(video->texture);
    }
    if (video->renderer != NULL) {
        SDL_DestroyRenderer(video->renderer);
    }
    video->texture = NULL;
    video->renderer = NULL;
}

void velot8_video_convert_framebuffer(const Velot8Emulator *emulator,
                                      uint32_t *pixels, size_t pixel_count) {
    size_t pixel;
    if (emulator == NULL || pixels == NULL ||
        pixel_count < VELOT8_SCREEN_WIDTH * VELOT8_SCREEN_HEIGHT) {
        return;
    }
    for (pixel = 0; pixel < VELOT8_SCREEN_WIDTH * VELOT8_SCREEN_HEIGHT; ++pixel) {
        pixels[pixel] = emulator->framebuffer[pixel] != 0U ? 0x44E3F5FFU : 0x101827FFU;
    }
}

void velot8_video_render(Velot8Video *video, const Velot8Emulator *emulator) {
    uint32_t pixels[VELOT8_SCREEN_WIDTH * VELOT8_SCREEN_HEIGHT];
    SDL_Rect destination;
    int width;
    int height;
    int scale_x;
    int scale_y;
    int scale;
    velot8_video_convert_framebuffer(emulator, pixels,
                                     sizeof(pixels) / sizeof(pixels[0]));
    (void)SDL_UpdateTexture(video->texture, NULL, pixels,
                            VELOT8_SCREEN_WIDTH * (int)sizeof(pixels[0]));
    SDL_GetRendererOutputSize(video->renderer, &width, &height);
    scale_x = width / VELOT8_SCREEN_WIDTH;
    scale_y = height / VELOT8_SCREEN_HEIGHT;
    scale = scale_x < scale_y ? scale_x : scale_y;
    if (scale < 1) {
        scale = 1;
    }
    destination.w = VELOT8_SCREEN_WIDTH * scale;
    destination.h = VELOT8_SCREEN_HEIGHT * scale;
    destination.x = (width - destination.w) / 2;
    destination.y = (height - destination.h) / 2;
    (void)SDL_SetRenderDrawColor(video->renderer, 0x10U, 0x18U, 0x27U, 0xFFU);
    (void)SDL_RenderClear(video->renderer);
    (void)SDL_RenderCopy(video->renderer, video->texture, NULL, &destination);
    SDL_RenderPresent(video->renderer);
}
