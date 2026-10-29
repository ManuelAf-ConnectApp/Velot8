/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include "input.h"

void velot8_input_handle_key(Velot8Emulator *emulator, SDL_Keycode key, bool pressed) {
    static const SDL_Keycode host_keys[16] = {
        SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
        SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v
    };
    uint8_t chip8_key;

    for (chip8_key = 0; chip8_key < 16U; ++chip8_key) {
        if (key == host_keys[chip8_key]) {
            velot8_set_key(emulator, chip8_key, pressed);
            return;
        }
    }
}

void velot8_input_release_all(Velot8Emulator *emulator) {
    uint8_t key;
    for (key = 0; key < 16U; ++key) {
        velot8_set_key(emulator, key, false);
    }
}
