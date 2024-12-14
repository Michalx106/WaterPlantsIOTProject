#pragma once

#include <DHT.h>

struct SensorData {
    float temperature;
    float humidity;
    float soilMoisture;
    float voltage;
};

class SensorManager {
private:
    DHT dht;
    int soilMoisturePin;
    int voltagePin;

public:
    SensorManager(int dhtPin, int dhtType, int soilPin, int voltagePin);
    void begin();
    SensorData readSensors();
    void printSensorData(const SensorData &data); // Nowa funkcja do wypisywania danych
};
