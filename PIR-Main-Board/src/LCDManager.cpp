#include "LCDManager.hpp"

LCDManager::LCDManager(uint8_t address, uint8_t cols, uint8_t rows) 
    : lcd(address, cols, rows), lastScrollTime(0), scrollPosition(0) {}

void LCDManager::init() {
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
}

void LCDManager::clear() {
    lcd.clear();
}

void LCDManager::printMessage(const String &message, int line) {
    lcd.setCursor(0, line);
    lcd.print(message);
}

void LCDManager::scrollText(const String &text, int line) {
    if (millis() - lastScrollTime > 300) {
        lastScrollTime = millis();
        int textLength = text.length();
        lcd.setCursor(0, line);
        lcd.print(text.substring(scrollPosition, scrollPosition + 16));
        scrollPosition++;
        if (scrollPosition > textLength - 16) {
            scrollPosition = 0;
        }
    }
}
