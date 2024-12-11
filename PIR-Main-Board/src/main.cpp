#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Button.hpp"
#include "EspNowManager.hpp"
#include "WateringSystem.hpp"
#include "DataStruct.hpp"

#define PUMP_PIN 5
#define BUTTON_PIN 23

auto button = std::make_shared<Button>(BUTTON_PIN, INPUT_PULLUP);
auto lcd = std::make_shared<LiquidCrystal_I2C>(0x27, 16, 2);
auto wateringSystem = std::make_shared<WateringSystem>(PUMP_PIN, button, lcd);
EspNowManager espNow;
SensorData currentSensorData;

void onDataReceived(const uint8_t *macAddr, const uint8_t *data, int len);

void setup() {
    Serial.begin(115200);
    wateringSystem->begin();
    
    if (espNow.begin()) {
        espNow.onReceive(onDataReceived);
    } else {
        Serial.println("Błąd inicjalizacji ESP-NOW");
    }
}

void loop() {
    Button::LoopFunction(*button, 3000, WateringSystem::handleWaterWhilePressed, *wateringSystem);
    Button::LoopFunction(*button, 3000, WateringSystem::handleWaterAfterHold, *wateringSystem, 3000, 30000);
}

void onDataReceived(const uint8_t *macAddr, const uint8_t *data, int len) {
    if (len == sizeof(SensorData)) {
        SensorData sensorData;
        memcpy(&sensorData, data, sizeof(SensorData));

        Serial.print("Temperatura: ");
        Serial.println(sensorData.temperature);
        Serial.print("Wilgotność: ");
        Serial.println(sensorData.humidity);
        Serial.print("Wilgotność gleby: ");
        Serial.println(sensorData.soilMoisture);
        Serial.print("Napięcie: ");
        Serial.println(sensorData.voltage);
    } else {
        Serial.println("Nieprawidłowe dane");
    }
}