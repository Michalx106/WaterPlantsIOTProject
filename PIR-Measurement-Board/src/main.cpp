#include <Adafruit_Sensor.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include "DHT.h"

#define DHT_PIN 18
#define DHT_TYPE DHT11
#define SOIL_MOISTURE_PIN 34

typedef struct struct_message {
    float temperature;
    float humidity;
    float soilMoisture;
    float voltage;
} struct_message;

struct_message outgoingData;
DHT dht(DHT_PIN, DHT_TYPE);

uint8_t receiverAddress[] = {0x10, 0x06, 0x1C, 0x82, 0xF7, 0xD0}; // Wstaw adres MAC odbiornika ESP32

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Dane zostały wysłane. Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sukces" : "Niepowodzenie");
}

void setup() {
    Serial.begin(115200);

    dht.begin();
    pinMode(SOIL_MOISTURE_PIN, INPUT);

    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(6, WIFI_SECOND_CHAN_NONE);
    Serial.println("WiFi ustawiono na kanał 6.");

    if (esp_now_init() != ESP_OK) {
        Serial.println("Błąd inicjalizacji ESP-NOW");
        return;
    }
    Serial.println("ESP-NOW zainicjalizowano pomyślnie.");

    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 6;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Błąd dodawania odbiorcy.");
        return;
    }
    Serial.println("Odbiorca dodany pomyślnie.");
}

void loop() {
    outgoingData.temperature = dht.readTemperature();
    outgoingData.humidity = dht.readHumidity();
    int soilValue = analogRead(SOIL_MOISTURE_PIN);
    outgoingData.voltage = soilValue * (3.3 / 4095.0);
    outgoingData.soilMoisture = (3.3 - outgoingData.voltage) / (3.3 - 0.9) * 100 + 50;

    if (outgoingData.soilMoisture > 100) outgoingData.soilMoisture = 100;
    if (outgoingData.soilMoisture < 0) outgoingData.soilMoisture = 0;

    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));

    if (result == ESP_OK) {
        Serial.println("Dane wysłane pomyślnie.");
    } else {
        Serial.println("Błąd wysyłania danych.");
    }

    Serial.print("Wysłane dane -> Temp: ");
    Serial.print(outgoingData.temperature);
    Serial.print("°C, Humidity: ");
    Serial.print(outgoingData.humidity);
    Serial.print("%, Soil Moisture: ");
    Serial.print(outgoingData.soilMoisture);
    Serial.print(", Voltage: ");
    Serial.println(outgoingData.voltage);

    Serial.println("Przejście w tryb Deep Sleep na 1 minutę...");
    esp_sleep_enable_timer_wakeup(60 * 1000000ULL); // 60 sekund w mikrosekundach
    esp_deep_sleep_start();
}
