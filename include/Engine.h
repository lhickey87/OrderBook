#pragma once
#include "Logger.h"
#include "Orderbook.h"
#include "Message.h"
#include <atomic>
#include <cstdint>
#include <unordered_map>

inline auto getMsgLength(const Byte* data){
    return get16bit(data);
}

struct ReadBuffer;
template<typename T> class LFQueue;

using BufferQueue = LFQueue<ReadBuffer>;

class Engine {

public:
    explicit Engine(MemoryPool<RawBuffer>* bufferPool,Logger* logger, BufferQueue* bufferQueue) :
        logger_(logger),
        orderBook_(new Orderbook()),
        bufferPool_(bufferPool)
        {}

    ~Engine() {}

    void run();
    auto start(int coreId){
        ASSERT(Threads::createThread(coreId, "Engine", [this](){run();}) != nullptr, "Failed starting Engine Thread");
    };

    auto readMessage();
    void handleMessage(const Byte* message, MessageType type);
    void handleBuffer(const ReadBuffer* bufPtr);

private:
    //we will need some sort of queue
    Logger* logger_;
    std::vector<Byte> splicedMessage{};
    Orderbook* orderBook_;
    MemoryPool<RawBuffer>* bufferPool_;
    BufferQueue* bufferQueue_;
};
