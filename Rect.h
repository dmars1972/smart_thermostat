#ifndef _RECT
#define _RECT

#include "Arduino.h"

class Rect
{
  protected:
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;

  public:
    Rect();
    Rect(uint16_t, uint16_t, uint16_t, uint16_t);

    bool touched(uint16_t, uint16_t);
};

#endif
