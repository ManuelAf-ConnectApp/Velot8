/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include "app.h"

#include <SDL.h>
#include <stdio.h>

#include "input.h"
#include "audio.h"
#include "video.h"
#include "velot8/scheduler.h"

static void set_title(SDL_Window *window, const char *name, bool paused) {
    char title[256];
    (void)SDL_snprintf(title, sizeof(title), "Velot8 — CHIP-8 Emulator%s — %s",
                       paused ? " [PAUSED]" : "", name != NULL ? name : "NO ROM");
    SDL_SetWindowTitle(window, title);
}

int velot8_sdl_run(Velot8Emulator *emulator, const char *rom_name, int scale,
                   unsigned cpu_hz) {
    SDL_Window *window = NULL;
    Velot8Video video = {0};
    Velot8Audio audio = {0};
    uint64_t last_counter;
    Velot8Scheduler scheduler;
    bool running = true;
    bool paused = false;
    int result = 1;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        (void)fprintf(stderr, "SDL initialization: %s\n", SDL_GetError());
        return 1;
    }
    window = SDL_CreateWindow("Velot8 — CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, VELOT8_SCREEN_WIDTH * scale,
                              VELOT8_SCREEN_HEIGHT * scale, SDL_WINDOW_RESIZABLE);
    if (window == NULL || velot8_video_init(&video, window) != 0) {
        (void)fprintf(stderr, "SDL window: %s\n", SDL_GetError());
        goto cleanup;
    }
    set_title(window, rom_name, paused);
    velot8_audio_init(&audio);
    velot8_scheduler_init(&scheduler, cpu_hz);
    last_counter = SDL_GetPerformanceCounter();
    while (running) {
        SDL_Event event;
        uint64_t current_counter = SDL_GetPerformanceCounter();
        double delta = (double)(current_counter - last_counter) /
                       (double)SDL_GetPerformanceFrequency();
        last_counter = current_counter;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                } else if (event.key.keysym.sym == SDLK_SPACE) {
                    paused = !paused;
                    set_title(window, rom_name, paused);
                } else if (event.key.keysym.sym == SDLK_r &&
                           (event.key.keysym.mod & KMOD_CTRL) != 0) {
                    velot8_reset(emulator);
                    set_title(window, rom_name, paused);
                } else if (event.key.keysym.sym == SDLK_n && paused) {
                    (void)velot8_step(emulator);
                } else if (event.key.keysym.sym == SDLK_F11) {
                    Uint32 flags = SDL_GetWindowFlags(window);
                    SDL_SetWindowFullscreen(window,
                                            (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0U ? 0U :
                                            SDL_WINDOW_FULLSCREEN_DESKTOP);
                } else {
                    velot8_input_handle_key(emulator, event.key.keysym.sym, true);
                }
            } else if (event.type == SDL_KEYUP) {
                velot8_input_handle_key(emulator, event.key.keysym.sym, false);
            } else if (event.type == SDL_WINDOWEVENT &&
                       event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                velot8_input_release_all(emulator);
            }
        }
        if (!paused && rom_name != NULL) {
            if (velot8_scheduler_advance(&scheduler, emulator, delta, NULL) != VELOT8_OK) {
                (void)fprintf(stderr, "Emulation stopped: %s\n",
                              velot8_error_string(emulator->last_error));
                paused = true;
                set_title(window, rom_name, paused);
            }
        }
        velot8_audio_update(&audio, emulator);
        velot8_video_render(&video, emulator);
        SDL_Delay(1U);
    }
    result = 0;
cleanup:
    velot8_audio_destroy(&audio);
    velot8_video_destroy(&video);
    if (window != NULL) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
    return result;
}
