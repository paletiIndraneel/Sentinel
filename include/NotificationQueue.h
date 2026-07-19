#pragma once

#include <Arduino.h>
#include <deque>

enum class NotificationLevel
{
    Info,
    Success,
    Warning,
    Error,
    Critical
};

struct NotificationRecord
{
    String timestamp;
    NotificationLevel level;
    String message;
    bool delivered;
};

class NotificationQueue
{
public:
    NotificationQueue();
    void enqueue(const NotificationRecord& notification);
    bool hasPending() const;
    const NotificationRecord& front() const;
    void dequeue();


private:
    std::deque<NotificationRecord> queue_;
};