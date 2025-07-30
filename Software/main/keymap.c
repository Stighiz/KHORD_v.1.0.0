#include "config.h"
#include "keymap.h"

const uint16_t keymaps[MATRIX_ROWS][MATRIX_COLS] = {
    { STN_S1, STN_TL, STN_PL, STN_HL, STN_ST1, STN_ST3, STN_FR, STN_PR, STN_LR, STN_TR, STN_DR },
    { STN_S2, STN_KL, STN_WL, STN_RL, STN_ST2, STN_ST4, STN_RR, STN_BR, STN_GR, STN_SR, STN_ZR },
    { STN_NA, KC_NO , STN_A , KC_NO , STN_O  , STN_E  , KC_NO , STN_U,  STN_NB, KC_NO , KC_NO  }
};