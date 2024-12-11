#pragma once

#include <esp_now.h>
#include <WiFi.h>
#include <memory>
#include <functional>

class EspNowManager {
public:
    using OnDataReceivedCallback = std::function<void(const uint8_t *macAddr, const uint8_t *data, int len)>;
    using OnDataSentCallback = std::function<void(const uint8_t *macAddr, esp_now_send_status_t status)>;

    EspNowManager();

    bool begin();

    void onReceive(OnDataReceivedCallback callback);
    void onSend(OnDataSentCallback callback);

    bool addPeer(const uint8_t *peerAddress);

    bool sendData(const uint8_t *peerAddress, const uint8_t *data, size_t len);

private:
    std::shared_ptr<OnDataReceivedCallback> dataReceivedCallback;
    std::shared_ptr<OnDataSentCallback> dataSentCallback;

    static void handleDataReceived(const uint8_t *macAddr, const uint8_t *data, int len);
    static void handleDataSent(const uint8_t *macAddr, esp_now_send_status_t status);

    static std::shared_ptr<OnDataReceivedCallback> staticDataReceivedCallback;
    static std::shared_ptr<OnDataSentCallback> staticDataSentCallback;
};
