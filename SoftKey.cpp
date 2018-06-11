#include "SoftKey.h"

SoftKey::SoftKey(
    Adafruit_ILI9341 *tft,
    uint16_t xpos,  // x, y, w and h are absolute to the keyboard
    uint16_t ypos,
    uint16_t width, 
    uint16_t height,
    char key,
    uint8_t keyWidthMult)
{
  int16_t t_x, t_y;
  uint16_t t_w, t_h;

  keyValue = key;
  widthMult = keyWidthMult;

  switch (tolower(keyValue)) {
      case 'a':  
            keyMap = _softkey_a;
            altValue = '@';
            break;
      case 'b':  
            keyMap = _softkey_b;
            altValue = ';';
            break;
      case 'c':  
            keyMap = _softkey_c;
            altValue = '/';
            break;
      case 'd':  
            keyMap = _softkey_d;
            altValue = '$';
            break;
      case 'e':  
            keyMap = _softkey_e;
            altValue = '3';
            break;
      case 'f':  
            keyMap = _softkey_f;
            altValue = '_';
            break;
      case 'g':  
            keyMap = _softkey_g;
            altValue = '&';
            break;
      case 'h':  
            keyMap = _softkey_h;
            altValue = '-';
            break;
      case 'i':  
            keyMap = _softkey_i;
            altValue = '8';
            break;
      case 'j':  
            keyMap = _softkey_j;
            altValue = '+';
            break;
      case 'k':  
            keyMap = _softkey_k;
            altValue = '(';
            break;
      case 'l':  
            keyMap = _softkey_l;
            altValue = ')';
            break;
      case 'm':  
            keyMap = _softkey_m;
            altValue = '?';
            break;
      case 'n':  
            keyMap = _softkey_n;
            altValue = '!';
            break;
      case 'o':  
            keyMap = _softkey_o;
            altValue = '9';
            break;
      case 'p':  
            keyMap = _softkey_p;
            altValue = '0';
            break;
      case 'q':  
            keyMap = _softkey_q;
            altValue = '1';
            break;
      case 'r':  
            keyMap = _softkey_r;
            altValue = '4';
            break;
      case 's':  
            keyMap = _softkey_s;
            altValue = '#';
            break;
      case 't':  
            keyMap = _softkey_t;
            altValue = '5';
            break;
      case 'u':  
            keyMap = _softkey_u;
            altValue = '7';
            break;
      case 'v':  
            keyMap = _softkey_v;
            altValue = ':';
            break;
      case 'w':  
            keyMap = _softkey_w;
            altValue = '2';
            break;
      case 'x':  
            keyMap = _softkey_x;
            altValue = '"';
            break;
      case 'y':  
            keyMap = _softkey_y;
            altValue = '6';
            break;
      case 'z':
            keyMap = _softkey_z;
            altValue = '*';
            break;
      case _SOFTKEY_BSPC:
            keyMap = _softkey_bspc;
            break;
      case _SOFTKEY_NEWL:
            keyMap = _softkey_entr;
            break;
      case ' ':
            keyMap = _softkey_spc;
            break;
      case _SOFTKEY_SHFT:
            keyMap = _softkey_shift;
            break;
      case _SOFTKEY_NUMP:
            keyMap = _softkey_numpad;
            break;
  }

  w = (width / 10)-1;
  h = (height / 3)-1;

  x = xpos + ((getColumn(keyMap)-1) * (w+1));
  y = ypos + ((getRow(keyMap)-1) * (h+1));

  if(isupper(keyValue)) {
    isUpper = true;
  } else {
    isUpper = false;
  }

  isAlternate = false;

  keyValue = tolower(keyValue);
  upperValue = toupper(keyValue);

  text_x = x + (w-20);
  text_y = y + (h - 16);

  //w *= keyWidthMult;
}

bool SoftKey::touched(uint16_t tx, uint16_t ty)
{
  if(tx >= x && tx <= x+(w*widthMult) && ty >= y && ty <= y+h)
    return true;

  return false;
}
