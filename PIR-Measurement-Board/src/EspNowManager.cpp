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
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Błąd dodania odbiornika ESP-NOW.");
    }
}

bool ESPNowManager::sendData(const SensorData &data) {
    esp_err_t result = esp_now_send(peerAddress, (uint8_t *)&data, sizeof(SensorData));
    return result == ESP_OK;
}