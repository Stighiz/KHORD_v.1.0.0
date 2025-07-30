#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "matrix.h"
#include "debounce.h"
#include "config.h"

//ESP-IDF
#include "esp_log.h"
#include "driver/gpio.h"

//Delay
#include "esp_rom_sys.h"

static const char *TAG = "matrix";

/*
    To save the current state of the keyboard, we make use of two arrays. One contains the RAW values read each scan()
    while the other matrix, the one actually used to consider a key pressed, takes the raw matrix and applies
    the debouncing logic should it be necessary.

    Each array is handled as a vector of MATRIX_ROWS cells, where each cell represents an unsigned integer (16 bits in this case)
    in which the first MATRIX_COLS bits are used to store the state (0: Pressed/1: Released) of the key at position [i, j].
*/
/* matrix state(1:on, 0:off) */
matrix_row_t raw_matrix[MATRIX_ROWS]; // raw values
matrix_row_t matrix[MATRIX_ROWS];     // debounced values

static const gpio_num_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const gpio_num_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;

static bool is_init = true;

void matrix_init_pins(void);
void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row);

inline uint8_t matrix_rows(void) {
    return (uint8_t)MATRIX_ROWS;
}
inline uint8_t matrix_cols(void) {
    return (uint8_t)MATRIX_COLS;
}

static inline void gpio_atomic_set_pin_output_low(gpio_num_t pin) {
    if (is_init) {
        gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        esp_rom_delay_us(1);
    }
    gpio_set_level(pin, LOW);
}

static inline void gpio_atomic_set_pin_output_high(gpio_num_t pin) {
    if (is_init) {
        gpio_set_direction(pin, GPIO_MODE_OUTPUT);    
        esp_rom_delay_us(1);
    }
    gpio_set_level(pin, HIGH);
}

static inline void gpio_atomic_set_pin_input_high(gpio_num_t pin) {
    if (is_init) {    
        gpio_set_direction(pin, GPIO_MODE_INPUT);    
        gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
        esp_rom_delay_us(1);
    }
}

static inline uint8_t readMatrixPin(gpio_num_t pin) {
    return (gpio_get_level(pin) == MATRIX_INPUT_PRESSED_STATE) ? 0 : 1;
}

inline void matrix_output_select_delay() {
    // Wait 1 us for the signal to stabilize
    esp_rom_delay_us(1);
}

inline void matrix_output_unselect_delay() {
    // Wait 30 us
    esp_rom_delay_us(30);
}

//Returns the value of the ith bit in the integer
uint8_t get_bit(uint8_t value, uint8_t i) {
    return (value >> i) & 0x01;
}

void matrix_print() {
    ESP_LOGI(TAG, "-----------------------------------------------");

    for (int i = 0; i < MATRIX_ROWS; i++) {
        char row_str[2 * MATRIX_COLS + 1] = {0};  // space + char + null terminator
        char *p = row_str;

        for (int j = 0; j < MATRIX_COLS; j++) {
            *p++ = get_bit(matrix[i], j) ? '1' : '0';
            *p++ = ' ';
        }

        *p = '\0';  // null-terminate the string
        ESP_LOGI(TAG, "%s", row_str);
    }
}

// matrix code

static void select_row(uint8_t row) {
    gpio_num_t pin = row_pins[row];
    gpio_atomic_set_pin_output_low(pin);
}

static void unselect_row(uint8_t row) {
    gpio_num_t pin = row_pins[row];
    gpio_atomic_set_pin_output_high(pin);
}

static void unselect_rows(void) {
    for (uint8_t x = 0; x < MATRIX_ROWS; x++) {
        unselect_row(x);
    }
}

/*
    The initial state of the keyboard sets columns to PULL_UP and INPUT, and rows to OUTPUT and HIGH state.
    We will consider a button clicked when the row is set to LOW and the column reads LOW.
*/
void matrix_init_pins(void) {
    unselect_rows();
    for (uint8_t x = 0; x < MATRIX_COLS; x++) {
        gpio_atomic_set_pin_input_high(col_pins[x]);
    }
}

void matrix_read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Start with a clear matrix row
    matrix_row_t current_row_value = 0;

    select_row(current_row);
    matrix_output_select_delay();

    // For each col...
    matrix_row_t row_shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++, row_shifter <<= 1) {
        uint8_t pin_state = readMatrixPin(col_pins[col_index]);

        // Populate the matrix row with the state of the col pin
        current_row_value |= pin_state ? 0 : row_shifter;
    }

    // Unselect row
    unselect_row(current_row);
    matrix_output_unselect_delay(); // wait for all col signals to go HIGH

    // Update the matrix
    current_matrix[current_row] = current_row_value;
}

void matrix_init(void) {
    // initialize key pins
    matrix_init_pins();

    // initialize matrix state: all keys off
    memset(matrix, 0, sizeof(matrix));
    memset(raw_matrix, 0, sizeof(raw_matrix));
}

/*
    To do a scan we read the values from the keyboard, check that at least one key has changed state
    and in that case we check that it is a real change of state and not the result of a debounce.
    
    For speed we are not interested in retrieving which keys are actually pressed but only that the state has changed, this allows us
    to worry about figuring out what the positions changed only if necessary and to use more efficient methods of comparison such as memcmp()
*/
uint8_t matrix_scan(void) {
    matrix_row_t curr_matrix[MATRIX_ROWS] = {0};

    // Set row, read cols
    for (uint8_t current_row = 0; current_row < MATRIX_ROWS; current_row++) {
        matrix_read_cols_on_row(curr_matrix, current_row);
    }

    bool changed = memcmp(raw_matrix, curr_matrix, sizeof(curr_matrix)) != 0;
    if (changed) memcpy(raw_matrix, curr_matrix, sizeof(curr_matrix));

    changed = debounce(raw_matrix, matrix, MATRIX_ROWS, changed);

    return (uint8_t)changed;
}

matrix_row_t matrix_get_row(uint8_t row) {
    return matrix[row];
}