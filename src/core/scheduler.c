/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#include "velot8/scheduler.h"

enum { VELOT8_MAX_DELTA_MILLISECONDS = 100 };

void velot8_scheduler_init(Velot8Scheduler *scheduler, unsigned cpu_hz) {
    if (scheduler == NULL) {
        return;
    }
    scheduler->cpu_accumulator = 0.0;
    scheduler->timer_accumulator = 0.0;
    scheduler->cpu_hz = cpu_hz;
}

Velot8Error velot8_scheduler_advance(Velot8Scheduler *scheduler,
                                      Velot8Emulator *emulator,
                                      double delta_seconds,
                                      unsigned *instructions_executed) {
    double cpu_period;
    double timer_period = 1.0 / VELOT8_TIMER_HZ;
    unsigned executed = 0;
    Velot8Error error;

    if (scheduler == NULL || emulator == NULL || scheduler->cpu_hz == 0U ||
        delta_seconds < 0.0) {
        return VELOT8_ERROR_ARGUMENT;
    }
    if (delta_seconds > VELOT8_MAX_DELTA_MILLISECONDS / 1000.0) {
        delta_seconds = VELOT8_MAX_DELTA_MILLISECONDS / 1000.0;
    }
    cpu_period = 1.0 / scheduler->cpu_hz;
    scheduler->cpu_accumulator += delta_seconds;
    scheduler->timer_accumulator += delta_seconds;
    while (scheduler->cpu_accumulator >= cpu_period) {
        error = velot8_step(emulator);
        if (error != VELOT8_OK) {
            if (instructions_executed != NULL) {
                *instructions_executed = executed;
            }
            return error;
        }
        scheduler->cpu_accumulator -= cpu_period;
        ++executed;
    }
    while (scheduler->timer_accumulator >= timer_period) {
        velot8_tick_timers(emulator);
        scheduler->timer_accumulator -= timer_period;
    }
    if (instructions_executed != NULL) {
        *instructions_executed = executed;
    }
    return VELOT8_OK;
}
