/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <string.h>

#include "velot8/emulator.h"
#include "velot8/scheduler.h"

static int failures;

#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        ++failures; \
    } \
} while (0)

static void test_reset_and_loader(void) {
    Velot8Emulator emulator;
    uint8_t rom[] = {0x60, 0xAB};
    uint8_t oversized[VELOT8_MEMORY_SIZE - VELOT8_PROGRAM_START + 1U] = {0};
    velot8_init(&emulator);
    CHECK(emulator.cpu.pc == VELOT8_PROGRAM_START);
    CHECK(velot8_load_rom(&emulator, rom, sizeof(rom)) == VELOT8_OK);
    CHECK(emulator.memory[VELOT8_PROGRAM_START] == 0x60U);
    emulator.cpu.v[0] = 0x55U;
    velot8_reset(&emulator);
    CHECK(emulator.cpu.pc == VELOT8_PROGRAM_START && emulator.cpu.v[0] == 0U);
    CHECK(emulator.memory[VELOT8_PROGRAM_START] == 0x60U);
    CHECK(velot8_load_rom(&emulator, oversized, sizeof(oversized)) == VELOT8_ERROR_ROM_TOO_LARGE);
}

static void test_instruction_and_draw(void) {
    Velot8Emulator emulator;
    const uint8_t rom[] = {0x60, 0x03, 0x61, 0x04, 0xA2, 0x08, 0xD0, 0x11, 0x81};
    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, rom, sizeof(rom)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.v[0] == 3U);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.framebuffer[4U * VELOT8_SCREEN_WIDTH + 3U] == 1U);
    CHECK(emulator.framebuffer[4U * VELOT8_SCREEN_WIDTH + 10U] == 1U);
    emulator.cpu.pc = (uint16_t)(VELOT8_PROGRAM_START + 6U);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.v[0xFU] == 1U);
}

static void test_call_return_and_arithmetic(void) {
    Velot8Emulator emulator;
    const uint8_t call_rom[] = {0x22, 0x06, 0x60, 0xFF, 0x12, 0x0A, 0x60, 0x03, 0x00, 0xEE};
    const uint8_t alu_rom[] = {
        0x60, 0xFE, 0x61, 0x02, 0x80, 0x14, 0x60, 0x03, 0x61, 0x05,
        0x80, 0x15, 0x60, 0x03, 0x61, 0x04, 0x80, 0x17, 0x60, 0x03,
        0x80, 0x16
    };
    unsigned step;

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, call_rom, sizeof(call_rom)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.pc == 0x206U && emulator.cpu.sp == 1U);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.pc == 0x202U && emulator.cpu.sp == 0U);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.v[0] == 0xFFU);

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, alu_rom, sizeof(alu_rom)) == VELOT8_OK);
    for (step = 0; step < 3U; ++step) CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.v[0] == 0U && emulator.cpu.v[0xF] == 1U);
    for (step = 0; step < 3U; ++step) CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.v[0] == 0xFEU && emulator.cpu.v[0xF] == 0U);
    for (step = 0; step < 3U; ++step) CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.v[0] == 1U && emulator.cpu.v[0xF] == 1U);
    for (step = 0; step < 2U; ++step) CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.v[0] == 1U && emulator.cpu.v[0xF] == 1U);
}

static void test_memory_timers_and_keyboard(void) {
    Velot8Emulator emulator;
    const uint8_t rom[] = {
        0x60, 0xE7, 0xA3, 0x00, 0xF0, 0x33, 0xF0, 0x15,
        0xF1, 0x07, 0xF2, 0x0A, 0x60, 0x0A, 0x61, 0x0B,
        0xA3, 0x10, 0xF1, 0x55, 0x60, 0x00, 0x61, 0x00,
        0xF1, 0x65
    };
    unsigned step;

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, rom, sizeof(rom)) == VELOT8_OK);
    for (step = 0; step < 6U; ++step) CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.memory[0x300] == 2U && emulator.memory[0x301] == 3U && emulator.memory[0x302] == 1U);
    CHECK(emulator.delay_timer == 0xE7U && emulator.cpu.v[1] == 0xE7U);
    CHECK(emulator.cpu.waiting_for_key);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.waiting_for_key);
    velot8_set_key(&emulator, 0xAU, true);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(!emulator.cpu.waiting_for_key && emulator.cpu.v[2] == 0xAU);
    velot8_set_key(&emulator, 0xAU, false);
    for (step = 0; step < 7U; ++step) CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.memory[0x310] == 0x0AU && emulator.memory[0x311] == 0x0BU);
    CHECK(emulator.cpu.v[0] == 0x0AU && emulator.cpu.v[1] == 0x0BU);
    velot8_tick_timers(&emulator);
    CHECK(emulator.delay_timer == 0xE6U);
}

static void test_quirks_and_errors(void) {
    Velot8Emulator emulator;
    Velot8Quirks quirks = {true, true, false, true, false};
    const uint8_t rom[] = {0x60, 0x03, 0x61, 0x08, 0x80, 0x16, 0x80, 0x11};
    const uint8_t invalid[] = {0xFF, 0xFF};

    velot8_init(&emulator);
    velot8_set_quirks(&emulator, quirks);
    CHECK(velot8_load_rom(&emulator, rom, sizeof(rom)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.v[0] == 4U && emulator.cpu.v[0xF] == 0U);
    emulator.cpu.v[0xF] = 1U;
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.v[0xF] == 0U);

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, invalid, sizeof(invalid)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_ERROR_INVALID_OPCODE);
    CHECK(emulator.halted);
}

static void test_quirk_profiles(void) {
    Velot8Quirks modern = velot8_quirks_for_profile(VELOT8_PROFILE_MODERN);
    Velot8Quirks vip = velot8_quirks_for_profile(VELOT8_PROFILE_VIP);
    Velot8Quirks chip48 = velot8_quirks_for_profile(VELOT8_PROFILE_CHIP48);

    CHECK(modern.draw_wrap && !modern.shift_uses_vy && !modern.load_store_increment_i);
    CHECK(vip.draw_wrap && vip.shift_uses_vy && vip.load_store_increment_i);
    CHECK(!chip48.draw_wrap && chip48.jump_uses_vx && !chip48.shift_uses_vy);
}

static void test_skips_jumps_random_and_key_opcodes(void) {
    Velot8Emulator emulator;
    Velot8Emulator second;
    const uint8_t skip_rom[] = {0x60, 0x01, 0x30, 0x01, 0x61, 0x02, 0x61, 0x0F};
    const uint8_t jump_rom[] = {0x60, 0x04, 0x62, 0x03, 0xB2, 0x10};
    const uint8_t random_rom[] = {0xC0, 0xF0};
    const uint8_t key_rom[] = {
        0x60, 0x0A, 0xE0, 0x9E, 0x12, 0x08, 0x61, 0x01,
        0xE0, 0xA1, 0x12, 0x0C, 0x61, 0x02
    };
    Velot8Quirks chip48 = velot8_quirks_for_profile(VELOT8_PROFILE_CHIP48);

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, skip_rom, sizeof(skip_rom)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.pc == 0x206U);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.v[1] == 0x0FU);

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, jump_rom, sizeof(jump_rom)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.pc == 0x214U);
    velot8_init(&emulator);
    velot8_set_quirks(&emulator, chip48);
    CHECK(velot8_load_rom(&emulator, jump_rom, sizeof(jump_rom)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.pc == 0x013U);

    velot8_init(&emulator);
    velot8_init(&second);
    CHECK(velot8_load_rom(&emulator, random_rom, sizeof(random_rom)) == VELOT8_OK);
    CHECK(velot8_load_rom(&second, random_rom, sizeof(random_rom)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK && velot8_step(&second) == VELOT8_OK);
    CHECK(emulator.cpu.v[0] == second.cpu.v[0]);
    CHECK((emulator.cpu.v[0] & 0x0FU) == 0U);

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, key_rom, sizeof(key_rom)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    velot8_set_key(&emulator, 0xAU, true);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.pc == 0x206U);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.v[1] == 1U);
    velot8_set_key(&emulator, 0xAU, false);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.pc == 0x20CU);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.v[1] == 2U);
}

static void test_draw_modes_and_memory_opcodes(void) {
    Velot8Emulator emulator;
    const uint8_t draw_rom[] = {0x60, 0x3F, 0x61, 0x1F, 0xA2, 0x08, 0xD0, 0x11, 0xC0};
    const uint8_t memory_rom[] = {
        0x60, 0x0A, 0xF0, 0x29, 0x60, 0x05, 0xF0, 0x1E,
        0x60, 0x07, 0xF0, 0x18
    };
    Velot8Quirks clipped = velot8_quirks_for_profile(VELOT8_PROFILE_CHIP48);
    unsigned step;

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, draw_rom, sizeof(draw_rom)) == VELOT8_OK);
    for (step = 0; step < 4U; ++step) CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.framebuffer[31U * VELOT8_SCREEN_WIDTH + 63U] == 1U);
    CHECK(emulator.framebuffer[31U * VELOT8_SCREEN_WIDTH] == 1U);

    velot8_init(&emulator);
    velot8_set_quirks(&emulator, clipped);
    CHECK(velot8_load_rom(&emulator, draw_rom, sizeof(draw_rom)) == VELOT8_OK);
    for (step = 0; step < 4U; ++step) CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.framebuffer[31U * VELOT8_SCREEN_WIDTH + 63U] == 1U);
    CHECK(emulator.framebuffer[31U * VELOT8_SCREEN_WIDTH] == 0U);

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, memory_rom, sizeof(memory_rom)) == VELOT8_OK);
    for (step = 0; step < 3U; ++step) CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(emulator.cpu.i == 0x032U);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.cpu.i == 0x037U);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK && emulator.sound_timer == 7U);
}

static void test_error_boundaries(void) {
    Velot8Emulator emulator;
    const uint8_t invalid_5xy[] = {0x50, 0x11};
    const uint8_t invalid_9xy[] = {0x90, 0x11};
    const uint8_t bcd_out_of_range[] = {0xAF, 0xFF, 0xF0, 0x33};
    const uint8_t recursive_call[] = {0x22, 0x00};
    unsigned step;

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, invalid_5xy, sizeof(invalid_5xy)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_ERROR_INVALID_OPCODE);
    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, invalid_9xy, sizeof(invalid_9xy)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_ERROR_INVALID_OPCODE);
    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, bcd_out_of_range, sizeof(bcd_out_of_range)) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_ERROR_MEMORY_ACCESS);
    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, recursive_call, sizeof(recursive_call)) == VELOT8_OK);
    for (step = 0; step < VELOT8_STACK_DEPTH; ++step) CHECK(velot8_step(&emulator) == VELOT8_OK);
    CHECK(velot8_step(&emulator) == VELOT8_ERROR_STACK_OVERFLOW);
}

static void test_scheduler(void) {
    Velot8Emulator emulator;
    Velot8Scheduler scheduler;
    const uint8_t loop_rom[] = {0x60, 0x01, 0x70, 0x01, 0x12, 0x02};
    const uint8_t invalid_rom[] = {0xFF, 0xFF};
    unsigned executed = 99U;

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, loop_rom, sizeof(loop_rom)) == VELOT8_OK);
    emulator.delay_timer = 3U;
    emulator.sound_timer = 3U;
    velot8_scheduler_init(&scheduler, 100U);
    CHECK(velot8_scheduler_advance(&scheduler, &emulator, 0.009, &executed) == VELOT8_OK);
    CHECK(executed == 0U && emulator.cpu.v[0] == 0U && emulator.delay_timer == 3U);
    CHECK(velot8_scheduler_advance(&scheduler, &emulator, 0.0111, &executed) == VELOT8_OK);
    CHECK(executed == 2U && emulator.cpu.v[0] == 2U);
    CHECK(emulator.delay_timer == 2U && emulator.sound_timer == 2U);
    CHECK(velot8_scheduler_advance(&scheduler, &emulator, -0.01, &executed) == VELOT8_ERROR_ARGUMENT);

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, loop_rom, sizeof(loop_rom)) == VELOT8_OK);
    velot8_scheduler_init(&scheduler, 10U);
    CHECK(velot8_scheduler_advance(&scheduler, &emulator, 0.5, &executed) == VELOT8_OK);
    CHECK(executed == 1U);

    velot8_init(&emulator);
    CHECK(velot8_load_rom(&emulator, invalid_rom, sizeof(invalid_rom)) == VELOT8_OK);
    velot8_scheduler_init(&scheduler, 10U);
    CHECK(velot8_scheduler_advance(&scheduler, &emulator, 0.1, &executed) == VELOT8_ERROR_INVALID_OPCODE);
    CHECK(executed == 0U);
}

int main(void) {
    test_reset_and_loader();
    test_instruction_and_draw();
    test_call_return_and_arithmetic();
    test_memory_timers_and_keyboard();
    test_quirks_and_errors();
    test_quirk_profiles();
    test_skips_jumps_random_and_key_opcodes();
    test_draw_modes_and_memory_opcodes();
    test_error_boundaries();
    test_scheduler();
    if (failures != 0) {
        return 1;
    }
    (void)puts("core tests passed");
    return 0;
}
