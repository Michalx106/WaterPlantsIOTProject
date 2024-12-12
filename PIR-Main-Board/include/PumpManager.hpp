#pragma once

#include <Arduino.h>

class PumpManager {
private:
    uint8_t pin;
    bool isActive;
    unsigned long startTime;

public:
    PumpManager(uint8_t pumpPin);
    void init();
    void start();
    void stop();
    bool isRunning();
    unsigned long getElapsedTime();
};
