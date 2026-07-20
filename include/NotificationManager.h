#pragma once

#include <Arduino.h>
#include "NotificationQueue.h"

namespace sentinel
{

class TimeManager;
}

class TelegramBot;

namespace sentinel
{

class NotificationManager
{
public:
    void begin(TimeManager* timeManager);

    void info(const String& message);
    void success(const String& message);
    void warning(const String& message);
    void error(const String& message);
    void critical(const String& message);

    void processQueue(TelegramBot& telegram,
                      bool internetAvailable);

private:
    void log(NotificationLevel level,const String& message);
    NotificationQueue queue_;
    TimeManager* timeManager_ = nullptr;
    uint32_t lastSendAttemptMs_ = 0;
};

}