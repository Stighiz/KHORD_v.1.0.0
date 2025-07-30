#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "usb.h"
#include "config.h"

//ESP-IDF
#include "esp_log.h"
//TinyUSB
#include "tinyusb.h"
#include "tusb_cdc_acm.h"

static const char *TAG = "USB";

void usb_init(void) {
    ESP_LOGI(TAG, "USB initialization");

    //Default initialization of the driver descriptor specified in Menuconfig (during installation)
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,         //Uses the default USB device descriptor
        .string_descriptor = NULL,
        .external_phy = false,             //using the internal USB PHY of the ESP32-S3 (not an external chip).
        .configuration_descriptor = NULL,  //default descriptor, will match the classes (CDC-ACM) you register later.
    };

    //It enables USB as a device (not a host) and prepares it to handle descriptors, endpoints, and enumeration
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    //sets up CDC-ACM — the USB class used for virtual serial ports (COM ports)
    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,          //ESP32-S3 supports only 1 USB peripheral
        .cdc_port = TINYUSB_CDC_ACM_0,        //First Virtual COM port to expose
        .rx_unread_buf_sz = 64,               //Size of the buffer before automatic flush
        .callback_rx = NULL,                  //Callback when data is received, in this case we only transmit
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };

    //create and activate a CDC-ACM interface (a virtual serial port over USB).
    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));

    ESP_LOGI(TAG, "USB initialization DONE");
}

void usb_send(const uint8_t* byte, size_t size) {
    tinyusb_cdcacm_write_queue(TINYUSB_CDC_ACM_0, byte, size);
    esp_err_t err = tinyusb_cdcacm_write_flush(TINYUSB_CDC_ACM_0, TIMEOUT_TICKS);

    //We skip the ESP_ERR_NOT_FINISHED error because it is always returned in case of a non-timeout based flush operation
    //to notify the user that the flush will happen asynchrousnly
    if (err != ESP_OK && err != ESP_ERR_NOT_FINISHED) {
        ESP_LOGE(TAG, "CDC ACM write flush error: %s", esp_err_to_name(err));
    }
}