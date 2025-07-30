#pragma once

#include <stdint.h>
#include <stdbool.h>

void usb_init(void);

void usb_send(const uint8_t* byte, size_t size);