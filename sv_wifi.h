#include <ESP8266WiFi.h>
#include <SpritzCipher.h>
#include <ESP8266httpUpdate.h>
#include "types.h"


void svDecrypt(byte *password, int len) {
  byte decrypted[64];
  spritz_ctx s_ctx;
  byte key[32] = {'k','a','u','o','k','2','9','u','a','9','$','3','8','7','a','8','9','d','a','8','n','w','?','u','l','s','U','9','3','4','n','o'};

  
  spritz_setup(&s_ctx, key, sizeof(key));
  spritz_crypt(&s_ctx, password, len, password);  

  return;
}

void svStartWifi(configStruct conf) {
  Serial.print("SSID: ");
  Serial.println(conf.ssid);
  Serial.print("Password: ");
  Serial.println((char *)conf.password);
  
  WiFi.begin(conf.ssid, (char *)conf.password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
void svConnect(configStruct conf) {
  // Just make sure we're still connected.  If not, reconnect.
  while(WiFi.status() != WL_CONNECTED) {  
    WiFi.begin(conf.ssid, (char *)conf.password);
    delay(500);
  }
}

void checkForUpdate(char *OTAIPAddress, const char *currentVersion, const char *deviceType) {
  char OTAURL[128];
  char OTAPATH[26] = "";

  if(!strcmp(deviceType, "vent"))
    strcpy(OTAPATH, "/svUpdate/svUpdate.php");
  else if (!strcmp(deviceType, "thermostat"))
    strcpy(OTAPATH, "/svUpdate/stUpdate.php");
    
  Serial.print("Checking for updates at ");
  Serial.print(OTAIPAddress);
  Serial.print(OTAPATH);
  Serial.print(" ");
  Serial.println(currentVersion);

  t_httpUpdate_return ret = ESPhttpUpdate.update(OTAIPAddress, 80, OTAPATH, currentVersion);
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n",  ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("No updates");
        break;
      case HTTP_UPDATE_OK:
        break;
    }
}

void sendTemperature(byte roomNumber, float temp) {
  WiFiClient client;
  byte tempAsByte;
  
  tempAsByte = round(temp);
  
  Serial.print("Temperature: ");
  Serial.println(tempAsByte, DEC);
  Serial.println("Attempting to connect to thermostat...");
  client.connect("thermostat", 54698);
  if(client.connected()) {
    Serial.println("Connected!");
    client.write(roomNumber);
    client.write(tempAsByte);
    client.flush();
  }
    
  client.stop();
  
  // Nothing to see here yet, need to work on the thermostat
  return;
}

int getExpectedPosition(byte roomNumber) {
  // Ditto
  
  return SV_CLOSED;
}

void svSetHostName(char *hostName) {
  WiFi.hostname(hostName);
}

void openVents(byte r) {
  Serial.print("Open vents in room ");
  Serial.println(r, DEC);
}

void closeVents(byte r) {
  Serial.print("Close vents in room ");
  Serial.println(r, DEC);
}


