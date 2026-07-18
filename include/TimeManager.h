#pragma once

#include <Arduino.h>

namespace sentinel {

class TimeManager {
public:
    void begin();

    bool setFromHttpDate(const String& dateHeader);

    String getTimestamp() const;

    bool isTimeValid() const;

private:
    bool timeValid_ = false;
};

}