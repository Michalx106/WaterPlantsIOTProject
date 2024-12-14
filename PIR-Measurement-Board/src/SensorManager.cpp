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
    data.soilMoisture = analogRead(soilMoisturePin) / 4095.0 * 100; // Procenty wilgotności
    data.voltage = analogRead(voltagePin) * (3.3 / 4095.0);          // Napięcie w woltach

    // Walidacja odczytów
    if (isnan(data.temperature) || isnan(data.humidity)) {
        data.temperature = 0;
        data.humidity = 0;
    }

    return data;
}

void SensorManager::printSensorData(const SensorData &data) {
    Serial.println("====================================");
    Serial.println("Zmierzono następujące dane:");
    Serial.printf("Temperatura: %.2f °C\n", data.temperature);
    Serial.printf("Wilgotność: %.2f %%\n", data.humidity);
    Serial.printf("Wilgotność gleby: %.2f %%\n", data.soilMoisture);
    Serial.printf("Napięcie: %.2f V\n", data.voltage);
    Serial.println("====================================");
}
