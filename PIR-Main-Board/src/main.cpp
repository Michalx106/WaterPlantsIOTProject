#include "ButtonManager.hpp"
#include "LCDManager.hpp"
#include "PumpManager.hpp"
#include "SensorDataManager.hpp"
#include <esp_now.h>
#include <WiFi.h>

#define BUTTON_PIN 23
#define PUMP_PIN 5

LCDManager lcd(0x27, 16, 2);
ButtonManager button(BUTTON_PIN);
PumpManager pump(PUMP_PIN);
SensorDataManager sensorData;

static unsigned long lastReceivedTime = 0;

void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    SensorDataManager data;
    memcpy(&data, incomingData, sizeof(SensorDataManager));
    sensorData.update(data.temperature, data.humidity, data.soilMoisture, data.voltage);
    lastReceivedTime = millis();
}

void setup() {
    Serial.begin(115200);
    lcd.init();
    button.init();
    pump.init();

    sensorData.setFrostThreshold(-2);
    sensorData.setDrySoilThreshold(25);

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
    } else {
        esp_now_register_recv_cb(onDataRecv);
    }

    lcd.printMessage("Ready!");
}

void loop() {
    unsigned long pressDuration = button.getPressDuration();

    if (pressDuration > 0) {
        if (pressDuration < 3000) {
            lcd.clear();
            lcd.printMessage("Testing...");
            pump.start();
            delay(500);
            pump.stop();
            lcd.clear();
            lcd.printMessage("Testing done");
            delay(1000);
            lcd.clear();
        } else if (!pump.isRunning()) {
            lcd.clear();
            lcd.printMessage("Watering...");
            pump.start();
        }
    }

    if (pump.isRunning()) {
        unsigned long elapsed = pump.getElapsedTime();
        if (elapsed >= 30) {
            pump.stop();
            lcd.clear();
            lcd.printMessage("Watering done", 1);
            delay(2000);
            lcd.clear();
        } else {
            unsigned long remaining = 30 - elapsed;
            lcd.printMessage("Time left: " + String(remaining) + "s", 1);
        }
    } else {
        String scrollText = "Temp: " + String(sensorData.temperature, 1) + "C Hum: " +
                            String(sensorData.humidity, 1) + "% Soil: " +
                            String(sensorData.soilMoisture, 1) + "% Volt: " +
                            String(sensorData.voltage, 1) + "V";

        lcd.scrollText(scrollText, 0);

        if (millis() - lastReceivedTime > 10000) {
            lcd.printMessage("No data!", 1);
        } else if (sensorData.isFrostAlert()) {
            if (millis() % 1000 < 500) {
                lcd.printMessage("Frost Alert!", 1);
            } else {
                lcd.printMessage("                ", 1);
            }
        } else if (sensorData.isDrySoil()) {
            if (millis() % 1000 < 500) {
                lcd.printMessage("Dry Soil!", 1);
            } else {
                lcd.printMessage("                ", 1);
            }
        } else {
            lcd.printMessage("                ", 1);
        }
    }

    if (sensorData.isDrySoil() && !pump.isRunning()) {
        lcd.clear();
        lcd.printMessage("Auto Watering", 1);
        pump.start();
    }

    delay(100);
}
