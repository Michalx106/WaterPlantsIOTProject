#include "DeepSleepManager.hpp"
#include <Arduino.h>

DeepSleepManager::DeepSleepManager(unsigned long sleepTime) : sleepTimeUs(sleepTime) {}

void DeepSleepManager::enterDeepSleep() {
    Serial.printf("Przechodzenie w tryb uśpienia na %.2f minut...\n", sleepTimeUs / 60000000.0);
    esp_sleep_enable_timer_wakeup(sleepTimeUs);
    esp_deep_sleep_start();
}