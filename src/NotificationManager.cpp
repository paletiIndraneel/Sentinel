#include "NotificationManager.h"
#include "TimeManager.h"

namespace sentinel {

void NotificationManager::begin(TimeManager* timeManager)
{
    timeManager_ = timeManager;
}

void NotificationManager::info(const String& message)
{
    log(NotificationLevel::Info, message);
}

void NotificationManager::success(const String& message)
{
    log(NotificationLevel::Success, message);
}

void NotificationManager::warning(const String& message)
{
    log(NotificationLevel::Warning, message);
}

void NotificationManager::error(const String& message)
{
    log(NotificationLevel::Error, message);
}

void NotificationManager::critical(const String& message)
{
    log(NotificationLevel::Critical, message);
}

void NotificationManager::log(NotificationLevel level,const String& message){
    NotificationRecord record;
record.timestamp = "";
record.level = level;
record.message = message;
record.delivered = false;

queue_.enqueue(record);
    String levelText;
    switch (level)
    {
        case NotificationLevel::Info:
            levelText = "INFO";
            break;

        case NotificationLevel::Success:
            levelText = "SUCCESS";
            break;

        case NotificationLevel::Warning:
            levelText = "WARNING";
            break;

        case NotificationLevel::Error:
            levelText = "ERROR";
            break;

        case NotificationLevel::Critical:
            levelText = "CRITICAL";
            break;
    }
    String timestamp = "----";
    if (timeManager_ && timeManager_->isTimeValid())
    {
        timestamp = timeManager_->getTimestamp();
    }
    Serial.printf("%s %-9s %s\n",timestamp.c_str(),levelText.c_str(),message.c_str());
}

}