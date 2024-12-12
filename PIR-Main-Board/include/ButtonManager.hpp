#pragma once

#include <Arduino.h>

class ButtonManager {
private:
    uint8_t pin;
    volatile bool buttonPressed;
    volatile unsigned long buttonPressStart;
    volatile unsigned long buttonPressEnd;

public:
    ButtonManager(uint8_t buttonPin);
    void init();
    static void IRAM_ATTR isr();
    unsigned long getPressDuration();

    static ButtonManager *instance;
};
