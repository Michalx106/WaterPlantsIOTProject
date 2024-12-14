#pragma once

class DeepSleepManager {
private:
    unsigned long sleepTimeUs;

public:
    DeepSleepManager(unsigned long sleepTime);
    void enterDeepSleep();
};
