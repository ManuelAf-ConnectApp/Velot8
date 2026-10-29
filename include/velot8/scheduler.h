/* Copyright 2026 Manuel María Alconchel Fernánde */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef VELOT8_SCHEDULER_H
#define VELOT8_SCHEDULER_H

#include "velot8/emulator.h"

enum { VELOT8_TIMER_HZ = 60 };

typedef struct Velot8Scheduler {
    double cpu_accumulator;
    double timer_accumulator;
    unsigned cpu_hz;
} Velot8Scheduler;

void velot8_scheduler_init(Velot8Scheduler *scheduler, unsigned cpu_hz);
Velot8Error velot8_scheduler_advance(Velot8Scheduler *scheduler,
                                      Velot8Emulator *emulator,
                                      double delta_seconds,
                                      unsigned *instructions_executed);

#endif
