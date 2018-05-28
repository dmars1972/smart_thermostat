#include "ImageRect.h"

ImageRect::ImageRect()
{
  x = 0;
  y = 0;
  w = 0;
  h = 0;

  bufferSize = (w * 3 + 3) & ~3;
  bitmap = Bitmap(w, h);

  return;
}

ImageRect::ImageRect(uint16_t xpos, uint16_t ypos, uint16_t width, uint16_t height)
{
  x = xpos;
  y = ypos;
  w = width;
  h = height;

  bufferSize = (w * 3 + 3) & ~3;

  bitmap = Bitmap(w, h);

  return;
}

bool ImageRect::setImage(Adafruit_ILI9341 *tft, char *bmp)
{
  memset(bmpFilename, '\0', sizeof(bmpFilename));

  strcpy(bmpFilename, bmp);

  refresh(tft);

  return true;
}

void ImageRect::refresh(Adafruit_ILI9341 *tft)
{
  uint8_t *pixelBuffer;
  int line;
  uint8_t r, g, b;
  int y1;
  File bmpFile;

  bmpFile = SD.open(bmpFilename, FILE_READ);

  if(!bmpFile) {
    Serial.print("Couldn't find ");
    Serial.println(bmpFilename);;
    return;
  }

  if(!bitmap.readHeader(bmpFile)) {
    Serial.println("Header not ok");
    bmpFile.close();
    return;
  }

  if(bufferSize == 0) {
    Serial.println("Didn't set a buffer size");
    return;
  }

  pixelBuffer = (uint8_t *) malloc (bitmap.getRowsize());

  if(!pixelBuffer) {
    Serial.println("malloc failed");
    return;
  } 

  for(line = 0; line < h; line++) {
    if(!bitmap.get(bmpFile, line, pixelBuffer)) {
      Serial.print("Couldn't read line ");
      Serial.println(line, DEC);

      free(pixelBuffer);
      bmpFile.close();
      return;
    }

    for(int pix = 0, y1 = y; pix < w * 3; pix += 3, ++y1) {
      b = pixelBuffer[pix];
      g = pixelBuffer[pix+1];
      r = pixelBuffer[pix+2];

      if(b == 0 && g == 211 && r == 0)
        tft->drawPixel(x + line, y+y1, backgroundColor);
      else
        tft->drawPixel(x + line, y+y1, tft->color565(r,g,b));

      yield();
    }

  }

  free(pixelBuffer);

  bmpFile.close();

  return;
}
