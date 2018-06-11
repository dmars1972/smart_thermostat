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
    uint16_t borderColor;
    char borderRadius;
    int16_t tb_x, tb_y;
    uint16_t tb_w, tb_h;

    char *text;
    unsigned char fontSize;
    TextAlignment alignment;
    HAlignment h_alignment;

    bool hasBorder;
    bool isButton;
    uint16_t buttonFillColor;
    uint16_t buttonBorderColor;
    uint16_t buttonTextColor;
  public:
    TextRect(uint16_t, uint16_t, uint16_t, uint16_t);
    TextRect(uint16_t, uint16_t, uint16_t, uint16_t, char *);
    ~TextRect();

    void setBackgroundColor(uint16_t c) { backgroundColor = c; };
    void setTextColor(uint16_t c) { textColor = c; };
    void setAlignment(TextAlignment a) { alignment = a; return; };
    void setHorizontalAlignment(HAlignment a) { h_alignment = a; return; };
    void setBorder(uint16_t, char br = 3);
    bool setText(Adafruit_ILI9341 *, const char *);
    void setFontSize(unsigned char fs) {fontSize =fs; return; };
    void setButton(Adafruit_ILI9341 *, char *, uint16_t, uint16_t, uint16_t);
    void clear(Adafruit_ILI9341 *);

    char * getText() { Serial.println(text); return text; };
    void refresh(Adafruit_ILI9341 *);
};

#endif
