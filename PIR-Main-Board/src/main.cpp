#include "LCDManager.hpp"
#include "ButtonManager.hpp"
#include "PumpManager.hpp"
#include "SensorDataManager.hpp"
#include <esp_now.h>
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

// Dane logowania do Blynk
char auth[] = "sb2Uqu8duDhkVwn1YDFb0PjmmZv2fWb0";
char ssid[] = "Eldorado";
char pass[] = "Kowieski08110";

// Inicjalizacja globalnego obiektu Blynk
BlynkTimer timer;

// Funkcja odbierająca dane z ESP-NOW
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    SensorDataManager data;
    memcpy(&data, incomingData, sizeof(SensorDataManager));
    sensorData.update(data.temperature, data.humidity, sensorData.soilMoisture, sensorData.voltage);
}

// Funkcja Blynk: Uruchomienie podlewania z aplikacji
BLYNK_WRITE(V1) {
    int value = param.asInt();
    if (value == 1 && !pump.isRunning()) {
        Blynk.virtualWrite(V6, 1); // Wyślij status pompy do Blynk
        lcd.clear();
        lcd.printMessage("Blynk Watering");
        pump.start();
    }
}

// Funkcja wysyłająca dane do Blynk
void sendSensorDataToBlynk() {
    Blynk.virtualWrite(V2, sensorData.temperature);
    Blynk.virtualWrite(V3, sensorData.humidity);
    Blynk.virtualWrite(V4, sensorData.soilMoisture);
    Blynk.virtualWrite(V5, sensorData.voltage);
}

// Funkcja aktualizująca status pompy w Blynk
void updatePumpStatusInBlynk() {
    if (pump.isRunning()) {
        Blynk.virtualWrite(V6, 1); // Pompa włączona
    } else {
        Blynk.virtualWrite(V6, 0); // Pompa wyłączona
    }
}

void setup() {
    Serial.begin(115200);

    // Inicjalizacja przycisku i pompy
    button.init();
    pump.init();

    // Ustawienie progów alarmowych
    sensorData.setFrostThreshold(-2);
    sensorData.setDrySoilThreshold(25);

    // Inicjalizacja WiFi i ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
    } else {
        esp_now_register_recv_cb(onDataRecv);
    }

    // Inicjalizacja Blynk
    Blynk.begin(auth, ssid, pass);
    timer.setInterval(2000L, sendSensorDataToBlynk); // Wysyłanie danych co 2 sekundy
    timer.setInterval(1000L, updatePumpStatusInBlynk); // Aktualizacja statusu pompy co 1 sekundę

    // Inicjalizacja LCD
    lcd.init();
    lcd.printMessage("Ready!");
}

void loop() {
    Blynk.run();
    timer.run();

    unsigned long pressDuration = button.getPressDuration();

    if (pressDuration > 0) {
        if (pressDuration < 3000) {
            lcd.clear();
            lcd.printMessage("Testing...");
            pump.start();
            Blynk.virtualWrite(V6, 1); // Aktualizacja statusu pompy w Blynk
            delay(500);
            pump.stop();
            Blynk.virtualWrite(V6, 0); // Aktualizacja statusu pompy w Blynk
            lcd.clear();
            lcd.printMessage("Testing done");
            delay(1000);
            lcd.clear();
        } else if (!pump.isRunning()) {
            lcd.clear();
            lcd.printMessage("Watering...");
            pump.start();
            Blynk.virtualWrite(V6, 1); // Aktualizacja statusu pompy w Blynk
        }
    }

    if (pump.isRunning()) {
        unsigned long elapsed = pump.getElapsedTime();
        if (elapsed >= 30) {
            pump.stop();
            Blynk.virtualWrite(V6, 0); // Aktualizacja statusu pompy w Blynk
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
    }

    if (sensorData.isDrySoil() && !pump.isRunning()) {
        lcd.clear();
        lcd.printMessage("Auto Watering", 1);
        pump.start();
        Blynk.virtualWrite(V6, 1); // Aktualizacja statusu pompy w Blynk
    }
}