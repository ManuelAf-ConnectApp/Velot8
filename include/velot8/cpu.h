/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef VELOT8_CPU_H
#define VELOT8_CPU_H

#include "velot8/types.h"

enum {
    VELOT8_MEMORY_SIZE = 4096,
    VELOT8_PROGRAM_START = 0x200,
    VELOT8_SCREEN_WIDTH = 64,
    VELOT8_SCREEN_HEIGHT = 32,
    VELOT8_STACK_DEPTH = 16
};

typedef struct Velot8Cpu {
    uint8_t v[16];
    uint16_t i;
    uint16_t pc;
    uint16_t stack[VELOT8_STACK_DEPTH];
    uint8_t sp;
    bool waiting_for_key;
    uint8_t waiting_register;
} Velot8Cpu;

#endif
