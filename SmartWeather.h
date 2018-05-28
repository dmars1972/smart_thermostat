#ifndef _SMART_WEATHER
#define _SMART_WEATHER

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

enum TempUnits {
  F,
  C,
  K
};

class SmartWeather
{
  private:
    char openWeatherMapAPIKey[64];

    unsigned long lastUpdate;
    unsigned int updateInterval = 3600;

    TempUnits units = F;

    char zipcode[6];
    int weather_id;
    char description[20];
    char icon[8];
    float temperature;
    int pressure;
    int humidity;
    float windSpeed;
    float windDeg;

    void updateWeather();
  public:
    SmartWeather();
    SmartWeather(char *);
    SmartWeather(char *, char *);
    SmartWeather(char *, char *, unsigned long);

    // getters
    int getWeatherID();
    char *getDescription();
    char *getIcon();
    float getTemperature();
    int getPressure();
    int getHumidity();
    float getWindSpeed();
    float getWindDirection();

    //setters
    void setZipcode(char *);
    void setUpdateInterval(unsigned int); 
    void setOpenWeatherMapAPIKey(char *);
    void setUnits(TempUnits t) { units = t; return; };
};

#endif
