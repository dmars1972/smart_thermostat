#ifndef _KEYMAP
#define _KEYMAP

#define ROW_1 (1<<4)
#define ROW_2 (2<<4)
#define ROW_3 (3<<4)
#define ROW_4 (4<<4)

#define COL_1 1
#define COL_2 2
#define COL_3 3
#define COL_4 4
#define COL_5 5
#define COL_6 6
#define COL_7 7
#define COL_8 8
#define COL_9 9
#define COL_10 10

enum KeyMap {
  _softkey_q = ROW_1 | COL_1,
  _softkey_w = ROW_1 | COL_2,
  _softkey_e = ROW_1 | COL_3,
  _softkey_r = ROW_1 | COL_4,
  _softkey_t = ROW_1 | COL_5,
  _softkey_y = ROW_1 | COL_6,
  _softkey_u = ROW_1 | COL_7,
  _softkey_i = ROW_1 | COL_8,
  _softkey_o = ROW_1 | COL_9,
  _softkey_p = ROW_1 | COL_10,

  _softkey_a = ROW_2 | COL_1,
  _softkey_s = ROW_2 | COL_2,
  _softkey_d = ROW_2 | COL_3,
  _softkey_f = ROW_2 | COL_4,
  _softkey_g = ROW_2 | COL_5,
  _softkey_h = ROW_2 | COL_6,
  _softkey_j = ROW_2 | COL_7,
  _softkey_k = ROW_2 | COL_8,
  _softkey_l = ROW_2 | COL_9,

  _softkey_z = ROW_3 | COL_2,
  _softkey_x = ROW_3 | COL_3,
  _softkey_c = ROW_3 | COL_4,
  _softkey_v = ROW_3 | COL_5,
  _softkey_b = ROW_3 | COL_6,
  _softkey_n = ROW_3 | COL_7,
  _softkey_m = ROW_3 | COL_8,

  _softkey_bspc = ROW_2 | COL_10,
  _softkey_entr = ROW_3 | COL_9,
  _softkey_spc = ROW_4 | COL_3,
  _softkey_shift = ROW_3 | COL_1,
  _softkey_numpad = ROW_4 | COL_1
};

#endif
