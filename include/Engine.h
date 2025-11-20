#include "Logger.h"
#include "Message.h"
#include "Orderbook.h"
#include <cstdint>
#include <unordered_map>


class Engine {

public:
    explicit Engine(LogQueue* logQueue) :
        messageQueue_(MAX_MESSAGES),
        logQueue_(logQueue) {}


    ~Engine() {

    }

    auto readMessage() {}

private:
    LFQueue<Message*> messageQueue_;
    LogQueue* logQueue_;
    std::unordered_map<TickerId, Orderbook*> orderBookMap;
};
