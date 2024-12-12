#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class LCDManager {
private:
    LiquidCrystal_I2C lcd;
    unsigned long lastScrollTime;
    int scrollPosition;

public:
    LCDManager(uint8_t address, uint8_t cols, uint8_t rows);
    void init();
    void clear();
    void printMessage(const String &message, int line = 0);
    void scrollText(const String &text, int line);
};
