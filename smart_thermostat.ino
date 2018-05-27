#include <SmartConfig.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

#include "SmartRoom.h"
#include "SmartHVAC.h"
#include "SmartWeather.h"
#include "SmartDisplay.h"

SmartConfig smartConfig;
SmartWeather sw;
SmartDisplay smartDisplay;

WiFiServer server(54698);
WiFiServer regServer(54699);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

SmartRoom sr[MAX_ROOMS];
SmartHVAC hvac;


const char CURRENT_VERSION[] = "1.0";
const char DEVICE_TYPE[] = "thermostat";

byte numRooms = 0;
byte openRooms[4] = {0, 0, 0, 0};

byte currentDayOfWeek;

char outsideTemp[7];

void setup() {
  int x, y;
  Serial.begin(115200);
  
  smartConfig.setup();

  smartDisplay.begin();

  smartDisplay.setStatus("Starting up...");
  sw.setZipcode(smartConfig.getZipcode());
  sw.setOpenWeatherMapAPIKey(smartConfig.getOpenWeatherMapAPIKey());
  sw.setUpdateInterval(3600);
  sw.setUnits(F);
  
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
  Serial.print("Heat Schedule EEPROM start: ");
  Serial.println(HEAT_SCHEDULE_EEPROM_START, DEC);
  Serial.print("Cool Schedule EEPROM start: ");
  Serial.println(COOL_SCHEDULE_EEPROM_START, DEC);

  Serial.print("Auto Heat Temp: ");
  Serial.println(smartConfig.getAutoHeatTemperature(), DEC);
  Serial.print("Auto Cool Temp: ");
  Serial.println(smartConfig.getAutoCoolTemperature(), DEC);
  
  Serial.print("Timezone offset: ");
  Serial.println(smartConfig.getTimezoneOffset(), DEC);
  
  //timeClient.setTimeOffset(smartConfig.getTimezoneOffset());
  timeClient.begin();
  
  timeClient.forceUpdate();

  smartConfig.queryTimezone(timeClient.getEpochTime());
  timeClient.setTimeOffset(smartConfig.getTimezoneOffset());
  
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
      sr[x].addSchedulePoint(HEAT, 0, y, 75);
      sr[x].addSchedulePoint(HEAT, 12*60, y, 78);
      sr[x].addSchedulePoint(HEAT, (15*60)+30, y, 68);
      sr[x].addSchedulePoint(HEAT, 18*60, y, 77);
    }
    for(y = 0; y < 7; ++y) {
      sr[x].addSchedulePoint(COOL, 0, y, 75);
      sr[x].addSchedulePoint(COOL, 12*60, y, 78);
      sr[x].addSchedulePoint(COOL, (15*60)+30, y, 68);
      sr[x].addSchedulePoint(COOL, 18*60, y, 77);
    }
    numRooms++;
  }
  
  smartDisplay.setWeatherIcon(sw.getIcon());

  sprintf(outsideTemp, "%.0fF", sw.getTemperature());

  Serial.println(outsideTemp);
  
  smartDisplay.setOutsideTemp(outsideTemp);
  
  smartDisplay.setStatus("Setup complete");
  Serial.println("Setup complete.");
  delay(1000);
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

  smartDisplay.clearStatus();

  if(!smartConfig.getTimezoneStatus()) {
    smartConfig.queryTimezone(timeClient.getEpochTime());
    timeClient.setTimeOffset(smartConfig.getTimezoneOffset());
  }
  Serial.println(timeClient.getFormattedTime());
  Serial.print("Current outside temperature: ");
  Serial.println(sw.getTemperature(), DEC);

  smartDisplay.setWeatherIcon(sw.getIcon());
  
  sprintf(outsideTemp, "%.0fF", sw.getTemperature());
  
  smartDisplay.setOutsideTemp(outsideTemp);
  
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
    
    setTemp = sr[x].getScheduledTemperature(hvac.getHVACMode(), currentDayOfWeek, currentMinute);

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
/*
void updateCurrentWeather(char *zipcode, char *apikey)
{
  static long lastCheck;
  WiFiClientSecure client;
  const char *host = "api.openweathermap.org";
  char url[128];
  
  if(lastCheck == 0 || lastCheck + WEATHER_CHECK_INTERVAL_SECONDS < timeClient.getEpochTime()) {
    Serial.println("Checking the weather...");
    
    sprintf(url, "/data/2.5/weather?zip=%s&units=imperial&APPID=%s", zipcode, apikey);

    if(!client.connect(host, 443)) {
      Serial.println("Failed to connect to api.openweathermap.org");
      return;
    }

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" +
             "User-Agent: BuildFailureDetectorESP8266\r\n" +
             "Connection: close\r\n\r\n");

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") 
        break;
    }

    String line = client.readStringUntil('\r');

    const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 400;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    const char* json = "{\"coord\":{\"lon\":-97.18,\"lat\":33.12},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01d\"}],\"base\":\"stations\",\"main\":{\"temp\":87.42,\"pressure\":1015,\"humidity\":48,\"temp_min\":86,\"temp_max\":89.6},\"visibility\":11265,\"wind\":{\"speed\":5.41,\"deg\":154.008},\"clouds\":{\"all\":1},\"dt\":1527023700,\"sys\":{\"type\":1,\"id\":2597,\"message\":0.0052,\"country\":\"US\",\"sunrise\":1526988258,\"sunset\":1527038821},\"id\":420037411,\"name\":\"Denton\",\"cod\":200}";

    JsonObject& root = jsonBuffer.parseObject(line);
    JsonObject& weather0 = root["weather"][0];
    ws.weather_id = weather0["id"];
    const char* weather0_main = weather0["main"];
    strcpy(ws.description, weather0["description"]);
    strcpy(ws.icon, weather0["icon"]);
    const char* base = root["base"];
    JsonObject& main = root["main"];
    ws.outsideTemperature = main["temp"];
    ws.pressure = main["pressure"];
    ws.humidity = main["humidity"]; 
    ws.wind_speed = root["wind"]["speed"];
    ws.wind_deg = root["wind"]["deg"];

    lastCheck = timeClient.getEpochTime();
  }
  client.stop();
    
  return;
}
*/

