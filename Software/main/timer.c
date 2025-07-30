#include "timer.h"
#include "config.h"

//ESP_IDF
#include "esp_timer.h"
#include "driver/gptimer.h"
#include "esp_log.h"

static const char *TAG = "timer";

bool matrix_scan_requested = false;

timer_t millis_elapsed() {
    return esp_timer_get_time() / 1000;
}

static gptimer_handle_t gptimer = NULL;

bool timer_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    matrix_scan_requested = true;
    
    // Return true to yield from ISR if a higher priority task was woken (optional here)
    return true;  
}

void timer_setup(void)
{
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,   // Use default clock (APB clock)
        .direction = GPTIMER_COUNT_UP,        // Direction of counting
        .resolution_hz = TIMER_RESOLUTION,    // 1 MHz, 1 tick = 1 microsecond
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_callback,            // Set the ISR
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &(gptimer_alarm_config_t){
        .alarm_count = TIMER_SCAN_DELAY,    // 1000 us = 1 ms (system tick every 3 ms)
        .reload_count = TIMER_COUNT_START,
        .flags.auto_reload_on_alarm = true,
    }));

    ESP_ERROR_CHECK(gptimer_start(gptimer));

    ESP_LOGI(TAG, "System timer started!");
}