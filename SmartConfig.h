#ifndef _SMART_CONFIG
#define _SMART_CONFIG

#include "Arduino.h"

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <SpritzCipher.h>
#include <ESP8266httpUpdate.h>

class SmartConfig {
  private:
    char otaIPAddress[4];
    unsigned char roomNumber;
    unsigned char floorNumber;
    bool hasTempSensor;
    char ssid[64];
    char password[64];
    unsigned char key[32] = {'k','a','u','o','k','2','9','u','a','9','$','3','8','7','a','8','9','d','a','8','n','w','?','u','l','s','U','9','3','4','n','o'};

  public:
    SmartConfig();

    void setup();
    unsigned char getFloorNumber() { return floorNumber; };
    char *getSSID() { return ssid; };
    bool getWiFiStatus();
};

#endif
