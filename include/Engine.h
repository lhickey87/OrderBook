#pragma once
#include "Logger.h"
#include "Orderbook.h"
#include "Message.h"
#include <atomic>
#include <cstdint>
#include <unordered_map>


struct ReadBuffer;
template<typename T> class LFQueue;

using BufferQueue = LFQueue<ReadBuffer>;

class Engine {

public:
    explicit Engine(MemoryPool<RawBuffer>* bufferPool,Logger* logger, BufferQueue* bufferQueue) :
        logger_(logger),
        orderBook_(new Orderbook()),
        bufferPool_(bufferPool),
        bufferQueue_(bufferQueue)
        {}

    ~Engine() {}

    void run();
    auto start(){
        ASSERT(Threads::createThread("Engine", [this](){run();}) != nullptr, "Failed starting Engine Thread");
    };

    auto readMessage();
    void handleMessage(const Byte* message, MessageType type);
    void handleBuffer(const ReadBuffer* bufPtr);

    Engine() = delete;
    Engine& operator=(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;
    Engine(Engine&&) = delete;
    Engine(const Engine&) = delete;

private:
    //we will need some sort of queue
    Logger* logger_;
    std::vector<Byte> splicedMessage{};
    Orderbook* orderBook_;
    MemoryPool<RawBuffer>* bufferPool_;
    BufferQueue* bufferQueue_;
};
