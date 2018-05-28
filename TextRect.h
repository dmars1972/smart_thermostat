#ifndef _TEXT_RECT
#define _TEXT_RECT

#include "Arduino.h"
#include "Rect.h"
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include "types.h"
#include "fonts.h"

class TextRect : public Rect
{
  private:
    uint16_t backgroundColor;
    uint16_t textColor;
    int16_t tb_x, tb_y;
    uint16_t tb_w, tb_h;

    char text[320];
    unsigned char fontSize;
    TextAlignment alignment;
  public:
    TextRect();
    TextRect(uint16_t, uint16_t, uint16_t, uint16_t);
    TextRect(uint16_t, uint16_t, uint16_t, uint16_t, char *);

    void setBackgroundColor(uint16_t c) { backgroundColor = c; };
    void setTextColor(uint16_t c) { textColor = c; };
    void setAlignment(TextAlignment a) { alignment = a; return; };
    bool setText(Adafruit_ILI9341 *, char *);
    void setFontSize(unsigned char fs) {fontSize =fs; return; };
    void setButton(Adafruit_ILI9341 *, char *, uint16_t, uint16_t, uint16_t);
    void clear(Adafruit_ILI9341 *);

    void refresh(Adafruit_ILI9341 *);
};

#endif
