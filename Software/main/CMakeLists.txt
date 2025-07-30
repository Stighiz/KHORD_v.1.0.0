idf_component_register(SRCS "main.c" "matrix.c" "debounce.c" "timer.c" "usb.c" "keymap.c" "keyboard.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_timer
                       REQUIRES esp_driver_gptimer)
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)