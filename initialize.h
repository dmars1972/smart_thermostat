#include <ESP8266WiFi.h>

void initialize() {
  IPAddress local_IP(192,168,43,77);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);
  char uniqueSSID[63];
  String mac = WiFi.macAddress();
  WiFiServer server(80);
  WiFiClient client;
  int x = 0;
  byte read_byte;
  int field = 0;
  char SSID[64] = "";
  char password[64];
  byte thisRoomNumber;

  Serial.println("Beginning initial setup...");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed");

  sprintf(uniqueSSID, "sv-%s", mac.c_str());
  Serial.print("My SSID is: ");
  Serial.println(uniqueSSID);
  Serial.println(WiFi.softAP((const char *) uniqueSSID) ? "Ready" : "Failed");
  Serial.print("My address is: ");
  Serial.println(WiFi.softAPIP());

  while( ! client.connected()) {
     client = server.available();
     delay(500);
  }

  while(true) {
    read_byte = client.read();
     
    if(field == 0) {
      SSID[x] = read_byte;
      x = x + 1;
      if(read_byte == '\r') {
        field = field + 1;
        x = 0;
      }
    } else if(field == 1) {
      password[x] = read_byte;
      x = x + 1;
      if(read_byte == '\r') {
        field = field + 1;
        x = 0;
      }
    } else if(field == 2) {
      thisRoomNumber = read_byte;
      break;
    }
  }
}

