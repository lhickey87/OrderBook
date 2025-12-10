#pragma once
#include "Logger.h"
#include "Orderbook.h"
#include "typedefs.h"
#include "Message.h"
#include "threads.h"
#include <atomic>
#include <cstdint>
#include <unordered_map>


struct ReadBuffer;
template<typename T> class LFQueue;

using BufferQueue = LFQueue<ReadBuffer>;

class Engine {

public:
    explicit Engine(MemoryPool<RawBuffer>* bufferPool,LogQueue* logQueue, BufferQueue* bufferQueue) :
        orderBook_(new Orderbook()),
        bufferPool_(bufferPool),
        logQueue_(logQueue),
        bufferQueue_(bufferQueue),
        splicedMessage(50)
        {}

    ~Engine() {}

    void run();
    auto start(int coreId){
        createThread(coreId, "Engine", [this](){run();});
    };


    auto readMessage();
    void handleMessage(const Byte* message, MessageType type);
    void handleBuffer(const ReadBuffer* bufPtr);

private:
    //we will need some sort of queue
    std::vector<Byte> splicedMessage;
    Orderbook* orderBook_;
    MemoryPool<RawBuffer>* bufferPool_;
    BufferQueue* bufferQueue_;
    LFQueue<std::string>* logQueue_;
};
