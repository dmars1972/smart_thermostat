#include <DallasTemperature.h>

const int DS_PIN = 5;

OneWire oneWire(DS_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress tempAddress;

void initTemperatureSensor(const int dspin) {
  sensors.begin();

  Serial.print("Temperature sensor: ");
  Serial.println(sensors.getDeviceCount());

  if(sensors.getDeviceCount() > 0) {
    sensors.getAddress(tempAddress, 0);
    sensors.setResolution(tempAddress, 9);
  }

  return;
}

float getCurrentTemperature() {
  float temperature;

  sensors.requestTemperatures();
  delay(500);
  temperature = sensors.getTempF(tempAddress);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("F ");
  Serial.print(DallasTemperature::toCelsius(temperature));
  Serial.println("C");

  return temperature;
}
