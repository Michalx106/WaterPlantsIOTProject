#include "SensorDataManager.hpp"

SensorDataManager::SensorDataManager()
    : temperature(0), humidity(0), soilMoisture(50), voltage(0),
      frostThreshold(0), drySoilThreshold(30) {}

void SensorDataManager::update(float temp, float hum, float soil, float volt) {
    temperature = temp;
    humidity = hum;
    soilMoisture = soil;
    voltage = volt;
}

void SensorDataManager::setFrostThreshold(float threshold) {
    frostThreshold = threshold;
}

void SensorDataManager::setDrySoilThreshold(float threshold) {
    drySoilThreshold = threshold;
}

bool SensorDataManager::isFrostAlert() {
    return temperature < frostThreshold;
}

bool SensorDataManager::isDrySoil() {
    return soilMoisture < drySoilThreshold;
}
