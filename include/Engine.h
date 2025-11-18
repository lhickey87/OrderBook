#include "Logger.h"
#include "Message.h"
#include "Orderbook.h"
#include <cstdint>
#include <unordered_map>

using LogQueue = LFQueue<LogElement>*;
using MessageQueue = LFQueue<Message*>;


class Engine {

public:
    explicit Engine(const LogQueue& logQueue) :
        messageQueue_(MAX_MESSAGES),
        logQueue_(logQueue) {}

    ~Engine() {

    }

    auto readMessage() {}

private:
    MessageQueue messageQueue_;
    LogQueue logQueue_;
    std::unordered_map<TickerId, Orderbook*> orderBookMap;
};
