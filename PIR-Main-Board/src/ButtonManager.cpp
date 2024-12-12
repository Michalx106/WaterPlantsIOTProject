#include "ButtonManager.hpp"

ButtonManager *ButtonManager::instance = nullptr;

ButtonManager::ButtonManager(uint8_t buttonPin) 
    : pin(buttonPin), buttonPressed(false), buttonPressStart(0), buttonPressEnd(0) {
    instance = this;
}

void ButtonManager::init() {
    pinMode(pin, INPUT_PULLUP);
    attachInterrupt(pin, isr, CHANGE);
}

void IRAM_ATTR ButtonManager::isr() {
    if (digitalRead(instance->pin) == LOW) {
        if (!instance->buttonPressed) {
            instance->buttonPressStart = millis();
            instance->buttonPressed = true;
        }
    } else {
        if (instance->buttonPressed) {
            instance->buttonPressEnd = millis();
            instance->buttonPressed = false;
        }
    }
}

unsigned long ButtonManager::getPressDuration() {
    if (buttonPressEnd > 0) {
        unsigned long duration = buttonPressEnd - buttonPressStart;
        buttonPressEnd = 0;
        return duration;
    }
    return 0;
}
