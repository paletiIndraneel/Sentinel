#include "TimeManager.h"
#include <time.h>
#include <sys/time.h>

namespace sentinel {

void TimeManager::begin() {

    setenv("TZ", "IST-5:30", 1);
    tzset();
}

bool TimeManager::setFromHttpDate(const String& dateHeader) {
    struct tm tm = {};
    // Example:Sat, 19 Jul 2026 03:24:15 GMT
    if (strptime(dateHeader.c_str(),"%a, %d %b %Y %H:%M:%S GMT",&tm) == nullptr) {
        Serial.println("Failed to parse HTTP date.");
        return false;
    }
    // Convert to Unix epoch
    // mktime() assumes local time (IST), so subtract the IST offset
    // Convert using UTC
    setenv("TZ", "UTC0", 1);
    tzset();

    time_t utc = mktime(&tm);

    struct timeval tv;
    tv.tv_sec = utc;
    tv.tv_usec = 0;
    if (settimeofday(&tv, nullptr) != 0){
        return false;
    }
    setenv("TZ", "IST-5:30", 1);
    tzset();
    timeValid_ = true;
    return true;
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