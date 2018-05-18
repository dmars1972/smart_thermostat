#include "sv_eeprom.h"
#include "sv_wifi.h"

const char CURRENT_VERSION[] = "1.0";
const char DEVICE_TYPE[] = "thermostat";

configStruct conf;
char OTAIPAddress[16];

WiFiServer server(54698);
WiFiServer regServer(54699);
Room rooms[20];

void setup() {
  int x, y;
  
  Serial.begin(115200);
  Serial.print("Device: ");
  Serial.println(DEVICE_TYPE);
  Serial.print("Software version: ");
  Serial.println(CURRENT_VERSION);

  conf = getConfiguration();

  sprintf(OTAIPAddress, "%d.%d.%d.%d", conf.otaIPAddress[0], conf.otaIPAddress[1], conf.otaIPAddress[2], conf.otaIPAddress[3]);

  svDecrypt(conf.password, sizeof(conf.password));
  Serial.println("Connecting to wifi...");
  svSetHostName("thermostat");

  svStartWifi(conf);
  Serial.println("Connected.");
  server.begin();
  regServer.begin();

  Serial.println("Setup complete.");
}


void loop() {
  WiFiClient client;
  
  int ventCount = 0;
  byte roomNumber = 0;
  byte temp = 0;
  char tempName[21];
  int x, y;
  int ventNumber;
  
  client = regServer.available();

  if(client) {
    Serial.println("Registering vent...");
    delay(500);
    roomNumber = client.read();
    x = 0;
    for(;;) {
      temp = client.read();
      if (temp == -1) {
        delay(50);
        continue;
      }
      if (temp == '\r')
        break;
      tempName[x] = temp;
      x++;
    }

    client.stop();

    ventCount++;

    ventNumber = rooms[roomNumber].numVents + 1;
    rooms[roomNumber].numVents = rooms[roomNumber].numVents + 1;
    Serial.print("vent name: ");
    Serial.println(tempName);
    
    strncpy(rooms[roomNumber].ventName[ventNumber], tempName, x-1);
    
    Serial.println("Registered a new vent");
    Serial.print("  Room Number: ");
    Serial.println(roomNumber);
    Serial.print("Vents in room: ");
    Serial.println(rooms[roomNumber].numVents);
    Serial.println("Vent Names: ");
    Serial.print("  ");
    Serial.println(rooms[roomNumber].ventName[rooms[roomNumber].numVents]);
    
    for(y = 0; y < rooms[roomNumber].numVents; y++) {
      Serial.print("  ");
      Serial.println(rooms[roomNumber].ventName[y]);
    }
  }
  
  client = server.available();
  
  if(client) {
    Serial.println("Got a client!");
    
    delay(100);
    roomNumber = client.read();
    temp = client.read();
    Serial.print("Room number: ");
    Serial.println(roomNumber, DEC);
    Serial.print("Temp: ");
    Serial.println(temp, DEC);
    client.stop();

    rooms[roomNumber].currentTemp = temp;
  }
}
