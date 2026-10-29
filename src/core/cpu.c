/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include "velot8/emulator.h"

enum { VELOT8_FONT_START = 0 };

static Velot8Error memory_range(size_t address, size_t length) {
    return address <= VELOT8_MEMORY_SIZE && length <= VELOT8_MEMORY_SIZE - address
               ? VELOT8_OK
               : VELOT8_ERROR_MEMORY_ACCESS;
}

static Velot8Error set_pc(Velot8Cpu *cpu, uint16_t address) {
    if (address > VELOT8_MEMORY_SIZE - 2U) {
        return VELOT8_ERROR_MEMORY_ACCESS;
    }
    cpu->pc = address;
    return VELOT8_OK;
}

static uint8_t next_random(Velot8Emulator *emulator) {
    uint32_t value = emulator->random_state;
    value ^= value << 13U;
    value ^= value >> 17U;
    value ^= value << 5U;
    emulator->random_state = value;
    return (uint8_t)value;
}

static Velot8Error draw_sprite(Velot8Emulator *emulator, uint8_t x, uint8_t y,
                               uint8_t height) {
    uint8_t row;
    uint8_t collision = 0;
    Velot8Error error = memory_range(emulator->cpu.i, height);

    if (error != VELOT8_OK) {
        return error;
    }
    for (row = 0; row < height; ++row) {
        uint8_t sprite = emulator->memory[emulator->cpu.i + row];
        uint8_t column;
        for (column = 0; column < 8U; ++column) {
            uint16_t draw_x;
            uint16_t draw_y;
            size_t pixel;
            if ((sprite & (uint8_t)(0x80U >> column)) == 0U) {
                continue;
            }
            draw_x = (uint16_t)x + column;
            draw_y = (uint16_t)y + row;
            if (emulator->quirks.draw_wrap) {
                draw_x %= VELOT8_SCREEN_WIDTH;
                draw_y %= VELOT8_SCREEN_HEIGHT;
            } else if (draw_x >= VELOT8_SCREEN_WIDTH || draw_y >= VELOT8_SCREEN_HEIGHT) {
                continue;
            }
            pixel = (size_t)draw_y * VELOT8_SCREEN_WIDTH + draw_x;
            if (emulator->framebuffer[pixel] != 0U) {
                collision = 1;
            }
            emulator->framebuffer[pixel] ^= 1U;
        }
    }
    emulator->cpu.v[0xFU] = collision;
    emulator->draw_pending = true;
    return VELOT8_OK;
}

static Velot8Error skip_instruction(Velot8Cpu *cpu) {
    return set_pc(cpu, (uint16_t)(cpu->pc + 2U));
}

static Velot8Error execute_8xy(Velot8Emulator *emulator, uint8_t x, uint8_t y,
                                uint8_t operation) {
    Velot8Cpu *cpu = &emulator->cpu;
    uint8_t source;
    uint16_t sum;

    switch (operation) {
    case 0x0U: cpu->v[x] = cpu->v[y]; return VELOT8_OK;
    case 0x1U: cpu->v[x] |= cpu->v[y]; break;
    case 0x2U: cpu->v[x] &= cpu->v[y]; break;
    case 0x3U: cpu->v[x] ^= cpu->v[y]; break;
    case 0x4U:
        sum = (uint16_t)cpu->v[x] + cpu->v[y];
        cpu->v[x] = (uint8_t)sum;
        cpu->v[0xFU] = sum > UINT8_MAX ? 1U : 0U;
        return VELOT8_OK;
    case 0x5U:
        cpu->v[0xFU] = cpu->v[x] >= cpu->v[y] ? 1U : 0U;
        cpu->v[x] = (uint8_t)(cpu->v[x] - cpu->v[y]);
        return VELOT8_OK;
    case 0x6U:
        source = emulator->quirks.shift_uses_vy ? cpu->v[y] : cpu->v[x];
        cpu->v[0xFU] = source & 1U;
        cpu->v[x] = (uint8_t)(source >> 1U);
        return VELOT8_OK;
    case 0x7U:
        cpu->v[0xFU] = cpu->v[y] >= cpu->v[x] ? 1U : 0U;
        cpu->v[x] = (uint8_t)(cpu->v[y] - cpu->v[x]);
        return VELOT8_OK;
    case 0xEU:
        source = emulator->quirks.shift_uses_vy ? cpu->v[y] : cpu->v[x];
        cpu->v[0xFU] = (source >> 7U) & 1U;
        cpu->v[x] = (uint8_t)(source << 1U);
        return VELOT8_OK;
    default: return VELOT8_ERROR_INVALID_OPCODE;
    }
    if (emulator->quirks.logic_clears_vf) {
        cpu->v[0xFU] = 0;
    }
    return VELOT8_OK;
}

static Velot8Error execute_fxy(Velot8Emulator *emulator, uint8_t x, uint8_t operation) {
    Velot8Cpu *cpu = &emulator->cpu;
    Velot8Error error;
    size_t index;
    switch (operation) {
    case 0x07U: cpu->v[x] = emulator->delay_timer; return VELOT8_OK;
    case 0x0AU: cpu->waiting_for_key = true; cpu->waiting_register = x; return VELOT8_OK;
    case 0x15U: emulator->delay_timer = cpu->v[x]; return VELOT8_OK;
    case 0x18U: emulator->sound_timer = cpu->v[x]; return VELOT8_OK;
    case 0x1EU: cpu->i = (uint16_t)(cpu->i + cpu->v[x]); return VELOT8_OK;
    case 0x29U: cpu->i = (uint16_t)(VELOT8_FONT_START + 5U * (cpu->v[x] & 0x0FU)); return VELOT8_OK;
    case 0x33U:
        error = memory_range(cpu->i, 3U);
        if (error != VELOT8_OK) return error;
        emulator->memory[cpu->i] = (uint8_t)(cpu->v[x] / 100U);
        emulator->memory[cpu->i + 1U] = (uint8_t)((cpu->v[x] / 10U) % 10U);
        emulator->memory[cpu->i + 2U] = (uint8_t)(cpu->v[x] % 10U);
        return VELOT8_OK;
    case 0x55U:
    case 0x65U:
        error = memory_range(cpu->i, (size_t)x + 1U);
        if (error != VELOT8_OK) return error;
        for (index = 0; index <= x; ++index) {
            if (operation == 0x55U) emulator->memory[cpu->i + index] = cpu->v[index];
            else cpu->v[index] = emulator->memory[cpu->i + index];
        }
        if (emulator->quirks.load_store_increment_i) cpu->i = (uint16_t)(cpu->i + x + 1U);
        return VELOT8_OK;
    default: return VELOT8_ERROR_INVALID_OPCODE;
    }
}

static Velot8Error execute(Velot8Emulator *emulator, uint16_t opcode) {
    Velot8Cpu *cpu = &emulator->cpu;
    uint8_t x = (uint8_t)((opcode >> 8U) & 0x0FU);
    uint8_t y = (uint8_t)((opcode >> 4U) & 0x0FU);
    uint8_t nn = (uint8_t)opcode;
    uint16_t nnn = opcode & 0x0FFFU;
    switch (opcode & 0xF000U) {
    case 0x0000U:
        if (opcode == 0x00E0U) {
            size_t pixel;
            for (pixel = 0; pixel < sizeof(emulator->framebuffer); ++pixel) emulator->framebuffer[pixel] = 0;
            emulator->draw_pending = true;
            return VELOT8_OK;
        }
        if (opcode == 0x00EEU) {
            if (cpu->sp == 0U) return VELOT8_ERROR_STACK_UNDERFLOW;
            return set_pc(cpu, cpu->stack[--cpu->sp]);
        }
        return VELOT8_ERROR_INVALID_OPCODE;
    case 0x1000U: return set_pc(cpu, nnn);
    case 0x2000U:
        if (cpu->sp >= VELOT8_STACK_DEPTH) return VELOT8_ERROR_STACK_OVERFLOW;
        if (nnn > VELOT8_MEMORY_SIZE - 2U) return VELOT8_ERROR_MEMORY_ACCESS;
        cpu->stack[cpu->sp++] = cpu->pc;
        cpu->pc = nnn;
        return VELOT8_OK;
    case 0x3000U: return cpu->v[x] == nn ? skip_instruction(cpu) : VELOT8_OK;
    case 0x4000U: return cpu->v[x] != nn ? skip_instruction(cpu) : VELOT8_OK;
    case 0x5000U:
        if ((opcode & 0x000FU) != 0U) return VELOT8_ERROR_INVALID_OPCODE;
        return cpu->v[x] == cpu->v[y] ? skip_instruction(cpu) : VELOT8_OK;
    case 0x6000U: cpu->v[x] = nn; return VELOT8_OK;
    case 0x7000U: cpu->v[x] = (uint8_t)(cpu->v[x] + nn); return VELOT8_OK;
    case 0x8000U: return execute_8xy(emulator, x, y, (uint8_t)(opcode & 0x000FU));
    case 0x9000U:
        if ((opcode & 0x000FU) != 0U) return VELOT8_ERROR_INVALID_OPCODE;
        return cpu->v[x] != cpu->v[y] ? skip_instruction(cpu) : VELOT8_OK;
    case 0xA000U: cpu->i = nnn; return VELOT8_OK;
    case 0xB000U: return set_pc(cpu, emulator->quirks.jump_uses_vx ? (uint16_t)(nn + cpu->v[x]) : (uint16_t)(nnn + cpu->v[0]));
    case 0xC000U: cpu->v[x] = (uint8_t)(next_random(emulator) & nn); return VELOT8_OK;
    case 0xD000U: return draw_sprite(emulator, cpu->v[x], cpu->v[y], (uint8_t)(opcode & 0x000FU));
    case 0xE000U:
        if (cpu->v[x] >= 16U) return VELOT8_OK;
        if (nn == 0x9EU) return (emulator->keys & (uint16_t)(1U << cpu->v[x])) != 0U ? skip_instruction(cpu) : VELOT8_OK;
        if (nn == 0xA1U) return (emulator->keys & (uint16_t)(1U << cpu->v[x])) == 0U ? skip_instruction(cpu) : VELOT8_OK;
        return VELOT8_ERROR_INVALID_OPCODE;
    case 0xF000U: return execute_fxy(emulator, x, nn);
    default: return VELOT8_ERROR_INVALID_OPCODE;
    }
}

Velot8Error velot8_step(Velot8Emulator *emulator) {
    uint16_t opcode;
    Velot8Error error;
    uint8_t key;
    if (emulator == NULL) return VELOT8_ERROR_ARGUMENT;
    if (emulator->halted) return emulator->last_error;
    if (emulator->cpu.waiting_for_key) {
        for (key = 0; key < 16U; ++key) {
            if ((emulator->keys & (uint16_t)(1U << key)) != 0U) {
                emulator->cpu.v[emulator->cpu.waiting_register] = key;
                emulator->cpu.waiting_for_key = false;
                break;
            }
        }
        return VELOT8_OK;
    }
    if (memory_range(emulator->cpu.pc, 2U) != VELOT8_OK) {
        emulator->last_error = VELOT8_ERROR_MEMORY_ACCESS;
        emulator->halted = true;
        return emulator->last_error;
    }
    opcode = (uint16_t)(((uint16_t)emulator->memory[emulator->cpu.pc] << 8U) | emulator->memory[emulator->cpu.pc + 1U]);
    emulator->cpu.pc = (uint16_t)(emulator->cpu.pc + 2U);
    error = execute(emulator, opcode);
    if (error != VELOT8_OK) { emulator->last_error = error; emulator->halted = true; }
    return error;
}
