#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>

struct Message
{
    const std::string content;
    int socketID;
};

class MessageQueue
{
public:
    void push(const Message &msg);

    Message pop();

private:
    std::queue<Message> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

#endif