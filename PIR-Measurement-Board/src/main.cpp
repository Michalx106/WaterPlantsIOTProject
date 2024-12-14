#include <Adafruit_Sensor.h>
#include "SensorManager.hpp"
#include "ESPNowManager.hpp"
#include "DeepSleepManager.hpp"

#define DHT_PIN 18
#define DHT_TYPE DHT11
#define SOIL_MOISTURE_PIN 34
#define VOLTAGE_PIN 35
#define SLEEP_TIME_US 10 * 60 * 100000ULL // 1 minuta w mikrosekundach

uint8_t peerAddress[] = {0x10, 0x06, 0x1C, 0x82, 0xF7, 0xD0};

SensorManager sensorManager(DHT_PIN, DHT_TYPE, SOIL_MOISTURE_PIN, VOLTAGE_PIN);
ESPNowManager espNowManager(peerAddress);
DeepSleepManager deepSleepManager(SLEEP_TIME_US);

void setup() {
    Serial.begin(115200);

    sensorManager.begin();
    espNowManager.begin();

    SensorData data = sensorManager.readSensors();
    sensorManager.printSensorData(data);

    espNowManager.sendData(data);

    deepSleepManager.enterDeepSleep();
}

void loop() {
    // Pętla jest pusta, ponieważ ESP przechodzi w tryb uśpienia
}
