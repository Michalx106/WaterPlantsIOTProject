#include "SensorManager.hpp"
#include <Arduino.h>

SensorManager::SensorManager(int dhtPin, int dhtType, int soilPin, int voltagePin)
    : dht(dhtPin, dhtType), soilMoisturePin(soilPin), voltagePin(voltagePin) {}

void SensorManager::begin() {
    dht.begin();
}

SensorData SensorManager::readSensors() {
    SensorData data;
    data.temperature = dht.readTemperature();
    data.humidity = dht.readHumidity();
    data.soilMoisture = analogRead(soilMoisturePin) / 4095.0 * 100;
    data.voltage = analogRead(voltagePin) * (3.3 / 4095.0);

    if (isnan(data.temperature) || isnan(data.humidity)) {
        data.temperature = 0;
        data.humidity = 0;
    }

    return data;
}