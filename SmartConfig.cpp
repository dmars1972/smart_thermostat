#include "SmartConfig.h"

SmartConfig::SmartConfig()
{
  return;
}
void SmartConfig::setup()
{
  int pos = 0;
  char encryptedPassword[64];
  spritz_ctx s_ctx;

  EEPROM.begin(4096);
  EEPROM.get(pos, otaIPAddress);
  pos += sizeof(otaIPAddress);
  EEPROM.get(pos, roomNumber);
  pos += sizeof(roomNumber);
  EEPROM.get(pos, floorNumber);
  pos += sizeof(floorNumber);
  EEPROM.get(pos, hasTempSensor);
  pos += sizeof(hasTempSensor);
  EEPROM.get(pos, ssid);
  pos += sizeof(ssid);
  EEPROM.get(pos, encryptedPassword);

  spritz_setup(&s_ctx, key, sizeof(key));
  spritz_crypt(&s_ctx, (unsigned char *)encryptedPassword, sizeof(password), (unsigned char *)password);  

  WiFi.hostname("thermostat");

  WiFi.begin(ssid, (char *)password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  return;
}

bool SmartConfig::getWiFiStatus() 
{
  if(WiFi.status() == WL_CONNECTED)
    return true;

  return false;
}
