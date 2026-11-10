#include <OneWire.h>
#include <DallasTemperature.h>

const int SENSOR_PIN = 17;
OneWire oneWire(SENSOR_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  sensors.begin();
}

void loop() {
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  if (temperature == DEVICE_DISCONNECTED_C) {
    Serial.println("Sensor error: check wiring");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
  }

  delay(2000);
}
