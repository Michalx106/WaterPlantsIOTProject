#include "ESPNowManager.hpp"
#include "LCDManager.hpp"
#include "ButtonManager.hpp"
#include "PumpManager.hpp"
#include "SensorDataManager.hpp"
#include <esp_wifi.h>
#include <WiFi.h>

#define BUTTON_PIN 23
#define PUMP_PIN 5

#define BLYNK_TEMPLATE_ID "TMPL4lrKLMEOn"
#define BLYNK_TEMPLATE_NAME "PIR centralka"
#include <BlynkSimpleEsp32.h>

LCDManager lcd(0x27, 16, 2);
ButtonManager button(BUTTON_PIN);
PumpManager pump(PUMP_PIN);
SensorDataManager sensorData;
ESPNowManager espNowManager;
static unsigned long lastReceivedTime = 0;

void sendSensorDataToBlynk();

char auth[] = "sb2Uqu8duDhkVwn1YDFb0PjmmZv2fWb0";
char ssid[] = "Eldorado";
char pass[] = "Kowieski08110";

typedef struct struct_message {
    float temperature;
    float humidity;
    float soilMoisture;
    float voltage;
} struct_message;

struct_message incomingData;

void onDataRecv(const uint8_t *mac, const uint8_t *incomingDataRaw, int len) {
    memcpy(&incomingData, incomingDataRaw, sizeof(incomingData));

    sensorData.update(incomingData.temperature, incomingData.humidity, incomingData.soilMoisture, incomingData.voltage);
    sensorData.printSensorData();
    lastReceivedTime = millis();

    Serial.println("ESP-NOW data received and updated.");
    sendSensorDataToBlynk();
}

void sendSensorDataToBlynk() {
    Blynk.virtualWrite(V2, sensorData.temperature);
    Blynk.virtualWrite(V3, sensorData.humidity);
    Blynk.virtualWrite(V4, sensorData.soilMoisture);
    Blynk.virtualWrite(V5, sensorData.voltage);
    Serial.println("Sensor data sent to Blynk.");
}

BLYNK_WRITE(V1) {
    int value = param.asInt();
    if (value == 1 && !pump.isRunning()) {
        Blynk.virtualWrite(V6, 1);
        lcd.clear();
        lcd.printMessage("Blynk Watering");
        pump.start();
        Serial.println("Pump started via Blynk.");
    }
}

void setup() {
    Serial.begin(115200);

    lcd.init();
    lcd.clear();
    lcd.printMessage("Ready!");

    button.init();
    pump.init();

    sensorData.setFrostThreshold(12);
    sensorData.setDrySoilThreshold(25);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi Connected");

    if (!espNowManager.begin(onDataRecv)) {
        while (1);
    }

    Blynk.begin(auth, ssid, pass);

    esp_wifi_set_ps(WIFI_PS_NONE);
    Serial.print("Wi-Fi Channel: ");
    Serial.println(WiFi.channel());

    Serial.println("Setup complete.");
}

void loop() {
    Blynk.run();

    unsigned long pressDuration = button.getPressDuration();

    if (pressDuration > 0) {
        if (pressDuration < 3000) { // Krótkie naciśnięcie
            lcd.clear();
            lcd.printMessage("Testing...");
            pump.start();
            Blynk.virtualWrite(V6, 1);
            delay(500);
            pump.stop();
            Blynk.virtualWrite(V6, 0);
            lcd.clear();
            lcd.printMessage("Testing done");
            delay(1000);
            lcd.clear();
            Serial.println("Short button press: Pump tested.");
        } else if (!pump.isRunning()) { // Długie naciśnięcie
            lcd.clear();
            lcd.printMessage("Watering...");
            pump.start();
            Blynk.virtualWrite(V6, 1);
            Serial.println("Long button press: Pump started.");
        }
    }

    if (pump.isRunning()) {
        unsigned long elapsed = pump.getElapsedTime();
        if (elapsed >= 30) {
            pump.stop();
            Blynk.virtualWrite(V6, 0);
            lcd.clear();
            lcd.printMessage("Watering done", 1);
            delay(2000);
            lcd.clear();
            Serial.println("Pump stopped after 30 seconds.");
        } else {
            unsigned long remaining = 30 - elapsed;
            lcd.printMessage("Time left: " + String(remaining) + "s", 1);
        }
    }

    if (sensorData.isDrySoil() && !pump.isRunning()) {
        lcd.clear();
        lcd.printMessage("Auto Watering", 1);
        pump.start();
        Blynk.virtualWrite(V6, 1);
        Serial.println("Auto watering triggered.");
    }

    if(!pump.isRunning())
    {
        lcd.clear();
        lcd.printMessage("Temp: " + String((int)sensorData.temperature) + " Hum:" + String((int)sensorData.humidity) + "%", 0);
        if (millis() - lastReceivedTime > 100000) 
        {
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
    delay(500);
}
