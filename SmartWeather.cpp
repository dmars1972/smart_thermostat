#include "SmartWeather.h"


SmartWeather::SmartWeather()
{
  memset(openWeatherMapAPIKey, '\0', sizeof(openWeatherMapAPIKey));
  memset(zipcode, '\0', sizeof(zipcode));
  lastUpdate = 0;
  weather_id = 0;
  memset(description, '\0', sizeof(description));
  memset(icon, '\0', sizeof(icon));
  temperature = 0.0;
  pressure = 0;
  humidity = 0;
  windSpeed = 0.0;
  windDeg = 0.0;

  return;
}

SmartWeather::SmartWeather(char *zip)
{
  memset(zipcode, '\0', sizeof(zipcode));
  memset(openWeatherMapAPIKey, '\0', sizeof(openWeatherMapAPIKey));
  lastUpdate = 0;
  weather_id = 0;
  memset(description, '\0', sizeof(description));
  memset(icon, '\0', sizeof(icon));
  temperature = 0.0;
  pressure = 0;
  humidity = 0;
  windSpeed = 0.0;
  windDeg = 0.0;

  strcpy(zipcode, zip);

  return;
}

SmartWeather::SmartWeather(char *zip, char *key)
{
  memset(zipcode, '\0', sizeof(zipcode));
  memset(openWeatherMapAPIKey, '\0', sizeof(openWeatherMapAPIKey));
  lastUpdate = 0;
  weather_id = 0;
  memset(description, '\0', sizeof(description));
  memset(icon, '\0', sizeof(icon));
  temperature = 0.0;
  pressure = 0;
  humidity = 0;
  windSpeed = 0.0;
  windDeg = 0.0;

  strcpy(zipcode, zip);
  strcpy(openWeatherMapAPIKey, key);

  return;
}

SmartWeather::SmartWeather(char *zip, char *key, unsigned long interval)
{
  memset(zipcode, '\0', sizeof(zipcode));
  memset(openWeatherMapAPIKey, '\0', sizeof(openWeatherMapAPIKey));
  lastUpdate = 0;
  weather_id = 0;
  memset(description, '\0', sizeof(description));
  memset(icon, '\0', sizeof(icon));
  temperature = 0.0;
  pressure = 0;
  humidity = 0;
  windSpeed = 0.0;
  windDeg = 0.0;

  strcpy(zipcode, zip);
  strcpy(openWeatherMapAPIKey, key);
  updateInterval = interval;
}

void SmartWeather::updateWeather()
{
  static long lastCheck;
  WiFiClientSecure client;
  const char *host = "api.openweathermap.org";
  char url[128];

  if(lastUpdate == 0 ||                           // first call
      (millis() - lastUpdate) / 1000 > updateInterval ||   // has interval passed
      millis() < lastUpdate) {                    // has the millis reset?
  
    switch(units) {
      case F: 
          sprintf(url, "/data/2.5/weather?zip=%s&units=imperial&APPID=%s", zipcode, openWeatherMapAPIKey);
          break;
      case C: 
          sprintf(url, "/data/2.5/weather?zip=%s&units=metric&APPID=%s", zipcode, openWeatherMapAPIKey);
          break;
      case K: 
          sprintf(url, "/data/2.5/weather?zip=%s&APPID=%s", zipcode, openWeatherMapAPIKey);
          break;
    }


    if(!client.connect(host, 443)) {
      return;
    }

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" +
             "User-Agent: BuildFailureDetectorESP8266\r\n" +
             "Connection: close\r\n\r\n");

    client.setTimeout(200);

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") 
        break;
    }

    String line = client.readStringUntil('\r');

    const size_t bufferSize = JSON_ARRAY_SIZE(1) + 
                              JSON_OBJECT_SIZE(1) + 
                              2*JSON_OBJECT_SIZE(2) + 
                              JSON_OBJECT_SIZE(4) + 
                              JSON_OBJECT_SIZE(5) + 
                              JSON_OBJECT_SIZE(6) + 
                              JSON_OBJECT_SIZE(12) + 400;

    DynamicJsonBuffer jsonBuffer(bufferSize);

    const char* json = "{\"coord\":{\"lon\":-97.18,\"lat\":33.12},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01d\"}],\"base\":\"stations\",\"main\":{\"temp\":87.42,\"pressure\":1015,\"humidity\":48,\"temp_min\":86,\"temp_max\":89.6},\"visibility\":11265,\"wind\":{\"speed\":5.41,\"deg\":154.008},\"clouds\":{\"all\":1},\"dt\":1527023700,\"sys\":{\"type\":1,\"id\":2597,\"message\":0.0052,\"country\":\"US\",\"sunrise\":1526988258,\"sunset\":1527038821},\"id\":420037411,\"name\":\"Denton\",\"cod\":200}";

    JsonObject& root = jsonBuffer.parseObject(line);
    JsonObject& weather0 = root["weather"][0];
    weather_id = weather0["id"];
    const char* weather0_main = weather0["main"];
    strcpy(description, weather0["description"]);
    strcpy(icon, weather0["icon"]);
    const char* base = root["base"];
    JsonObject& main = root["main"];
    temperature = main["temp"];
    pressure = main["pressure"];
    humidity = main["humidity"]; 
    windSpeed = root["wind"]["speed"];
    windDeg = root["wind"]["deg"];

    lastUpdate = millis();
  }

  return;
}

void SmartWeather::setZipcode(char *zip)
{
  strcpy(zipcode, zip);

  return;
}

void SmartWeather::setOpenWeatherMapAPIKey(char *key)
{
  strcpy(openWeatherMapAPIKey, key);

  return;
}

void SmartWeather::setUpdateInterval(unsigned int i)
{
  updateInterval = i;

  return;
}

int SmartWeather::getWeatherID() 
{
  updateWeather();

  return weather_id;
}

char *SmartWeather::getDescription()
{
  updateWeather();

  return (char *)&description;
}

char *SmartWeather::getIcon() 
{
  updateWeather();

  return (char *)&icon;
}

float SmartWeather::getTemperature()
{
  updateWeather();

  return temperature;
}

int SmartWeather::getPressure()
{
  updateWeather();
  return pressure;
}

int SmartWeather::getHumidity()
{
  updateWeather();

  return humidity;
}

float SmartWeather::getWindSpeed()
{
  updateWeather();

  return windSpeed;
}

float SmartWeather::getWindDirection()
{
  updateWeather();

  return windDeg;
}
