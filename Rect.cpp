#include "Rect.h"

Rect::Rect() 
{
  x = 0;
  y = 0;
  w = 0;
  h = 0;
}

Rect::Rect(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height)
{
  x = xpos;
  y = ypos;
  w = width;
  h = height;

  return;
}

bool Rect::touched(uint16_t xpos, uint16_t ypos)
{
  if(xpos >= x && xpos <= x+w && ypos >= y && ypos <= y+h) {
    if(_onTouch) _onTouch();
    return true;
  }

  return false;
}

void Rect::onTouch(THandler fn)
{
  _onTouch = fn;
}
