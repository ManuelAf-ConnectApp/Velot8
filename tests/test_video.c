/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>

#include "video.h"

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        ++failures; \
    } \
} while (0)

int main(void) {
    Velot8Emulator emulator;
    uint32_t pixels[VELOT8_SCREEN_WIDTH * VELOT8_SCREEN_HEIGHT];
    uint32_t untouched = 0xDEADBEEFU;

    velot8_init(&emulator);
    emulator.framebuffer[0] = 1U;
    emulator.framebuffer[VELOT8_SCREEN_WIDTH + 1U] = 1U;
    velot8_video_convert_framebuffer(&emulator, pixels,
                                     sizeof(pixels) / sizeof(pixels[0]));
    CHECK(pixels[0] == 0x44E3F5FFU);
    CHECK(pixels[1] == 0x101827FFU);
    CHECK(pixels[VELOT8_SCREEN_WIDTH + 1U] == 0x44E3F5FFU);
    velot8_video_convert_framebuffer(&emulator, &untouched, 1U);
    CHECK(untouched == 0xDEADBEEFU);
    if (failures != 0) {
        return 1;
    }
    (void)puts("SDL video tests passed");
    return 0;
}
