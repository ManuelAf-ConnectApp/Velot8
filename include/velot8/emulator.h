/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef VELOT8_EMULATOR_H
#define VELOT8_EMULATOR_H

#include "velot8/cpu.h"

typedef enum Velot8Error {
    VELOT8_OK = 0,
    VELOT8_ERROR_ARGUMENT,
    VELOT8_ERROR_ROM_TOO_LARGE,
    VELOT8_ERROR_MEMORY_ACCESS,
    VELOT8_ERROR_STACK_OVERFLOW,
    VELOT8_ERROR_STACK_UNDERFLOW,
    VELOT8_ERROR_INVALID_OPCODE
} Velot8Error;

typedef struct Velot8Quirks {
    bool shift_uses_vy;
    bool load_store_increment_i;
    bool jump_uses_vx;
    bool logic_clears_vf;
    bool draw_wrap;
} Velot8Quirks;

typedef enum Velot8QuirkProfile {
    VELOT8_PROFILE_MODERN = 0,
    VELOT8_PROFILE_VIP,
    VELOT8_PROFILE_CHIP48
} Velot8QuirkProfile;

typedef struct Velot8Emulator {
    Velot8Cpu cpu;
    uint8_t memory[VELOT8_MEMORY_SIZE];
    uint8_t framebuffer[VELOT8_SCREEN_WIDTH * VELOT8_SCREEN_HEIGHT];
    uint16_t keys;
    uint8_t delay_timer;
    uint8_t sound_timer;
    bool draw_pending;
    bool halted;
    size_t rom_size;
    uint32_t random_state;
    Velot8Quirks quirks;
    Velot8Error last_error;
} Velot8Emulator;

void velot8_init(Velot8Emulator *emulator);
void velot8_reset(Velot8Emulator *emulator);
void velot8_set_quirks(Velot8Emulator *emulator, Velot8Quirks quirks);
Velot8Quirks velot8_quirks_for_profile(Velot8QuirkProfile profile);
Velot8Error velot8_load_rom(Velot8Emulator *emulator, const uint8_t *rom,
                            size_t rom_size);
Velot8Error velot8_step(Velot8Emulator *emulator);
void velot8_tick_timers(Velot8Emulator *emulator);
void velot8_set_key(Velot8Emulator *emulator, uint8_t key, bool pressed);
const char *velot8_error_string(Velot8Error error);

#endif
