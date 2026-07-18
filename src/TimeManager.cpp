#include "TimeManager.h"
#include <time.h>

namespace sentinel {

void TimeManager::begin() {

    setenv("TZ", "IST-5:30", 1);
    tzset();
}

bool TimeManager::setFromHttpDate(const String& dateHeader) {
    Serial.println();
    Serial.println("========== HTTP DATE ==========");
    Serial.println(dateHeader);
    Serial.println("===============================");
    return false;
}

String TimeManager::getTimestamp() const {
    time_t now;
    time(&now);
    struct tm info;
    localtime_r(&now, &info);
    char buffer[32];
    strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",&info);
    return String(buffer);
}

bool TimeManager::isTimeValid() const {
    return timeValid_;
}

}