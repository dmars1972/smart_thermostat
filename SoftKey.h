#ifndef _SOFTKEY
#define _SOFTKEY

#include <Adafruit_ILI9341.h>
#include <functional>
#include "KeyMap.h"

#define _SOFTKEY_BSPC 8
#define _SOFTKEY_NEWL 10
#define _SOFTKEY_SHFT 15
#define _SOFTKEY_NUMP 26

class SoftKey
{
  private:
// Coordinates for where the key is displayed on the screen
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint8_t widthMult;

// What letter and where should the letter be placed
    char keyValue;
    unsigned char getRow(KeyMap k) { return (int)k >> 4; };
    unsigned char getColumn(KeyMap k) { return (int)k - (((int)k >> 4) * 16); };

    uint16_t text_x;
    uint16_t text_y;

    bool isUpper;
    char upperValue;
    bool isAlternate;
    char altValue;

    KeyMap keyMap;

  public:
    SoftKey() {};

    SoftKey(Adafruit_ILI9341 *, uint16_t, uint16_t, uint16_t, uint16_t, char, uint8_t = 1);

// getters
    uint16_t getX() { return x; };
    uint16_t getY() { return y; };
    uint16_t getHeight() { return h; };
    uint16_t getWidth() { return w * widthMult + widthMult - 1; };
    uint16_t getTextX() { return text_x; };
    uint16_t getTextY() { return text_y; };
    char getKeyValue() { return isAlternate ? altValue : (isUpper ? upperValue : keyValue) ; };

// setters
    void setUpperValue(unsigned char c) { upperValue = c; };
    void setCase() { isUpper = !isUpper ; };
    void setAlt() { isAlternate = !isAlternate ; };

    bool touched(uint16_t, uint16_t);

};

#endif
