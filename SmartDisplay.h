#ifndef _SMART_DISPLAY
#define _SMART_DISPLAY

#include "Arduino.h"
#include "Print.h"

#include "SmartRoom.h"
#include "SoftKeyboard.h"

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_STMPE610.h>
#include <SD.h>
#include "types.h"
#include "fonts.h"
#include "pins.h"

#include "Settings.h"
#include "Rect.h"
#include "ImageRect.h"
#include "TextRect.h"

#define TS_MINX 400
#define TS_MINY 400
#define TS_MAXX 3600
#define TS_MAXY 3600

const uint16_t WEATHER_ICON_X = 10;
const uint16_t WEATHER_ICON_Y = 10;
const uint16_t WEATHER_ICON_W = 50;
const uint16_t WEATHER_ICON_H = 50;

class SmartDisplay
{
  private:
    Settings *s;
    SmartRoom *rooms;

    std::vector<String> unknownVents;

    Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
    Adafruit_STMPE610 spitouch = Adafruit_STMPE610(STMPE_CS);

    uint16_t textColor;
    uint16_t backgroundColor;

    Sd2Card card;
    
    char imagefile[10];
    ImageRect weatherIcon;
    char currentIcon[10];

    bool hasNotification;

    ImageRect settingsIcon;
    TextRect outsideTempText;
    TextRect timeText;
    TextRect setTempText;
    TextRect setTempLabel;

    void weatherIconTouched();
    void timeTouched();
    void setTempTouched();
    void settingsTouched();
    void clearBuffer();
    void displayRooms();
    void displayRoom(SmartRoom);
    void addUnknownToRoom(SmartRoom r);
  public:
    SmartDisplay(Settings *, SmartRoom []);

    void setTextColor(uint16_t);
    void setBackgroundColor(uint16_t);
    void setWeatherIcon(char *);
    void setOutsideTemp(char *);
    void setTime(char *);
    void setSetTemp(unsigned char);
    void handleTouch();

    bool begin();

    void addUnknownVent(char *vent);

    void addNotification();

    void refreshScreen();

};
    
#endif
