#include "Button.hpp"

Button::Button(u_int8_t pinDefinition, u_int8_t mode)
{
    pinNumber = pinDefinition;
    mode = mode;
    lastClick = millis();
    instances.push_back(shared_from_this());
}

bool Button::Init()
{
    pinMode(pinNumber, mode);
}

bool Button::InitAll()
{
    for (const auto& instance : instances) {
        instance->Init();
    }
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
        // Rozpoczęcie wciśnięcia
        pressStartTime = millis();
        isPressed = true;
    } else if (!currentState && isPressed) {
        // Zakończenie wciśnięcia
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
    return digitalRead(pinNumber) ? true : false;
}

template <typename T, typename... Args>
void Button::LoopFunction(std::shared_ptr<Button> button, unsigned long interval, T func, Args... args)
{
    button.state = CheckState();
    button.timeBetweenClick = setNewTimeBetweenClicks();

    if(button.timeBetweenClick >= interval && button.state) func(button, args);
}