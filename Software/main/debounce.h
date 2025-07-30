#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "matrix.h"

bool debounce(matrix_row_t raw[], matrix_row_t cooked[], uint8_t num_rows, bool changed);