#include "LCDManager.hpp"
#include "ButtonManager.hpp"
#include "PumpManager.hpp"
#include "SensorDataManager.hpp"
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

#define BUTTON_PIN 23
#define PUMP_PIN 5

#define BLYNK_TEMPLATE_ID "TMPL4lrKLMEOn"
#define BLYNK_TEMPLATE_NAME "PIR centralka"
#include <BlynkSimpleEsp32.h>

// Inicjalizacja LCD, przycisku, pompy i czujników
LCDManager lcd(0x27, 16, 2);
ButtonManager button(BUTTON_PIN);
PumpManager pump(PUMP_PIN);
SensorDataManager sensorData;

// Dane logowania Blynk
char auth[] = "sb2Uqu8duDhkVwn1YDFb0PjmmZv2fWb0";
char ssid[] = "dlinkjarka";
char pass[] = "1230984567";

BlynkTimer timer;

// Funkcja callback do odbierania danych ESP-NOW
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    SensorDataManager data;
    memcpy(&data, incomingData, sizeof(SensorDataManager));
    sensorData.update(data.temperature, data.humidity, sensorData.soilMoisture, sensorData.voltage);
    sensorData.printSensorData();
    Serial.println("ESP-NOW data received and updated.");
}

// Obsługa przycisku w Blynk do uruchamiania pompy
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

// Funkcja do wysyłania danych czujników do Blynk
void sendSensorDataToBlynk() {
    Blynk.virtualWrite(V2, sensorData.temperature);
    Blynk.virtualWrite(V3, sensorData.humidity);
    Blynk.virtualWrite(V4, sensorData.soilMoisture);
    Blynk.virtualWrite(V5, sensorData.voltage);
    Serial.println("Sensor data sent to Blynk.");
}

// Aktualizacja statusu pompy w Blynk
void updatePumpStatusInBlynk() {
    Blynk.virtualWrite(V6, pump.isRunning() ? 1 : 0);
}

void setup() {
    Serial.begin(115200);

    // Inicjalizacja LCD
    lcd.init();
    lcd.printMessage("Ready!");

    // Inicjalizacja przycisku i pompy
    button.init();
    pump.init();

    // Ustawienie progów alarmowych
    sensorData.setFrostThreshold(-2);
    sensorData.setDrySoilThreshold(25);

    // Konfiguracja Wi-Fi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi Connected");

    // Konfiguracja ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW.");
        return;
    }
    esp_now_register_recv_cb(onDataRecv);
    esp_wifi_set_ps(WIFI_PS_NONE); // Wyłącz oszczędzanie energii dla ESP-NOW
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    // Inicjalizacja Blynk
    Blynk.begin(auth, ssid, pass);
    timer.setInterval(20000L, sendSensorDataToBlynk); // Wysyłanie danych do Blynk co 2 sekundy
    timer.setInterval(10000L, updatePumpStatusInBlynk); // Aktualizacja statusu pompy co 1 sekundę

    Serial.println("Setup complete.");
}

void loop() {
    Blynk.run();
    timer.run();

    unsigned long pressDuration = button.getPressDuration();

    // Obsługa ręcznego przycisku do uruchamiania pompy
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

    // Automatyczne zatrzymanie pompy po 30 sekundach
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

    // Automatyczne podlewanie w przypadku suchej gleby
    if (sensorData.isDrySoil() && !pump.isRunning()) {
        lcd.clear();
        lcd.printMessage("Auto Watering", 1);
        pump.start();
        Blynk.virtualWrite(V6, 1);
        Serial.println("Auto watering triggered.");
    }
}
