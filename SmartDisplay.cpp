#include "SmartDisplay.h"

SmartDisplay::SmartDisplay(Settings *settings)
{
  s = settings;

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  card.init(SPI_FULL_SPEED, SD_CS); 
    
  if(!SD.begin(SD_CS)) {
    Serial.println("SD card failure!");
    while(1);
  }

  weatherIcon = ImageRect(10, 10, 50, 50);

  statusText = TextRect(0, 240-24, 24, 320);

  outsideTempText = TextRect(10, 85, 18, 18*3);

  timeText = TextRect(70, 40, 24, 180);

  setTempText = TextRect(300 - (26*3), 240-34, 24, 26*3);
  
  return;
}

bool SmartDisplay::begin()
{
  if(!spitouch.begin()) {
    Serial.println("STMPE not found.");
    return false;
  }

/*
  spitouch.writeRegister8(STMPE_GPIO_DIR, _BV(2));
  spitouch.writeRegister8(STMPE_GPIO_ALT_FUNCT, _BV(2));
*/

  spitouch.begin();

  // backlight on
  spitouch.writeRegister8(STMPE_GPIO_SET_PIN, _BV(2));

  tft.begin();
  tft.setTextWrap(false);
  tft.fillScreen(ILI9341_BLACK);

  tft.setRotation(3);

  statusText.setTextColor(ILI9341_CYAN);
  statusText.setBackgroundColor(backgroundColor);
  statusText.setFontSize((unsigned char)12);
  statusText.setText(&tft, "it worked!");

  outsideTempText.setTextColor(ILI9341_CYAN);
  outsideTempText.setAlignment(CENTER);
  outsideTempText.setBackgroundColor(backgroundColor);
  outsideTempText.setFontSize((unsigned char)18);

  timeText.setTextColor(ILI9341_CYAN);
  timeText.setBackgroundColor(backgroundColor);
  timeText.setFontSize((unsigned char)24);
  timeText.setAlignment(CENTER);

  setTempText.setTextColor(ILI9341_CYAN);
  setTempText.setBackgroundColor(backgroundColor);
  setTempText.setFontSize((unsigned char)24);
  setTempText.setAlignment(RIGHT);

  weatherIcon.setBackgroundColor(backgroundColor);

  return true;
}

void SmartDisplay::setTextColor(uint16_t c)
{
  textColor = c;

  tft.setTextColor(textColor, backgroundColor);

  return;
}

void SmartDisplay::setBackgroundColor(uint16_t c)
{
  backgroundColor = c;

  tft.setTextColor(textColor, backgroundColor);

  return;
}

void SmartDisplay::setWeatherIcon(char *i)
{
  sprintf(imagefile, "%s.bmp", i);

  weatherIcon.setImage(&tft, imagefile);

  return;
}

void SmartDisplay::setOutsideTemp(char *t)
{
  outsideTempText.setText(&tft, t);

  return;
}

void SmartDisplay::setSetTemp(unsigned char t)
{
  char temp[4];

  sprintf(temp, "%d", t);

  setTempText.setText(&tft, temp);

  return;
}

void SmartDisplay::setStatus(char *t)
{
  statusText.setText(&tft, t);

  return;
}

void SmartDisplay::clearStatus()
{
  statusText.clear(&tft);

  return;
}

void SmartDisplay::setTime(char *t)
{
  timeText.setText(&tft, t);

  return;
}

void SmartDisplay::handleTouch()
{
  TS_Point p;
  int touchx;
  int touchy;

  if (!spitouch.bufferEmpty()) {
    // read x & y & z;
    p = spitouch.getPoint();

    touchx = map(p.y, TS_MAXY, TS_MINY, 0, tft.width()); 
    touchy = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());

    // clear the buffer
    do {
      spitouch.getPoint();
    } while(!spitouch.bufferEmpty());

    if(weatherIcon.touched(touchx, touchy))
      weatherIconTouched();

    if(outsideTempText.touched(touchx, touchy))
      weatherIconTouched();

    if(timeText.touched(touchx, touchy))
      timeTouched();

    if(setTempText.touched(touchx, touchy))
      setTempTouched();
  }

  return;
}

void SmartDisplay::weatherIconTouched()
{
  Serial.println("Weather Icon callback");
} 

void SmartDisplay::timeTouched()
{
  Serial.println("Time callback");
}

void SmartDisplay::setTempTouched()
{
  TS_Point p;
  uint16_t touchx, touchy;
  bool OK = false;
  unsigned char setTemp = s->getCurrentSetTemp();
  char t[4];

  sprintf(t, "%d", setTemp);

  tft.fillScreen(backgroundColor);

  TextRect currentBox(110, 78, 50, 70);
  TextRect downBox(50, 63, 50, 50);
  TextRect upBox(195, 63, 50, 50);
  TextRect okBox(50, 153, 50, 75);
  TextRect cancelBox(245-75, 153, 50, 75);

  currentBox.setBackgroundColor(backgroundColor);
  currentBox.setFontSize((unsigned char)18);
  currentBox.setTextColor(ILI9341_CYAN);
  currentBox.setAlignment(CENTER);
  currentBox.setText(&tft, t);

  upBox.setBackgroundColor(backgroundColor);
  upBox.setFontSize((unsigned char)18);
  upBox.setTextColor(ILI9341_BLACK);
  upBox.setAlignment(CENTER);
  upBox.setButton(&tft, ">", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  downBox.setBackgroundColor(backgroundColor);
  downBox.setFontSize((unsigned char)18);
  downBox.setTextColor(ILI9341_BLACK);
  downBox.setAlignment(CENTER);
  downBox.setButton(&tft, "<", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  okBox.setBackgroundColor(backgroundColor);
  okBox.setFontSize((unsigned char)12);
  okBox.setTextColor(ILI9341_BLACK);
  okBox.setAlignment(CENTER);
  okBox.setButton(&tft, "OK", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

  cancelBox.setBackgroundColor(backgroundColor);
  cancelBox.setFontSize((unsigned char)12);
  cancelBox.setTextColor(ILI9341_BLACK);
  cancelBox.setAlignment(CENTER);
  cancelBox.setButton(&tft, "Cancel", ILI9341_CYAN, ILI9341_DARKCYAN, ILI9341_BLACK);

// empty the buffer
  do {
    spitouch.getPoint();
  } while(!spitouch.bufferEmpty());

  do {
    if (!spitouch.bufferEmpty()) {
      p = spitouch.getPoint();

      touchx = map(p.y, TS_MAXY, TS_MINY, 0, tft.width()); 
      touchy = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());

      if(upBox.touched(touchx, touchy)) {
        setTemp++;
        sprintf(t, "%d", setTemp);
        currentBox.setText(&tft, t);
      } else if(downBox.touched(touchx, touchy)) {
        setTemp--;
        sprintf(t, "%d", setTemp);
        currentBox.setText(&tft, t);
      } else if(okBox.touched(touchx, touchy)) {
        OK = true;
        break;
      } else if(cancelBox.touched(touchx, touchy)) {
        OK = false;
        break;
      }

      while(!spitouch.bufferEmpty())
        spitouch.getPoint();
    } else 
      delay(100);


  } while(true);

  if(OK)
    s->setCurrentSetTemp(setTemp);

  refreshScreen();
}

void SmartDisplay::refreshScreen()
{
  tft.fillScreen(backgroundColor);

  weatherIcon.refresh(&tft);
  
  outsideTempText.refresh(&tft);

  timeText.refresh(&tft);

  setTempText.refresh(&tft);
}
