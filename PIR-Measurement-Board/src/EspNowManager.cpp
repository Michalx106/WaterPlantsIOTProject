#include "ESPNowManager.hpp"
#include <Arduino.h>

ESPNowManager::ESPNowManager(const uint8_t *address) {
    memcpy(peerAddress, address, 6);
}

void ESPNowManager::begin() {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Błąd inicjalizacji ESP-NOW");
    }

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 1;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Błąd dodania odbiornika ESP-NOW.");
    }
}

bool ESPNowManager::sendData(const SensorData &data) {
    const int maxRetries = 3;
    int attempt = 0;
    esp_err_t result;

    do {
        attempt++;
        Serial.printf("Wysyłanie danych, próba %d...\n", attempt);
        result = esp_now_send(peerAddress, (uint8_t *)&data, sizeof(SensorData));

        if (result == ESP_OK) {
            Serial.println("Dane wysłane pomyślnie.");
            return true;
        } else {
            Serial.println("Błąd wysyłania danych.");
            delay(500);
        }
    } while (attempt < maxRetries);

    Serial.println("Nie udało się wysłać danych po maksymalnej liczbie prób.");
    return false;
}
