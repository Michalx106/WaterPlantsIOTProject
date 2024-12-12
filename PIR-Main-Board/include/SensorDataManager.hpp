#pragma once

#include <Arduino.h>

class SensorDataManager {
private:
    float frostThreshold;
    float drySoilThreshold;

public:
    float temperature;
    float humidity;
    float soilMoisture;
    float voltage;

    SensorDataManager();
    void update(float temp, float hum, float soil, float volt);
    bool isFrostAlert();
    bool isDrySoil();
    void setFrostThreshold(float threshold);
    void setDrySoilThreshold(float threshold);
};
