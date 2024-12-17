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

void SensorDataManager::printSensorData() {
    Serial.println("====================================");
    Serial.println("Zmierzono następujące dane:");
    Serial.printf("Temperatura: %.2f °C\n", temperature);
    Serial.printf("Wilgotność: %.2f %%\n", humidity);
    Serial.printf("Wilgotność gleby: %.2f %%\n", soilMoisture);
    Serial.printf("Napięcie: %.2f V\n", voltage);
    Serial.println("====================================");
}
