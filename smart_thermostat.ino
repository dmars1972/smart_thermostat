#include <TimeLib.h>

#include <NTPClient.h>
#include "sv_eeprom.h"
#include "sv_wifi.h"
#include <WiFiUdp.h>
#include "SmartConfig.h"
#include "SmartRoom.h"

SmartConfig smartConfig;
SmartRoom sr[MAX_ROOMS];

const char CURRENT_VERSION[] = "1.0";
const char DEVICE_TYPE[] = "thermostat";

WiFiServer server(54698);
WiFiServer regServer(54699);
//Room rooms[MAX_ROOMS];
byte numRooms = 0;
byte currentMode;
byte currentState;
byte openRooms[4] = {0, 0, 0, 0};
byte furnaceState[4] = {OFF, OFF, OFF, OFF};
byte ACState[4] = {OFF, OFF, OFF, OFF};

byte currentDayOfWeek;

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP);

void setup() {
  int x, y;

  smartConfig.setup();
  Serial.begin(115200);
  Serial.print("Device: ");
  Serial.println(DEVICE_TYPE);
  Serial.print("Software version: ");
  Serial.println(CURRENT_VERSION);

  Serial.print("Configuration EEPROM start: ");
  Serial.println(CONFIG_EEPROM_START, DEC);
  Serial.print("Room EEPROM start: ");
  Serial.println(ROOM_EEPROM_START, DEC);
  Serial.print("Vent EEPROM start: ");
  Serial.println(VENT_EEPROM_START, DEC);
  Serial.print("Schedule EEPROM start: ");
  Serial.println(SCHEDULE_EEPROM_START, DEC);

  Serial.print("WiFi Status: ");
  if(smartConfig.getWiFiStatus() == true)
    Serial.println("connected");
  else
    Serial.println("not connected");
  timeClient.setTimeOffset(-5*3600);
  
  timeClient.begin();
  
  Serial.print("Current time: ");
  Serial.println(timeClient.getFormattedTime());

  setSyncProvider(getNTPTime);
  server.begin();
  regServer.begin();

  for(x = 0; x < MAX_ROOMS; x++) {
    if(! sr[x].load(x)) {
      Serial.println("Load failed");
      break;
    }
    sr[x].exists = true;

    // For testing
    for(y = 0; y < 7; ++y) {
      sr[x].addSchedulePoint(0, y, 75);
      sr[x].addSchedulePoint(12*60, y, 78);
      sr[x].addSchedulePoint((15*60)+30, y, 68);
      sr[x].addSchedulePoint(18*60, y, 77);
    }
    numRooms++;
  }

  currentMode = COOL;
  currentState = OFF;
  
  Serial.println("Setup complete.");
}

long int getNTPTime() {
  timeClient.update();
  return (long int)timeClient.getEpochTime();
}

void loop() {
  WiFiClient client;
  
  int ventCount = 0;
  byte roomNumber = 0;
  byte temp = 0;
  char tempName[VENT_NAME_SIZE];
  int x, y;
  int ventNumber;
  byte floorNumber;
  int openRooms = 0;
  SmartRoom r;

  client = regServer.available();

  if(client) {
    Serial.println("Registering vent...");
    delay(50);
    roomNumber = client.read();
    delay(50);
    floorNumber = client.read();
    memset(tempName, '\0', sizeof(tempName));
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
    if(sr[roomNumber].exists) {
      sr[roomNumber].addVent(tempName);
    } else {
      Serial.print("Room: ");
      Serial.println(roomNumber, DEC);
      Serial.print("Floor: ");
      Serial.println(floorNumber, DEC);
      sr[roomNumber] = SmartRoom("Unknown", floorNumber, roomNumber);
      sr[roomNumber].addVent(tempName);
      sr[roomNumber].save();
      numRooms++;
    }

    client.stop();    
  }
  
  client = server.available();
  
  if(client) {
    Serial.println("Got a client!");
    
    delay(100);
    roomNumber = client.read();
    temp = client.read();
    if(sr[roomNumber].exists)
      sr[roomNumber].setCurrentTemperature(temp);
    else {
      Serial.print("Got an unknown room: ");
      Serial.println(roomNumber, DEC);
    }

    client.stop();
  }

  processRooms();

  delay(10000);

}

void processRooms() {
  int x;
  int currentMinute = (hour()*60)+minute();
  
  byte setTemp;
  char vn[VENT_NAME_SIZE];
  currentDayOfWeek = weekday();
  
  for(x = 0; x < numRooms; x++) {
    Serial.print("Working on room: ");
    Serial.println(x);
    Serial.print("Room Name: ");
    Serial.println(sr[x].getRoomName());
    
    setTemp = sr[x].getScheduledTemperature(currentDayOfWeek, currentMinute);

    Serial.print("Current set temperature is ");
    Serial.println(setTemp, DEC);
    for(x = 0; x < sr[x].getNumVents(); ++x) {
      strcpy(vn, sr[x].getVent(x));
      Serial.print("  ");
      Serial.println(vn);
    }
/*
    if(currentMode == HEAT) {
      if(rooms[x].currentTemp < setTemp) {
        if(rooms[x].currentPosition == SV_CLOSED) {
          openVents(x);
          rooms[x].currentPosition = SV_OPEN;
          openRooms[rooms[x].floorNumber]++;
        }
      } else {
        if(rooms[x].currentPosition == SV_OPEN) {
          closeVents(x);
          rooms[x].currentPosition = SV_CLOSED; 
          openRooms[rooms[x].floorNumber]--;
        }
      }
      if(openRooms[rooms[x].floorNumber] > 0 && furnaceState[rooms[x].floorNumber] == OFF) {
        setFurnace(rooms[x].floorNumber, ON);
      } else if(openRooms[rooms[x].floorNumber] == 0 && furnaceState[rooms[x].floorNumber] == ON) {
        setFurnace(rooms[x].floorNumber, OFF);
      }
    } else if(currentMode == COOL) {
      if(rooms[x].currentTemp > setTemp) {
        if(rooms[x].currentPosition == SV_CLOSED) {
          openVents(x);
          rooms[x].currentPosition = SV_OPEN;
          openRooms[rooms[x].floorNumber]++;
        }
      } else {
        if(rooms[x].currentPosition == SV_OPEN) {
          closeVents(x);
          rooms[x].currentPosition = SV_CLOSED;
          openRooms[rooms[x].floorNumber]--;
        }
      }
      if(openRooms[rooms[x].floorNumber] > 0 && ACState[rooms[x].floorNumber] == OFF) {
        setAC(rooms[x].floorNumber, ON);
      } else if(openRooms[rooms[x].floorNumber] == 0 && ACState[rooms[x].floorNumber] == ON) {
        setAC(rooms[x].floorNumber, OFF);
      }
    }
    */
  }

  delay(1000);
}

byte getCurrentSetTemperature(byte r, byte d, int m) {
  int x;
  byte setTemp;
  
/*  for(x = 0; x < 4; x++) {
    if(roomSchedule[r][d].timeSchedule[x].minuteStart < m)
      setTemp = roomSchedule[r][d].timeSchedule[x].setTemp;
    else if(roomSchedule[r][d].timeSchedule[x].minuteStart >= m)
      break;
  }
*/
  return setTemp;
}

void setFurnace(byte f, byte s) {
  if(s == ON) {
    Serial.print("Turn on furnace on floor ");
    Serial.println(f, DEC);
    furnaceState[f] = ON;
  } else {
    Serial.print("Turn off furnace on floor ");
    Serial.println(f, DEC);
    furnaceState[f] = OFF;
  }
}

void setAC(byte f, byte s) {
  if(s == ON) {
    Serial.print("Turn on AC on floor ");
    Serial.println(f, DEC);
    ACState[f] = ON;
  } else {
    Serial.print("Turn off AC on floor ");
    Serial.println(f, DEC);
    ACState[f] = OFF;
  }
}


