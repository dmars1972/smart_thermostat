#ifndef _RECT
#define _RECT

#include "Arduino.h"

#include <functional>

class Rect
{
  protected:
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;

  public:
    typedef std::function<void(void)> THandler;

    Rect();
    Rect(uint16_t, uint16_t, uint16_t, uint16_t);

    uint16_t getX() {return x;};
    uint16_t getY() {return y;};
    uint16_t getWidth() {return w;};
    uint16_t getHeight() {return h;};
    void onTouch(THandler);

    bool touched(uint16_t, uint16_t);

  private:
    THandler _onTouch;
};

#endif
