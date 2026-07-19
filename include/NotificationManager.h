#pragma once

#include <Arduino.h>
#include "NotificationQueue.h"

namespace sentinel {

class TimeManager;

class NotificationManager {
public:
    void begin(TimeManager* timeManager);

    void info(const String& message);
    void success(const String& message);
    void warning(const String& message);
    void error(const String& message);
    void critical(const String& message);

private:
    void log(NotificationLevel level, const String& message);
    NotificationQueue queue_;

    TimeManager* timeManager_ = nullptr;
};

}