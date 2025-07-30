#include <stdio.h>
#include <stdbool.h>

#include "matrix.h"
#include "config.h"
#include "keyboard.h"
#include "timer.h"
#include "usb.h"

//ESP-IDF
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

//Delay
#include "esp_rom_sys.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "-----------------------------------------------");
    ESP_LOGI(TAG, "Configuring TinyUSB");
    usb_init();
    
    vTaskDelay(10);

    ESP_LOGI(TAG, "-----------------------------------------------");
    ESP_LOGI(TAG, "Starting MATRIX_INIT");
    matrix_init();

    vTaskDelay(10);

    ESP_LOGI(TAG, "-----------------------------------------------");
    ESP_LOGI(TAG, "Starting TIMER_SETUP");
    timer_setup();

    while(1) 
    {
        if (matrix_scan_requested)
        {
            matrix_scan_requested = false;  // Clear flag
            matrix_task();                  
        }

        //Micro delay to yield the CPU to other processes
        vTaskDelay(1);
    }
}