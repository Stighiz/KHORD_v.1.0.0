#pragma once

#include <stdint.h>
#include <stdbool.h>

/* diode directions */
#define COL2ROW 0

typedef uint8_t pin_t;
typedef uint16_t matrix_row_t;

#define MATRIX_ROW_SHIFTER ((matrix_row_t)1)

/* number of matrix rows */
uint8_t matrix_rows(void);
/* number of matrix columns */
uint8_t matrix_cols(void);
/* intialize matrix for scaning. */
void matrix_init(void);
/* scan all key states on matrix */
uint8_t matrix_scan(void);
/* matrix state on row */
matrix_row_t matrix_get_row(uint8_t row);
/* print matrix for debug */
void matrix_print(void);
/* delay between changing matrix pin state and reading values */
void matrix_output_select_delay(void);
void matrix_output_unselect_delay(void);