#include "Button.hpp"

std::vector<std::shared_ptr<Button>> Button::instances;

Button::Button(u_int8_t pinDefinition, u_int8_t mode)
{
    pinNumber = pinDefinition;
    this->mode = mode;
    lastClick = millis();
}

void Button::registerInstance()
{
    instances.push_back(shared_from_this());
}

bool Button::Init()
{
    pinMode(pinNumber, mode);
    registerInstance();
    return true;
}

bool Button::InitAll()
{
    for (const auto& instance : instances) {
        instance->Init();
    }
    return true;
}

u_int8_t Button::getPinNumber()
{
    return pinNumber;
}

bool Button::getState(std::shared_ptr<Button> button)
{
    return button->state;
}

u_int8_t Button::getPinNumber(std::shared_ptr<Button> button)
{
    return button->pinNumber;
}

unsigned long Button::getTimeBetweenClicks()
{
    return timeBetweenClick;
}

unsigned long Button::getPressDuration()
{
    bool currentState = CheckState();
    unsigned long pressDuration = 0;

    if (currentState && !isPressed) {
        pressStartTime = millis();
        isPressed = true;
    } else if (!currentState && isPressed) {
        pressDuration = millis() - pressStartTime;
        isPressed = false;
    }

    return pressDuration;
}

unsigned long Button::setNewTimeBetweenClicks()
{
    unsigned long time = millis() - lastClick;
    lastClick = millis();
    return time;
}

bool Button::CheckState()
{
    return digitalRead(this->pinNumber) ? true : false;
}