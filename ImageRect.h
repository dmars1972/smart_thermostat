#ifndef _IMAGE_RECT
#define _IMAGE_RECT

#include "Rect.h"
#include "Bitmap.h"
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>


class ImageRect : public Rect, Bitmap
{
  private:
    Bitmap bitmap;

    char bmpFilename[20];

    int bufferSize;
    uint16_t backgroundColor;

  public:
    ImageRect(uint16_t, uint16_t, uint16_t, uint16_t);
    ImageRect(uint16_t, uint16_t, uint16_t, uint16_t, char *);

    void setBackgroundColor(uint16_t c) { backgroundColor = c; };
    bool setImage(Adafruit_ILI9341 *, char *);
    void refresh(Adafruit_ILI9341 *);
};

#endif
