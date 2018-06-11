#include "SoftKeyboard.h"

SoftKeyboard::SoftKeyboard(
    Adafruit_ILI9341 *_tft, 
    Adafruit_STMPE610 *_spitouch,
    uint16_t xpos, 
    uint16_t ypos, 
    uint16_t width, 
    uint16_t height, 
    uint16_t touch_minx, 
    uint16_t touch_maxx, 
    uint16_t touch_miny, 
    uint16_t touch_maxy,
    String &retstr)
{
  unsigned char letter;
  int index;
  char str[64];
  uint8_t bgColor = ILI9341_CYAN;

  memset(str, '\0', sizeof(str));

  strcpy(str, retstr.c_str());

  x = xpos;
  y = ypos;
  w = width;
  h = height;
  minX = touch_minx;
  maxX = touch_maxx;
  minY = touch_miny;
  maxY = touch_maxy;

  _tft->fillScreen(ILI9341_BLACK);

  _tft->fillRoundRect(x, y, w, 35, 3, ILI9341_BLACK);
  _tft->drawRoundRect(x, y, w, 35, 3, ILI9341_CYAN);

  _tft->setTextColor(ILI9341_CYAN);
  _tft->setCursor(x+5, y+23);
  _tft->print(str);

  for(letter = 'a', index = 0; letter <= 'z'; ++letter, ++index)
    keys[index] = SoftKey(_tft, x, y+40, w, h, letter);

  keys[26] = SoftKey(_tft, x, y+40, w, h, ' ', 6);
  keys[27] = SoftKey(_tft, x, y+40, w, h, _SOFTKEY_BSPC);
  keys[28] = SoftKey(_tft, x, y+40, w, h, _SOFTKEY_NEWL, 2);
  keys[29] = SoftKey(_tft, x, y+40, w, h, _SOFTKEY_SHFT);
  keys[30] = SoftKey(_tft, x, y+40, w, h, _SOFTKEY_NUMP, 2);

  for(letter = 0; letter < 27; letter++)
    drawKey(_tft, keys[letter]);

  for(letter = 27; letter < 31; letter++) {
    drawBlankKey(_tft, keys[letter]);

    switch(keys[letter].getKeyValue()) {
        case ' ':
            break;
        case _SOFTKEY_BSPC:
            drawBackspace(_tft, keys[letter]);
            break;
        case _SOFTKEY_NEWL:
            drawEnter(_tft, keys[letter]);
            break;
        case _SOFTKEY_SHFT:
            drawShift(_tft, keys[letter]);
            break;
        case _SOFTKEY_NUMP:
            int16_t tb_x, tb_y;
            uint16_t tb_w, tb_h;
            int16_t x1, y1;
            char t123[4] = "123";

            _tft->getTextBounds(t123, keys[letter].getX(), keys[letter].getY(), &tb_x, &tb_y, &tb_w, &tb_h);
             x1 = keys[letter].getX() + (keys[letter].getWidth()/2) - (tb_w/2);
             y1 = keys[letter].getY() + (keys[letter].getHeight()/2) + (tb_h/2);
             _tft->setCursor(x1, y1);
             _tft->print(t123);
            break;
    }
  }

  clearBuffer(_spitouch);

  while(true) {
    TS_Point touch_p;
    uint16_t touchx, touchy;

    if (!_spitouch->bufferEmpty()) {
      bool gotChar = false;
      touch_p = _spitouch->getPoint();

      touchx = map(touch_p.y, touch_minx, touch_maxx, 0, 320);

      touchy = map(touch_p.x, touch_miny, touch_maxy, 0, 240);

      for(index = 0; index < 31; ++index) {
        if(keys[index].touched(touchx, touchy)) {
          switch (keys[index].getKeyValue()) {
            case _SOFTKEY_BSPC:
              if(strlen(str) > 0)
                str[strlen(str)-1] = '\0';
                
              _tft->fillRoundRect(x, y, w, 35, 3, ILI9341_BLACK);
              _tft->drawRoundRect(x, y, w, 35, 3, ILI9341_CYAN);
              break;
            case _SOFTKEY_SHFT:
              for ( int z = 0; z < 26; z++) {
                keys[z].setCase();

                drawKey(_tft, keys[z]);
              }
              break;
            case _SOFTKEY_NEWL:
              _tft->fillScreen(ILI9341_BLACK);
              retstr = String(str);
              return;
            case _SOFTKEY_NUMP:
               for(int z = 0; z < 26; z++) {
                 keys[z].setAlt();
                 drawKey(_tft, keys[z]);
               }
              break;
            default:
              str[strlen(str)] = keys[index].getKeyValue();
              break;
          }

          _tft->setTextColor(ILI9341_CYAN);
          _tft->setCursor(x+5, y+23);
          _tft->print(str);
          break;
        }
      }

      // clear the buffer

      clearBuffer(_spitouch);
    }

    yield();
  }

  clearBuffer(_spitouch);
}

void SoftKeyboard::drawBackspace(Adafruit_ILI9341 *__tft, SoftKey k)
{
  KeyPoints kpa[9];

  float w_incr = k.getWidth() / 10;
  float h_incr = k.getHeight() / 10;

// Top left
  kpa[0].x = k.getX() + (w_incr * 4);
  kpa[0].y = k.getY() + h_incr;

// Top Right
  kpa[1].x = k.getX() + (w_incr * 10);
  kpa[1].y = k.getY() + h_incr;

// Bottom left
  kpa[2].x = k.getX() + (w_incr * 4);
  kpa[2].y = k.getY() + (h_incr * 9);

// Bottom Right
  kpa[3].x = k.getX() + (w_incr * 10);
  kpa[3].y = k.getY() + (h_incr * 9);

// Back to top left
  kpa[4].x = kpa[0].x;
  kpa[4].y = kpa[0].y;

// left point
  kpa[5].x = k.getX() + (w_incr * 2);
  kpa[5].y = k.getY() + (h_incr * 5);

// Back to bottom left
  kpa[6].x = kpa[2].x;
  kpa[6].y = kpa[2].y;

// back to bottom right
  kpa[7].x = kpa[3].x;
  kpa[7].y = kpa[3].y;

// Finally, back to top right
  kpa[8].x = kpa[1].x;
  kpa[8].y = kpa[1].y;

  drawShape(__tft, kpa, 9);
}

void SoftKeyboard::drawEnter(Adafruit_ILI9341 *__tft, SoftKey k)
{
  KeyPoints kpa[10];
  float w_incr = k.getWidth() / 10;
  float h_incr = k.getHeight() / 10;
/***********************************************************

                             point 1,10  *********** point 2
                                   7,2   *         * 9,2
                                         *         *
                 point 7                 *         *
                 *   4,2                 *         *
                **                       *         *
               * *************************         *
              *  point 8 4,5            point 9    *
    point 6  *                          7,5        *
        2,5   *  point 4 4,7                       *
               * *********************************** point 3
                **                                   9,7
                 *
                 point 5 4,9
************************************************************/
  kpa[0].x = k.getX() + (w_incr * 7);
  kpa[0].y = k.getY() + (h_incr * 2);

  kpa[1].x = k.getX() + (w_incr * 9);
  kpa[1].y = k.getY() + (h_incr * 2);

  kpa[2].x = k.getX() + (w_incr * 9);
  kpa[2].y = k.getY() + (h_incr * 7);

  kpa[3].x = k.getX() + (w_incr * 4);
  kpa[3].y = k.getY() + (h_incr * 7);

  kpa[4].x = k.getX() + (w_incr * 4);
  kpa[4].y = k.getY() + (h_incr * 9);

  kpa[5].x = k.getX() + (w_incr * 2);
  kpa[5].y = k.getY() + (h_incr * 5.5);

  kpa[6].x = k.getX() + (w_incr * 4);
  kpa[6].y = k.getY() + (h_incr * 3);

  kpa[7].x = k.getX() + (w_incr * 4);
  kpa[7].y = k.getY() + (h_incr * 5);

  kpa[8].x = k.getX() + (w_incr * 7);
  kpa[8].y = k.getY() + (h_incr * 5);

  kpa[9].x = kpa[0].x;
  kpa[9].y = kpa[0].y;

  drawShape(__tft, kpa, 10);
}

void SoftKeyboard::drawShift(Adafruit_ILI9341 *__tft, SoftKey k)
{
  KeyPoints kpa[8];
  float w_incr = k.getWidth() / 10;
  float h_incr = k.getHeight() / 10;

/*****************************************************************

                               *  point 0 (5, 2)
                             *   *
                           *       *
                         *           *
                       *  point 5      *
       point 6(2,4)  ****** (3.5,4) ******  point 1 (8, 4)
                          *         * point 2 (6.5, 4)
                          *         *
                          *         *
                          *         *
          point 4 (3.5,8) *********** point 3 (6.5, 8)

*****************************************************************/
  kpa[0].x = k.getX() + (w_incr * 5);
  kpa[0].y = k.getY() + (h_incr * 2);
  kpa[1].x = k.getX() + (w_incr * 10);
  kpa[1].y = k.getY() + (h_incr * 4);
  kpa[2].x = k.getX() + (w_incr * 7);
  kpa[2].y = k.getY() + (h_incr * 4);
  kpa[3].x = k.getX() + (w_incr * 7);
  kpa[3].y = k.getY() + (h_incr * 8);
  kpa[4].x = k.getX() + (w_incr * 3);
  kpa[4].y = k.getY() + (h_incr * 8);
  kpa[5].x = k.getX() + (w_incr * 3);
  kpa[5].y = k.getY() + (h_incr * 4);
  kpa[6].x = k.getX() + (w_incr * 1);
  kpa[6].y = k.getY() + (h_incr * 4);
  kpa[7].x = kpa[0].x;
  kpa[7].y = kpa[0].y;

  drawShape(__tft, kpa, 8);
}

void SoftKeyboard::drawShape(Adafruit_ILI9341 *___tft, KeyPoints *k, int numPoints)
{
  for(int x = 0; x < numPoints-1; ++x) {
    ___tft->drawLine(k[x].x, k[x].y, k[x+1].x, k[x+1].y, ILI9341_BLACK);
  }
}

void SoftKeyboard::drawKey(Adafruit_ILI9341 *__tft, SoftKey k)
{
  __tft->fillRoundRect(
      k.getX(),
      k.getY(),
      k.getWidth(),
      k.getHeight(),
      3,
      ILI9341_CYAN);

  __tft->drawRoundRect(
      k.getX(),
      k.getY(),
      k.getWidth(),
      k.getHeight(),
      3,
      ILI9341_DARKCYAN);

  __tft->drawChar(
      k.getTextX(), 
      k.getTextY(), 
      k.getKeyValue(), 
      ILI9341_BLACK, 
      ILI9341_CYAN, 
      1);
}

void SoftKeyboard::drawBlankKey(Adafruit_ILI9341 *__tft, SoftKey k)
{
  __tft->fillRoundRect(
      k.getX(),
      k.getY(),
      k.getWidth(),
      k.getHeight(),
      3,
      ILI9341_CYAN);

  __tft->drawRoundRect(
      k.getX(),
      k.getY(),
      k.getWidth(),
      k.getHeight(),
      3,
      ILI9341_DARKCYAN);

}

void SoftKeyboard::clearBuffer(Adafruit_STMPE610 *_spitouch)
{
  delay(200);
  do {
    _spitouch->getPoint();
  } while(!_spitouch->bufferEmpty());
}
