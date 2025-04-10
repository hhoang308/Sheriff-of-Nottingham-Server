#include "MessageQueue.h"

void MessageQueue::push(const Message &msg)
{
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push(msg);
    cond_.notify_one();
}

Message MessageQueue::pop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]
               { return !queue_.empty(); });

    Message msg = queue_.front();
    queue_.pop();
    return msg;
}
