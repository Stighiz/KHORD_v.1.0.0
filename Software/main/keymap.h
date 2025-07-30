#pragma once

/*
    GeminiPR encodes 42 keys into a 6-byte packet. Contains everything that is necessary for standard stenography and
    opens up many more options, including differentiating between top and bottom S-, and supporting non-English theories.
*/

//GeminiPR Keycode Reference
enum steno_keycodes {
    KC_NO    = 0x0000,
    STNMIN   = 0x74C0,
    STN_FN   = STNMIN,
    STN_NUM,
    STN_N1   = STN_NUM,
    STN_N2,
    STN_N3,
    STN_N4,
    STN_N5,
    STN_N6,
    STN_SL,
    STN_S1   = STN_SL,
    STN_S2,
    STN_TL,
    STN_KL,
    STN_PL,
    STN_WL,
    STN_HL,
    STN_RL,
    STN_A,
    STN_O,
    STN_STR,
    STN_ST1  = STN_STR,
    STN_ST2,
    STN_RES1,
    STN_RE1  = STN_RES1,
    STN_RES2,
    STN_RE2  = STN_RES2,
    STN_PWR,
    STN_ST3,
    STN_ST4,
    STN_E,
    STN_U,
    STN_FR,
    STN_RR,
    STN_PR,
    STN_BR,
    STN_LR,
    STN_GR,
    STN_TR,
    STN_SR,
    STN_DR,
    STN_N7,
    STN_N8,
    STN_N9,
    STN_NA,
    STN_NB,
    STN_NC,
    STN_ZR,
    STNMAX   = STN_ZR,
};

const extern uint16_t keymaps[MATRIX_ROWS][MATRIX_COLS];