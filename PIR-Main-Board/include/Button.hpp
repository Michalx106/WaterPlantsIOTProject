#pragma once
#include <Arduino.h>
#include <memory>
#include <vector>

class Button : public std::enable_shared_from_this<Button>
{
    public:
    Button(u_int8_t pinDefinition, u_int8_t mode);

    void registerInstance();
    bool Init();
    static bool InitAll();
    bool CheckState();

    u_int8_t getPinNumber();
    static u_int8_t getPinNumber(std::shared_ptr<Button> button);
    static bool getState(std::shared_ptr<Button> button);
    unsigned long getTimeBetweenClicks();

    unsigned long getPressDuration();
    
    template <typename T, typename... Args>
    static void LoopFunction(Button button, unsigned long interval, T func, Args... args)
    {
        button.state = button.CheckState();
        button.timeBetweenClick = button.setNewTimeBetweenClicks();

        if (button.timeBetweenClick >= interval && button.state) func(std::make_shared<Button>(button), args...);
    }

    private:
    static std::vector<std::shared_ptr<Button>> instances;

    u_int8_t pinNumber;
    u_int8_t mode;
    bool state = false;
    unsigned long lastClick = 0;
    unsigned long timeBetweenClick = 0;
    unsigned long pressStartTime = 0;
    bool isPressed = false; 

    unsigned long setNewTimeBetweenClicks();
};