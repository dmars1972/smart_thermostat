#include <SmartConfig.h>
#include <SmartVentComm.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

#include "Settings.h"
#include "SmartRoom.h"
#include "SmartHVAC.h"
#include "SmartWeather.h"
#include "SmartDisplay.h"
#include "defs.h"

Settings settings;
SmartConfig smartConfig;
SmartWeather sw;
SmartRoom sr[MAX_ROOMS];
SmartDisplay smartDisplay(&settings, sr);
SmartVentComm svc(25836, 25837);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);



SmartHVAC hvac;

const char CURRENT_VERSION[] = "1.0";
const char DEVICE_TYPE[] = "thermostat";

byte numRooms = 0;
byte openRooms[4] = {0, 0, 0, 0};

byte currentDayOfWeek;

char outsideTemp[7];
unsigned char currentSetTemp;

void setup() {
  int x, y;
  Serial.begin(115200);
  
  smartConfig.setup("thermostat");

  smartDisplay.begin();

  sw.setZipcode(smartConfig.getZipcode());
  sw.setOpenWeatherMapAPIKey(smartConfig.getOpenWeatherMapAPIKey());
  sw.setUpdateInterval(300);
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

  for(x = 0; x < MAX_ROOMS; x++) {
    char roomname[32];
    if(! sr[x].load(x)) {
      Serial.println("Load failed");
      break;
    }
    Serial.println("Loaded room");
    //sprintf(roomname, "room %d", x);
    //sr[x].setRoomName(roomname);
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

  sprintf(outsideTemp, "%.0f", sw.getTemperature());

  Serial.println(outsideTemp);
  
  smartDisplay.setOutsideTemp(outsideTemp);
  
  settings.setCurrentSetTemp((unsigned char)76);
  smartDisplay.setSetTemp(settings.getCurrentSetTemp());
}

long int getNTPTime() {
  timeClient.update();
  return (long int)timeClient.getEpochTime();
}

unsigned long secs, lastsecs;

void loop() {
  WiFiClient client;
  char currentTime[6];
  TempStruct tempStruct;
  char currentSetTempString[4];
  RegisterStruct reg;
  
  
/*
Serial.println("handleTouch()");
Serial.flush();
*/
  smartDisplay.handleTouch();
/*
Serial.println("getHours");
Serial.flush();
*/
  if(timeClient.getHours() > 12) {
    sprintf(currentTime, "%d:%02d", timeClient.getHours() - 12, timeClient.getMinutes());
  } else {
    sprintf(currentTime, "%d:%02d", timeClient.getHours(), timeClient.getMinutes());
  }

  secs = millis()/1000;
  if(secs != lastsecs) {
    Serial.println(secs);
    lastsecs = secs;
  }
  
  
/*
Serial.println("setTime");
Serial.flush();
*/
smartDisplay.setTime(currentTime);
/*
Serial.println("setWeatherIcon");
Serial.flush();
*/
  smartDisplay.setWeatherIcon(sw.getIcon());
/*Serial.println("setweather done");
Serial.flush();
*/
  sprintf(outsideTemp, "%.0f", sw.getTemperature());

/*
Serial.println("setOutsideTemp");
Serial.flush();
*/
  smartDisplay.setOutsideTemp(outsideTemp);
  smartDisplay.setSetTemp(settings.getCurrentSetTemp());


  processRooms();

  if(svc.receiveTemperature(&tempStruct)) {
    Serial.println("receiveTemperature: ");
    Serial.print("  ");
    Serial.println(tempStruct.host);
    Serial.print("  ");
    Serial.println(tempStruct.temp);
    
    int x = 0;
    bool found = false;
    do {
      Serial.println("setRoomTemp");
      Serial.flush();
      if(sr[x].setRoomTemp(tempStruct.host, tempStruct.temp)) {
        Serial.println("got a match, temp set");
        found = true;
        break;
      }
      x++;
    } while(x < numRooms);

    if(!found) {
      smartDisplay.addNotification();
      smartDisplay.addUnknownVent(tempStruct.host);
    }
    Serial.println("Received: ");
    Serial.print("host  ");
    Serial.println(tempStruct.host);
    Serial.print("temp  ");
    Serial.println(tempStruct.temp);
  } 
/*
  Serial.println("loop complete");
  Serial.flush();
*/
  yield();
}

void processRooms() {
  int x;
  int currentMinute;
  
  byte setTemp;
  char vn[VENT_NAME_SIZE];

  currentMinute = (hour()*60)+minute();
  currentDayOfWeek = weekday();

  for(x = 0; x < numRooms; x++) {
    
    setTemp = sr[x].getScheduledTemperature(hvac.getHVACMode(), currentDayOfWeek, currentMinute);
    for(x = 0; x < sr[x].getNumVents(); ++x) {
      strcpy(vn, sr[x].getVent(x));
    }
  }

  //delay(1000);
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
