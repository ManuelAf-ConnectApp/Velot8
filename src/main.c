/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "demo_rom.h"
#include "velot8/emulator.h"
#include "velot8/version.h"

#ifndef VELOT8_HEADLESS
#include "app.h"
#endif

static int load_file(Velot8Emulator *emulator, const char *path) {
    FILE *file = fopen(path, "rb");
    uint8_t bytes[VELOT8_MEMORY_SIZE - VELOT8_PROGRAM_START + 1U];
    size_t size;
    if (file == NULL) {
        (void)fprintf(stderr, "Cannot open ROM '%s'\n", path);
        return 1;
    }
    size = fread(bytes, 1, sizeof(bytes), file);
    if (ferror(file) != 0 || fclose(file) != 0) {
        (void)fprintf(stderr, "Cannot read ROM '%s'\n", path);
        return 1;
    }
    if (velot8_load_rom(emulator, bytes, size) != VELOT8_OK) {
        (void)fprintf(stderr, "Cannot load ROM '%s': %s\n", path,
                      velot8_error_string(emulator->last_error));
        return 1;
    }
    return 0;
}

static int parse_scale(const char *value, int *scale) {
    char *end = NULL;
    long parsed = strtol(value, &end, 10);
    if (end == value || *end != '\0' || parsed < 1L || parsed > 64L) {
        return 1;
    }
    *scale = (int)parsed;
    return 0;
}

static int parse_cpu_hz(const char *value, unsigned *cpu_hz) {
    char *end = NULL;
    unsigned long parsed = strtoul(value, &end, 10);
    if (end == value || *end != '\0' || parsed < 1UL || parsed > 5000UL) {
        return 1;
    }
    *cpu_hz = (unsigned)parsed;
    return 0;
}

static int parse_steps(const char *value, unsigned *steps) {
    char *end = NULL;
    unsigned long parsed = strtoul(value, &end, 10);
    if (end == value || *end != '\0' || parsed < 1UL || parsed > 100000000UL) {
        return 1;
    }
    *steps = (unsigned)parsed;
    return 0;
}

static int parse_profile(const char *value, Velot8QuirkProfile *profile) {
    if (strcmp(value, "modern") == 0) {
        *profile = VELOT8_PROFILE_MODERN;
    } else if (strcmp(value, "vip") == 0) {
        *profile = VELOT8_PROFILE_VIP;
    } else if (strcmp(value, "chip48") == 0) {
        *profile = VELOT8_PROFILE_CHIP48;
    } else {
        return 1;
    }
    return 0;
}

static void trace_step(const Velot8Emulator *emulator) {
    uint16_t opcode = (uint16_t)(((uint16_t)emulator->memory[emulator->cpu.pc] << 8U) |
                                 emulator->memory[emulator->cpu.pc + 1U]);
    unsigned register_index;
    (void)fprintf(stderr, "PC=%03X OP=%04X I=%03X", emulator->cpu.pc, opcode,
                  emulator->cpu.i);
    for (register_index = 0; register_index < 16U; ++register_index) {
        (void)fprintf(stderr, " V%X=%02X", register_index, emulator->cpu.v[register_index]);
    }
    (void)fputc('\n', stderr);
}

int main(int argc, char **argv) {
    Velot8Emulator emulator;
    const char *rom_name = NULL;
    int scale = 15;
    int index;
    bool demo = false;
    bool headless = false;
    bool trace = false;
    unsigned cpu_hz = 700U;
    unsigned headless_steps = 120U;
    Velot8QuirkProfile profile = VELOT8_PROFILE_MODERN;

    velot8_init(&emulator);
#ifdef VELOT8_HEADLESS
    headless = true;
#endif
    for (index = 1; index < argc; ++index) {
        if (strcmp(argv[index], "--version") == 0) {
            (void)printf("velot8 %s\n", VELOT8_VERSION);
            return 0;
        } else if (strcmp(argv[index], "--demo") == 0) {
            demo = true;
        } else if (strcmp(argv[index], "--headless") == 0) {
            headless = true;
        } else if (strcmp(argv[index], "--trace") == 0) {
            trace = true;
        } else if (strcmp(argv[index], "--rom") == 0 && index + 1 < argc) {
            rom_name = argv[++index];
        } else if (strcmp(argv[index], "--scale") == 0 && index + 1 < argc) {
            if (parse_scale(argv[++index], &scale) != 0) {
                (void)fprintf(stderr, "--scale must be an integer from 1 to 64\n");
                return 2;
            }
        } else if (strcmp(argv[index], "--cpu-hz") == 0 && index + 1 < argc) {
            if (parse_cpu_hz(argv[++index], &cpu_hz) != 0) {
                (void)fprintf(stderr, "--cpu-hz must be an integer from 1 to 5000\n");
                return 2;
            }
        } else if (strcmp(argv[index], "--steps") == 0 && index + 1 < argc) {
            if (parse_steps(argv[++index], &headless_steps) != 0) {
                (void)fprintf(stderr, "--steps must be an integer from 1 to 100000000\n");
                return 2;
            }
        } else if (strcmp(argv[index], "--profile") == 0 && index + 1 < argc) {
            if (parse_profile(argv[++index], &profile) != 0) {
                (void)fprintf(stderr, "--profile must be modern, vip, or chip48\n");
                return 2;
            }
        } else {
            (void)fprintf(stderr, "Usage: %s [--version] [--demo] [--rom FILE] [--scale N] [--cpu-hz N] [--profile NAME] [--trace] [--steps N] [--headless]\n", argv[0]);
            return 2;
        }
    }
    if (demo && rom_name != NULL) {
        (void)fprintf(stderr, "Choose either --demo or --rom, not both\n");
        return 2;
    }
    if (demo) {
        if (velot8_load_rom(&emulator, velot8_demo_rom, velot8_demo_rom_size) != VELOT8_OK) {
            return 1;
        }
        rom_name = "DEMO";
    } else if (rom_name != NULL && load_file(&emulator, rom_name) != 0) {
        return 1;
    }
    velot8_set_quirks(&emulator, velot8_quirks_for_profile(profile));
    if (headless) {
        unsigned steps;
        for (steps = 0; steps < headless_steps && rom_name != NULL; ++steps) {
            if (trace && emulator.cpu.pc <= VELOT8_MEMORY_SIZE - 2U) {
                trace_step(&emulator);
            }
            if (velot8_step(&emulator) != VELOT8_OK) {
                (void)fprintf(stderr, "Emulation stopped: %s\n", velot8_error_string(emulator.last_error));
                return 1;
            }
        }
        (void)printf("headless: pc=0x%03X, framebuffer=%s\n", emulator.cpu.pc,
                     emulator.draw_pending ? "updated" : "unchanged");
        return 0;
    }
#ifdef VELOT8_HEADLESS
    (void)fprintf(stderr, "This binary was built without SDL2; use --headless.\n");
    return 2;
#else
    return velot8_sdl_run(&emulator, rom_name, scale, cpu_hz);
#endif
}
