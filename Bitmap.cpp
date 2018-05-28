#include "Bitmap.h"

// This was almost entirely plagarized from the Adafruit tftbmp example

Bitmap::Bitmap()
{
}

Bitmap::Bitmap(uint8_t w, uint8_t h)
{
  width = w;
  height = h;

  rowSize = (w * 3 + 3) & ~3;

  return;
}

bool Bitmap::readHeader(File f)
{
  // read header
  uint32_t tmp;
  uint8_t bmpDepth;
    
  if (read16(f) != 0x4D42) {
      // magic bytes missing
      return false;
  }

  // read file size
  tmp = read32(f);

  // read and ignore creator bytes
  read32(f);

  imageOffset = read32(f);

  // read DIB header
  tmp = read32(f);

  int bmp_width = read32(f);
  int bmp_height = read32(f);
    
  if (read16(f) != 1)
    return false;

  bmpDepth = read16(f);

  if (read32(f) != 0) {
      // compression not supported!
      return false;
  }

  return true;
}

bool Bitmap::get(File f, int l, uint8_t *buf)
{
  int x;

  if(!f.seek(imageOffset + (l * rowSize))) {
    Serial.print("Failed to seek to ");
    Serial.println(imageOffset + (l * rowSize));
    return false;
  }

  x = f.read(buf, rowSize);

  if(f.read(buf, rowSize) == -1) {
    Serial.print("Failed to read");
    return false;
  }

  return true;
}

/*********************************************/
// These read data from the SD card file and convert them to big endian
// (the data is stored in little endian format!)

// LITTLE ENDIAN!
uint16_t Bitmap::read16(File f)
{
    uint16_t d;
    uint8_t b;
    b = f.read();
    d = f.read();
    d <<= 8;
    d |= b;
    return d;
}

// LITTLE ENDIAN!
uint32_t Bitmap::read32(File f)
{
    uint32_t d;
    uint16_t b;

    b = read16(f);
    d = read16(f);
    d <<= 16;
    d |= b;
    return d;
}
