#pragma once

#include <esp_now.h>
#include <WiFi.h>
#include "SensorManager.hpp"

class ESPNowManager {
private:
    uint8_t peerAddress[6];

public:
    ESPNowManager(const uint8_t *address);
    void begin();
    bool sendData(const SensorData &data);
};