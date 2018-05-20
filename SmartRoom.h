#ifndef SMART_ROOM
#define SMART_ROOM
#include <string.h>
#include "types.h"
#include <EEPROM.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define MAX_VENTS_PER_ROOM 10
#define DAYS_OF_WEEK 7

struct Schedule {
  int startMinute;
  unsigned char temperature;
};

class SmartRoom {
  private:
    char roomName[32];
    unsigned char roomNumber;
    unsigned char floorNumber;
    unsigned char numVents = 0;
    
    unsigned char defaultTemperature = 70;
    unsigned char currentTemperature;

    unsigned char schedulePoints = 0;
    void printRoom();
  public:
    // Constructors
    SmartRoom();
    SmartRoom(char *);
    SmartRoom(unsigned char, unsigned char);
    SmartRoom(char *, unsigned char, unsigned char);

    // getters
    unsigned char getRoomNumber();
    unsigned char getFloorNumber();
    char *getRoomName();
    char *getVent(unsigned char);
    unsigned char getNumVents() {return numVents;};
    unsigned char getCurrentTemperature();
    unsigned char getScheduledTemperature(unsigned char, int);
 
    // setters
    void setRoomNumber(unsigned char);
    void setRoomName(char *);
    void setCurrentTemperature(unsigned char);
    void setDefaultTemperature(unsigned char);
    void setFloorNumber(unsigned char);
    
    // member functions
    void addVent(char *);
    bool addSchedulePoint(int, unsigned char, unsigned char);
    bool load(unsigned char);
    void save();
    
    // member variables
    bool exists = false;
    
};

#endif

