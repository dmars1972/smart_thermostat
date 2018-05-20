#ifndef _SV_TYPES_
#define _SV_TYPES_

const int MAX_ROOMS = 20;
const int MAX_VENTS_PER_ROOM = 5;
const int VENT_NAME_SIZE = 15;
const int MAX_SCHEDULE_SET_POINTS = 4;

struct configStruct {
  char otaIPAddress[4];
  unsigned char roomNumber;
  unsigned char floorNumber;
  bool hasTempSensor;
  char ssid[64];
  char password[64];
};
/*
struct Room {
  byte floorNumber;
  byte setTemp;
  byte currentTemp;
  byte numVents;
  byte currentPosition;
  char ventName[5][21];
};
*/
struct EEPROM_Room {
  char roomName[31];
  unsigned char floorNumber;
  unsigned char numVents;
};

struct EEPROM_Vent {
  char ventName[VENT_NAME_SIZE];
};

struct EEPROM_Schedule {
  int minuteStart;
  unsigned char setTemp;
};

const int SV_OPEN = 75;
const int SV_CLOSED = 0;

const unsigned char ON = 1;
const unsigned char OFF = 0;
const unsigned char HEAT = 1;
const unsigned char COOL = 0;

const int CONFIG_EEPROM_START = 0;
const int ROOM_EEPROM_START = sizeof(configStruct) + 1;
const int VENT_EEPROM_START = ROOM_EEPROM_START + (sizeof(EEPROM_Room) * MAX_ROOMS) + 1;
const int SCHEDULE_EEPROM_START = VENT_EEPROM_START + ((VENT_NAME_SIZE * MAX_ROOMS) * MAX_VENTS_PER_ROOM) + 1;
#endif

