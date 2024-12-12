#include "PumpManager.hpp"

PumpManager::PumpManager(uint8_t pumpPin) : pin(pumpPin), isActive(false), startTime(0) {}

void PumpManager::init() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void PumpManager::start() {
    isActive = true;
    startTime = millis();
    digitalWrite(pin, HIGH);
}

void PumpManager::stop() {
    isActive = false;
    digitalWrite(pin, LOW);
}

bool PumpManager::isRunning() {
    return isActive;
}

unsigned long PumpManager::getElapsedTime() {
    return (millis() - startTime) / 1000;
}
