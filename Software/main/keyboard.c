#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "keyboard.h"
#include "matrix.h"
#include "config.h"
#include "keymap.h"
#include "usb.h"

//Per GEMINI protocol specifications
#define MAX_CHORD_SIZE 6

// All steno keys that have been pressed to form this chord,
// stored in MAX_CHORD_SIZE groups of 8-bit arrays.
static uint8_t chord[MAX_CHORD_SIZE] = {0};

// The number of physical keys actually being held down.
// This is not always equal to the number of 1 bits in `chord` because it is possible to
// simultaneously press down n keys, release dome of them and then press other keys and return 
// to n pressed keys but >n `chord` keys.
static int8_t pressed_keys = 0;

static inline void clear_chord(void) {
    memset(chord, 0, sizeof(chord));
}

/*
    Examples of steno strokes and the associated packet:

    EUBG    = 10000000 00000000 00000000 00001100 00101000 00000000
    WAZ     = 10000000 00000010 00100000 00000000 00000000 00000001
    PHAPBGS = 10000000 00000101 00100000 00000000 01101010 00000000
 */
bool add_key_to_chord(uint8_t key) {
    // Although each group of the packet is 8 bits long, the MSB is reserved
    // to indicate whether that byte is the first byte of the packet (MSB=1)
    // or one of the remaining five bytes of the packet (MSB=0).

    // As a consequence, only 7 out of the 8 bits are left to be used as a bit array
    // for the steno keys of that group. ()
    const int group_idx       = key / 7;
    const int intra_group_idx = key - group_idx * 7;
    // The 0th steno key of the group has bit=0b01000000, the 1st has bit=0b00100000, etc.
    const uint8_t bit = 1 << (6 - intra_group_idx);
    chord[group_idx] |= bit;
    return false;
}

void send_chord(void) {
    // Set MSB to 1 to indicate the start of packet: 0b10000000
    chord[0] |= 0x80; 
    usb_send(chord, sizeof(chord) / sizeof(chord[0]));
}

bool process_keycode(uint16_t keycode, keyevent_t event) {
    if (event.pressed) {
        pressed_keys++;
        add_key_to_chord(keycode - STNMIN);
    } else { // is released
        pressed_keys--;
        if (pressed_keys > 0) {
            // User hasn't released all keys yet, so the chord cannot be sent
            return false;
        }

        pressed_keys = 0;
        send_chord();
        clear_chord();
    }

    return false;
}

uint16_t get_keycode_from_keymap(uint8_t row, uint8_t column) {
    if (row < MATRIX_ROWS && column < MATRIX_COLS) {
        return keymaps[row][column];
    }
    return KC_NO;
}

void process_event(keyevent_t event) {
    uint16_t keycode = get_keycode_from_keymap(event.key.row, event.key.col);

    if (!process_keycode(keycode, event)) {
        return;
    }

    //Eventual post process logic
}

bool matrix_task(void) {
    static matrix_row_t matrix_previous[MATRIX_ROWS];

    matrix_scan();
    bool matrix_changed = false;
    for (uint8_t row = 0; row < MATRIX_ROWS && !matrix_changed; row++) {
        //Bitwise XOR to check whether a bit changed in the row
        matrix_changed |= matrix_previous[row] ^ matrix_get_row(row);
    }

    // Short-circuit the complete matrix processing if it is not necessary
    if (!matrix_changed) {
        return matrix_changed;
    }

    if (PRINT_DEBUG) {
        matrix_print();
    }

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        const matrix_row_t current_row = matrix_get_row(row);
        const matrix_row_t row_changes = current_row ^ matrix_previous[row];

        if (!row_changes) {
            continue;
        }

        matrix_row_t col_mask = 1;
        for (uint8_t col = 0; col < MATRIX_COLS; col++, col_mask <<= 1) {
            //If this column is part of the columns changed in the row
            if (row_changes & col_mask) {
                const bool key_pressed = current_row & col_mask;

                keypos_t key = { .row = row, .col = col };
                keyevent_t event = { .key = key, .timestamp = millis_elapsed(), .pressed = key_pressed };
                process_event(event);
            }
        }

        matrix_previous[row] = current_row;
    }

    return matrix_changed;
}