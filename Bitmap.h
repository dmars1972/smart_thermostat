#ifndef _BITMAP
#define _BITMAP

#include "Arduino.h"
#include <SD.h>

class Bitmap
{
  private:
    unsigned char imageOffset;

    int width;
    int height;
    int rowSize;

    uint16_t read16(File);
    uint32_t read32(File);
  public:
    Bitmap();
    Bitmap(uint8_t, uint8_t);
    bool readHeader(File);
    int getRowsize() { return rowSize; };
    bool get(File, int, uint8_t *);
};

#endif
