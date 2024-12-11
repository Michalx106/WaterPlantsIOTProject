#include "WateringSystem.hpp"

WateringSystem::WateringSystem(uint8_t pumpPin, std::shared_ptr<Button> button, std::shared_ptr<LiquidCrystal_I2C> lcd)
    : pumpPin(pumpPin), button(button), lcd(lcd) {}

void WateringSystem::begin() {
    pinMode(pumpPin, OUTPUT);
    digitalWrite(pumpPin, LOW);
    button->Init();

    lcd->init();
    lcd->backlight();
    lcd->print("System ready");
    delay(2000);
    lcd->clear();
}

void WateringSystem::waterWhileButtonPressed() {
    digitalWrite(pumpPin, button->CheckState() ? HIGH : LOW);
}

void WateringSystem::waterAfterHold(unsigned long holdTime, unsigned long waterTime) {
    unsigned long pressDuration = button->getPressDuration();

    if (pressDuration >= holdTime && !isWatering) {
        digitalWrite(pumpPin, HIGH);
        isWatering = true;
        wateringStartTime = millis();

        lcd->setCursor(0, 0);
        lcd->print("Watering: ON  ");
    }

    if (isWatering) {
        unsigned long elapsedTime = millis() - wateringStartTime;
        unsigned long remainingTime = (elapsedTime < waterTime) ? (waterTime - elapsedTime) / 1000 : 0;

        displayRemainingTime(remainingTime);

        if (elapsedTime >= waterTime) {
            digitalWrite(pumpPin, LOW);
            isWatering = false;

            lcd->setCursor(0, 0);
            lcd->print("Watering: OFF ");
        }
    }
}

void WateringSystem::handleWaterWhilePressed(std::shared_ptr<Button> button, WateringSystem& system) {
    system.waterWhileButtonPressed();
}

void WateringSystem::handleWaterAfterHold(std::shared_ptr<Button> button, WateringSystem& system, unsigned long holdTime, unsigned long waterTime) {
    system.waterAfterHold(holdTime, waterTime);
}

void WateringSystem::displayRemainingTime(unsigned long remainingTime) {
    lcd->setCursor(0, 1);
    lcd->print("Time left: ");
    lcd->print(remainingTime);
    lcd->print("s   ");
}