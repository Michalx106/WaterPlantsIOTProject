#pragma once

#include <DHT.h>
#include <Arduino.h>

class SensorReader {
public:
    SensorReader(uint8_t dhtPin, uint8_t dhtType, uint8_t soilMoisturePin, uint8_t voltagePin);

    void begin();

    float readTemperature();
    float readHumidity();
    float readSoilMoisture();
    float readVoltage();

private:
    DHT dht;
    uint8_t soilMoisturePin;
    uint8_t voltagePin;
};