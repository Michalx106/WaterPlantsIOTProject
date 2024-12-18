#pragma once

#include <esp_now.h>
#include <WiFi.h>

class ESPNowManager {
public:
    ESPNowManager() {}

    bool begin(void (*recvCallback)(const uint8_t *, const uint8_t *, int)) {
        WiFi.mode(WIFI_STA);
        if (esp_now_init() != ESP_OK) {
            Serial.println("Error initializing ESP-NOW.");
            return false;
        }
        Serial.println("ESP-NOW initialized successfully.");

        esp_now_register_recv_cb(recvCallback);
        return true;
    }

    bool addPeer(const uint8_t *peerAddress) {
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, peerAddress, 6);
        peerInfo.channel = 0;  // Domyślny kanał
        peerInfo.encrypt = false;

        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
            Serial.println("Failed to add peer.");
            return false;
        }
        Serial.println("Peer added successfully.");
        return true;
    }

    bool sendData(const uint8_t *peerAddress, const uint8_t *data, size_t len) {
        esp_err_t result = esp_now_send(peerAddress, data, len);
        if (result == ESP_OK) {
            Serial.println("Data sent successfully.");
            return true;
        } else {
            Serial.println("Error sending data.");
            return false;
        }
    }

private:
};
