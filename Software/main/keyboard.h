#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "timer.h"

typedef struct {
    uint8_t col;
    uint8_t row;
} keypos_t;

typedef struct {
    keypos_t       key;
    timer_t        timestamp;
    bool           pressed;
} keyevent_t;

void process_event(keyevent_t event);

bool matrix_task(void);