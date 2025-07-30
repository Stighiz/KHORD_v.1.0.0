#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "matrix.h"
#include "debounce.h"
#include "config.h"
#include "timer.h"

static bool debouncing = false;
static timer_t debouncing_time;

/*
    The algorithm implemented for debouncing is a filter on the changed field.
    If this is the first change it saves the timestamp and flags the start of the debouncing timer.
    If another change arrives in the debouncing window it is filtered out and not propagated.
 */
bool debounce(matrix_row_t raw[], matrix_row_t filtered[], uint8_t num_rows, bool changed) {
    bool filtered_changed = false;

    if (changed) {
        debouncing = true;
        debouncing_time = millis_elapsed();
    } else if (debouncing && TIMER_DIFF_32(millis_elapsed(), debouncing_time) >= DEBOUNCE) {
        size_t matrix_size = num_rows * sizeof(matrix_row_t);
        if (memcmp(filtered, raw, matrix_size) != 0) {
            memcpy(filtered, raw, matrix_size);
            filtered_changed = true;
        }
        debouncing = false;
    }

    return filtered_changed;
}