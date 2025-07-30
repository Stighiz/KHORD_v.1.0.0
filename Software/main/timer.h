#pragma once

#include <stdint.h>

//Timer
#include "driver/gptimer.h"
//ESP_IDF
#include "esp_attr.h"

#define TIMER_DIFF_32(a, b) (uint32_t)((a) - (b))

typedef uint32_t timer_t;

/*
    To optimise CPU utilisation and allow separate threads to access the processor, we
    chose to use a periodic timer that triggers an interrupt. the interrupt in question simply sets a flag
    which will notify the main thread that it is time to do a scan() of the keyboard.
    This technique is called ‘deferred work’ and allows us to spend as little time as possible in the code section of the interrupt.
 */
extern bool matrix_scan_requested;

timer_t millis_elapsed();

void timer_setup(void);

bool IRAM_ATTR timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);
