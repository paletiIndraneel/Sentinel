#include "NotificationManager.h"
#include "TimeManager.h"
#include "TelegramBot.h"

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

void NotificationManager::processQueue(TelegramBot& telegram,bool internetAvailable){
    if (!internetAvailable)
    {
        return;
    }

    if (!telegram.isReady())
    {
        return;
    }

    if (!queue_.hasPending())
    {
        return;
    }

    const uint32_t now = millis();

    if (now - lastSendAttemptMs_ < 1000)
    {
        return;
    }

    lastSendAttemptMs_ = now;

    const NotificationRecord& record = queue_.front();

    if (telegram.sendMessage(record.message.c_str()))
    {
        Serial.printf("[Notification] Sent: %s\n", record.message.c_str());
        queue_.dequeue();
    }
    else
    {
        Serial.printf("[Notification] Send failed, will retry.\n");
    }
}


void NotificationManager::log(NotificationLevel level,const String& message)
{
    // Get timestamp
    String timestamp = "----";

    if (timeManager_ && timeManager_->isTimeValid())
    {
        timestamp = timeManager_->getTimestamp();
    }

    // Create notification
    NotificationRecord record;
    record.timestamp = timestamp;
    record.level = level;
    record.message = message;
    record.delivered = false;

    // Store notification
    queue_.enqueue(record);

    // Convert level to text
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

    // Serial log
    Serial.printf("%s %-9s %s\n",
                  timestamp.c_str(),
                  levelText.c_str(),
                  message.c_str());
}

} // namespace sentinel