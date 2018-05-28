#include "TextRect.h"

TextRect::TextRect() 
{
  x = 0;
  y = 0;
  h = 0;
  w = 0;

  alignment = LEFT;
  return;
}

TextRect::TextRect(uint16_t xpos, uint16_t ypos, uint16_t height, uint16_t width)
{
  x = xpos;
  y = ypos;
  h = height;
  w = width;
  memset(text, '\0', sizeof(text));

  alignment = LEFT;
  return;
}

void TextRect::clear(Adafruit_ILI9341 *tft)
{
  tft->fillRect(x, y-h, w, h*2, backgroundColor);

  return;
}

bool TextRect::setText(Adafruit_ILI9341 *tft, char *t)
{
  if(!strcmp(t, text))
    return true;

  strcpy(text, t);

  refresh(tft);

  return true;
}

void TextRect::setButton(Adafruit_ILI9341 *tft, char *t, uint16_t borderColor, uint16_t fillColor, uint16_t textColor)
{
  uint16_t x1;

  strcpy(text, t);

  tft->setTextWrap(false);
  switch(fontSize) {
    case 12:
        tft->setFont(&FreeSans12pt7b);
        break;
    case 18:
        tft->setFont(&FreeSans18pt7b);
        break;
    case 24: 
        tft->setFont(&FreeSans24pt7b);
        break;
    default: 
        tft->setFont(&FreeSans12pt7b);
        break;
  }

  tft->setTextColor(textColor);
  tft->getTextBounds(text, x, y, &tb_x, &tb_y, &tb_w, &tb_h);
  tft->fillRoundRect(x, y, w, h, 7, fillColor);
  tft->drawRoundRect(x, y, w, h, 7, borderColor);

  x1 = x + (w/2) - (tb_w / 2) -1;
  tft->setCursor(x1, y+(h/2)+(tb_h/2));
  tft->print(text);

  return;
}

void TextRect::refresh(Adafruit_ILI9341 *tft)
{
  uint16_t x1;

  tft->setTextWrap(false);
  tft->fillRect(x, y-h, w, h*2, backgroundColor);

  switch(fontSize) {
    case 12: tft->setFont(&FreeSans12pt7b);
             break;
    case 18: tft->setFont(&FreeSans18pt7b);
             break;
    case 24: tft->setFont(&FreeSans24pt7b);
             break;
    default: tft->setFont(&FreeSans12pt7b);
             break;
  }

  tft->setTextColor(textColor);
  tft->getTextBounds(text, x, y, &tb_x, &tb_y, &tb_w, &tb_h);

  switch(alignment) {
    case LEFT:
      x1 = x;
      break;
    case CENTER:
      x1 = x + (w/2) - (tb_w / 2);
      break;
    case RIGHT:
      x1 = x + w - tb_w - 10;
      break;
  }

  tft->setCursor(x1, y+(h/2));
  tft->print(text);
}
