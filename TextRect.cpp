#include "TextRect.h"

TextRect::TextRect(uint16_t xpos, uint16_t ypos, uint16_t height, uint16_t width)
{
  x = xpos;
  y = ypos;
  h = height;
  w = width;

  //memset(text, '\0', sizeof(text));

  text = (char *) malloc (1);
  text[0] = '\0';

  alignment = LEFT;
  h_alignment = H_TOP;
  hasBorder = false;
  isButton = false;
  return;
}

TextRect::~TextRect()
{
Serial.println("Freeing");

  free(text);
}

void TextRect::clear(Adafruit_ILI9341 *tft)
{
  tft->fillRect(x, y-h, w, h*2, backgroundColor);

  return;
}

bool TextRect::setText(Adafruit_ILI9341 *tft, const char *t)
{
  if(!strncmp(t, text, strlen(t)))
    return true;

  text = (char *) realloc (text, strlen(t)+1);

  if(!text) {
    Serial.println("Realloc failed");
    return false;
  }

  memset(text, '\0', strlen(t)+1);
  strcpy(text, t);

  refresh(tft);

  return true;
}

void TextRect::setButton(Adafruit_ILI9341 *tft, char *t, uint16_t borderColor, uint16_t fillColor, uint16_t textColor)
{
  uint16_t x1, y1;

  isButton = true;

  buttonBorderColor = borderColor;
  buttonFillColor = fillColor;
  buttonTextColor = textColor;

  if(strcmp(t, text)) {
    text = (char *) realloc(text, strlen(t)+1);
    memset(text, '\0', strlen(t)+1);

    strcpy(text, t);
  }

  alignment = CENTER;
  h_alignment = H_CENTER;

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

  tft->getTextBounds(text, x, y+h, &tb_x, &tb_y, &tb_w, &tb_h);

  switch(alignment) {
    case LEFT:
      x1 = tb_x;
      break;
    case CENTER:
      x1 = x + (w/2) - (tb_w/2);
      break;
    case RIGHT:
      x1 = tb_x + w - tb_w - 10;
      break;
  }

  switch(h_alignment) {
    case H_TOP:
      y1 = y+(tb_h-2);
      break;
    case H_CENTER:
      y1 = y + (h/2) + (tb_h/2);
      break;
    case H_BOTTOM:
      y1 = y+h-tb_h-1;
      break;
  } 

  tft->setTextColor(textColor);

  tft->fillRoundRect(x, y, w, h, 7, fillColor);
  tft->drawRoundRect(x, y, w, h, 7, borderColor);

  tft->setCursor(x1, y1);
  tft->print(text);

Serial.print("Button ");
Serial.println(text);
  return;
}

void TextRect::refresh(Adafruit_ILI9341 *tft)
{
  uint16_t x1, y1;

  if(isButton) {
    setButton(tft, text, buttonBorderColor, buttonFillColor, buttonTextColor);
    return;
  }

  tft->setTextWrap(false);
  tft->fillRect(x, y, w, h, backgroundColor);
  if(hasBorder) {
    tft->drawRoundRect(x, y, w, h, borderRadius, borderColor);
  }

  tft->setTextColor(textColor);

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

  tft->getTextBounds(text, x, y+h, &tb_x, &tb_y, &tb_w, &tb_h);

  switch(alignment) {
    case LEFT:
      x1 = tb_x;
      break;
    case CENTER:
      x1 = tb_x + (w/2) - (tb_w / 2);
      break;
    case RIGHT:
      x1 = tb_x + w - tb_w - 10;
      break;
  }

  switch(h_alignment) {
    case H_TOP:
      y1 = y+(tb_h-2);
      break;
    case H_CENTER:
      y1 = y + (h/2) + (tb_h/2);
      break;
    case H_BOTTOM:
      y1 = y+h-tb_h-1;
      break;
  } 

  tft->setCursor(x1, y1);
  tft->print(text);
}

void TextRect::setBorder(uint16_t bc, char br)
{
  hasBorder = true;
  borderColor = bc;
  borderRadius = br;

  return;
}
