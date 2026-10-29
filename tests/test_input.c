/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>

#include "input.h"

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        ++failures; \
    } \
} while (0)

int main(void) {
    static const SDL_Keycode host_keys[16] = {
        SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
        SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v
    };
    Velot8Emulator emulator;
    uint8_t key;

    velot8_init(&emulator);
    for (key = 0; key < 16U; ++key) {
        velot8_input_handle_key(&emulator, host_keys[key], true);
        CHECK(emulator.keys == (uint16_t)(1U << key));
        velot8_input_handle_key(&emulator, host_keys[key], false);
        CHECK(emulator.keys == 0U);
    }
    velot8_input_handle_key(&emulator, SDLK_ESCAPE, true);
    CHECK(emulator.keys == 0U);
    velot8_set_key(&emulator, 2U, true);
    velot8_set_key(&emulator, 14U, true);
    velot8_input_release_all(&emulator);
    CHECK(emulator.keys == 0U);
    if (failures != 0) {
        return 1;
    }
    (void)puts("SDL input tests passed");
    return 0;
}
