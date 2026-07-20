#include "NotificationQueue.h"

NotificationQueue::NotificationQueue()
{
}

void NotificationQueue::enqueue(const NotificationRecord& notification)
{
    queue_.push_back(notification);
}

bool NotificationQueue::hasPending() const
{
    return !queue_.empty();
}

const NotificationRecord& NotificationQueue::front() const
{
    return queue_.front();
}

void NotificationQueue::dequeue()
{
    if (!queue_.empty())
    {
        queue_.pop_front();
    }
}

size_t NotificationQueue::size() const
{
    return queue_.size();
}
    void NotificationQueue::clear()
    {
        queue_.clear();
    }