/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>

#include "audio.h"

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        ++failures; \
    } \
} while (0)

int main(void) {
    int16_t samples[5] = {0};
    uint32_t phase = 0U;

    CHECK(velot8_audio_generate_square_wave(samples, 5U, 1000U, &phase) == 5U);
    CHECK(samples[0] == 7000 && samples[1] == 7000);
    CHECK(samples[2] == -7000 && samples[3] == 7000 && samples[4] == -7000);
    CHECK(phase == 200U);
    CHECK(velot8_audio_generate_square_wave(NULL, 1U, 1000U, &phase) == 0U);
    CHECK(velot8_audio_generate_square_wave(samples, 1U, 0U, &phase) == 0U);
    if (failures != 0) {
        return 1;
    }
    (void)puts("SDL audio tests passed");
    return 0;
}
