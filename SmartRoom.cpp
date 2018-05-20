#include "SmartRoom.h"

SmartRoom::SmartRoom()
{
  return;
}

SmartRoom::SmartRoom(char *n) 
{
  strcpy(roomName, n);
  exists = true;
  return;
}

SmartRoom::SmartRoom(unsigned char f, unsigned char r)
{
  roomNumber = r;
  floorNumber = f;
  exists = true;
  return;
}

SmartRoom::SmartRoom(char *n, unsigned char f, unsigned char r)
{
  strcpy(roomName, n);
  roomNumber = r;
  floorNumber = f;
  exists = true;
  return;
}

void SmartRoom::setRoomName(char *n) 
{
  strcpy(roomName, n);

  return;
}
 
char *SmartRoom::getRoomName()
{
  return (char *)roomName;
}

unsigned char SmartRoom::getRoomNumber()
{
  return roomNumber;
}

unsigned char SmartRoom::getFloorNumber()
{
  return floorNumber;
}

void SmartRoom::setRoomNumber(unsigned char r)
{
  roomNumber = r;
  return;
}

void SmartRoom::setFloorNumber(unsigned char f)
{
  floorNumber = f;
  return;
}

void SmartRoom::addVent(char *v)
{
  int x;
  int exist = 0;
  char vn[VENT_NAME_SIZE];
  int pos = VENT_EEPROM_START + (roomNumber * VENT_NAME_SIZE);

  EEPROM.begin(4096);
  
  for(x = 0; x < numVents; x++) {
    memset(vn, '\0', sizeof(vn));
  
    EEPROM.get(pos, vn);
    if(vn[0] == 255)
      break;
    
    if(!strncmp(v, vn, VENT_NAME_SIZE)) {
      exist = 1;
      break;
    }
    pos += VENT_NAME_SIZE;
  }
  
  if(exist == 0) {
    pos = VENT_EEPROM_START + ((roomNumber * VENT_NAME_SIZE) * numVents);
    strcpy(vn, v);
    vn[VENT_NAME_SIZE] = '\0';
    EEPROM.put(pos, vn);
    EEPROM.commit();
    numVents++;
  }

  EEPROM.end();
  save();
  
  return;
}

char *SmartRoom::getVent(unsigned char n) 
{
  char sv[VENT_NAME_SIZE];
  int pos = VENT_EEPROM_START + (VENT_NAME_SIZE * n);

  memset(sv, '\0', sizeof(sv));
  
  EEPROM.begin(4096);
  EEPROM.get(pos, sv);
  EEPROM.end();

  return (char *)sv;
}

bool SmartRoom::load(unsigned char r)
{
//Read EEPROM and load members
  EEPROM_Room roomStruct;

  int x;

  Serial.print("Loading room ");
  Serial.println(r, DEC);
  Serial.print("From position: ");
  Serial.println(ROOM_EEPROM_START + (r * sizeof(roomStruct)));

  Serial.print("reading ");
  Serial.print(sizeof(roomStruct));
  Serial.println(" Bytes");

  EEPROM.begin(4096);
  EEPROM.get(ROOM_EEPROM_START + (r * sizeof(roomStruct)), roomStruct);
  EEPROM.end();

  Serial.print("First byte is ");
  Serial.println(roomStruct.roomName[0]);
  
  if(roomStruct.roomName[0] == 255) {
    Serial.println("No more rooms");
    return false;
  }

  strcpy(roomName, roomStruct.roomName);
  roomNumber = r;
  floorNumber = roomStruct.floorNumber;
  numVents = roomStruct.numVents;

  return true;
}

void SmartRoom::save()
{
//Write this room to EEPROM at position (whatever)+roomnumber
  EEPROM_Room roomStruct;

  int x;

  strcpy(roomStruct.roomName, roomName);
  roomStruct.floorNumber = floorNumber;
  roomStruct.numVents = numVents;
  
  EEPROM.begin(4096);
  EEPROM.put(ROOM_EEPROM_START + (roomNumber * sizeof(roomStruct)), roomStruct);
  EEPROM.commit();
  EEPROM.end();

  return;
}

void SmartRoom::setCurrentTemperature(unsigned char t)
{
  currentTemperature = t;

  return;
}

void SmartRoom::setDefaultTemperature(unsigned char t)
{
  defaultTemperature = t;

  return;
}
unsigned char SmartRoom::getCurrentTemperature()
{
  return currentTemperature;
}

void SmartRoom::printRoom() 
{
  int x;

  Serial.print("Room Name: ");
  Serial.println(roomName);

  Serial.print("Room Number: ");
  Serial.println(roomNumber, DEC);

  Serial.print("Floor Number: ");
  Serial.println(floorNumber, DEC);

  Serial.print("numVents: ");
  Serial.println(numVents, DEC);
/*
  Serial.println("Vent Names:");
  for(x = 0; x < numVents - 1; x++) {
    Serial.print(x, DEC);
    Serial.print("	");
    Serial.println(r.ventName[x]);
  }
*/
  Serial.print("Default Temp: ");
  Serial.println(defaultTemperature, DEC);
}

unsigned char SmartRoom::getScheduledTemperature(unsigned char d, int m)
{
  Schedule s;
  int x;
  unsigned char temp = defaultTemperature;
  int pos = SCHEDULE_EEPROM_START + (roomNumber * sizeof(struct Schedule) * DAYS_OF_WEEK);

  EEPROM.begin(4096);
  for(x = 0; x < schedulePoints; ++x) {
    EEPROM.get(pos, s);

    Serial.println("Read schedule: ");
    Serial.print("  startMinute: ");
    Serial.println(s.startMinute, DEC);
    Serial.print("  temperature: ");
    Serial.println(s.temperature, DEC);
    
    if(s.startMinute > m)
      break;
  
    if(s.startMinute <= m)
      temp = s.temperature;

    pos += sizeof(struct Schedule);
  }
  
  EEPROM.end();
  // This should return the setting for the last set point, so if the time is between midnight and the first start point, it'll return the last point from yesterday
  // or the default temp if there aren't any scheduled points.
  return temp;
}

bool SmartRoom::addSchedulePoint(int m, unsigned char d, unsigned char t)
{
  Schedule s;
  int x, y;
  int pos;

  if(schedulePoints >= MAX_SCHEDULE_SET_POINTS)
    return false;

  pos = SCHEDULE_EEPROM_START + (roomNumber * sizeof(struct Schedule) * DAYS_OF_WEEK); // First sched point
  
  Serial.print("Minute: ");
  Serial.println(m);
  
  EEPROM.begin(4096);

  for(x = 0; x < MAX_SCHEDULE_SET_POINTS; ++x) {
    EEPROM.get(pos, s);

    Serial.print("  existing minute: ");
    Serial.println(s.startMinute);

    if(s.startMinute == -1) {
      s.startMinute = m;
      s.temperature = t;
      EEPROM.put(pos, s);
      break;
    }
    
    if(s.startMinute < m) {
      pos += sizeof(struct Schedule);
      continue;
    }

    if(s.startMinute == m) {
      // Replace existing
      s.startMinute = m;
      s.temperature = t;
      EEPROM.put(pos, s);
      break;
    }
    pos = SCHEDULE_EEPROM_START + (roomNumber * sizeof(struct Schedule) * DAYS_OF_WEEK); // First sched point
    pos += sizeof(struct Schedule) * (MAX_SCHEDULE_SET_POINTS - 1);
    
    for(y = MAX_SCHEDULE_SET_POINTS - 1; y > x; --y) {
      EEPROM.get(pos, s);
      EEPROM.put(pos + sizeof(struct Schedule), s);
      pos -= sizeof(struct Schedule);
    }
    pos -= sizeof(struct Schedule);
    
    s.startMinute = m;
    s.temperature = t;
    EEPROM.put(pos, s);
  }

  EEPROM.commit();
  EEPROM.end();

  schedulePoints++;
  save();
  return true;
}

