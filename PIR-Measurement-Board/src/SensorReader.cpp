#include "SensorReader.hpp"

SensorReader::SensorReader(uint8_t dhtPin, uint8_t dhtType, uint8_t soilMoisturePin, uint8_t voltagePin)
    : dht(dhtPin, dhtType), soilMoisturePin(soilMoisturePin), voltagePin(voltagePin) {}

void SensorReader::begin() {
    dht.begin();
}

float SensorReader::readTemperature() {
    float temperature = dht.readTemperature();
    if (isnan(temperature)) {
        Serial.println("Błąd odczytu temperatury z DHT11");
        return -1.0;
    }
    return temperature;
}

float SensorReader::readHumidity() {
    float humidity = dht.readHumidity();
    if (isnan(humidity)) {
        Serial.println("Błąd odczytu wilgotności z DHT11");
        return -1.0;
    }
    return humidity;
}

float SensorReader::readSoilMoisture() {
    int rawValue = analogRead(soilMoisturePin);
    float percentage = map(rawValue, 1023, 0, 0, 100);
    return constrain(percentage, 0, 100);
}

float SensorReader::readVoltage() {
    int rawValue = analogRead(voltagePin);
    float voltage = rawValue * (3.3 / 4095.0);
    return voltage;
}