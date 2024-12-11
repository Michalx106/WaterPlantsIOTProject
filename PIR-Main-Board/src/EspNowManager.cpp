#include "EspNowManager.hpp"

std::shared_ptr<EspNowManager::OnDataReceivedCallback> EspNowManager::staticDataReceivedCallback = nullptr;
std::shared_ptr<EspNowManager::OnDataSentCallback> EspNowManager::staticDataSentCallback = nullptr;

EspNowManager::EspNowManager()
    : dataReceivedCallback(std::make_shared<OnDataReceivedCallback>(nullptr)),
      dataSentCallback(std::make_shared<OnDataSentCallback>(nullptr)) {}

bool EspNowManager::begin() {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Błąd inicjalizacji ESP-NOW");
        return false;
    }
    esp_now_register_recv_cb(handleDataReceived);
    esp_now_register_send_cb(handleDataSent);
    return true;
}

void EspNowManager::onReceive(OnDataReceivedCallback callback) {
    *dataReceivedCallback = callback;
    staticDataReceivedCallback = dataReceivedCallback;
}

void EspNowManager::onSend(OnDataSentCallback callback) {
    *dataSentCallback = callback;
    staticDataSentCallback = dataSentCallback;
}

bool EspNowManager::addPeer(const uint8_t *peerAddress) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Błąd dodawania odbiorcy");
        return false;
    }
    return true;
}

bool EspNowManager::sendData(const uint8_t *peerAddress, const uint8_t *data, size_t len) {
    esp_err_t result = esp_now_send(peerAddress, data, len);
    if (result != ESP_OK) {
        Serial.println("Błąd wysyłania danych");
        return false;
    }
    return true;
}

void EspNowManager::handleDataReceived(const uint8_t *macAddr, const uint8_t *data, int len) {
    if (staticDataReceivedCallback && *staticDataReceivedCallback) {
        (*staticDataReceivedCallback)(macAddr, data, len);
    }
}

void EspNowManager::handleDataSent(const uint8_t *macAddr, esp_now_send_status_t status) {
    if (staticDataSentCallback && *staticDataSentCallback) {
        (*staticDataSentCallback)(macAddr, status);
    }
}
