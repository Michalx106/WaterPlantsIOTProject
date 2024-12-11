#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <memory>
#include "Button.hpp"

class WateringSystem : public std::enable_shared_from_this<WateringSystem>
{
public:
    WateringSystem(uint8_t pumpPin, std::shared_ptr<Button> button, std::shared_ptr<LiquidCrystal_I2C> lcd);

    void begin();

    void waterWhileButtonPressed();
    void waterAfterHold(unsigned long holdTime, unsigned long waterTime);

    static void handleWaterWhilePressed(WateringSystem& system);
    static void handleWaterAfterHold(WateringSystem& system, unsigned long holdTime, unsigned long waterTime);

private:
    uint8_t pumpPin;
    std::shared_ptr<Button> button;
    std::shared_ptr<LiquidCrystal_I2C> lcd;

    unsigned long wateringStartTime = 0;
    bool isWatering = false;
    void displayRemainingTime(unsigned long remainingTime);
};
