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
using LogQueue = LFQueue<LogElement>;

class Engine {

public:
    explicit Engine(Orderbook* orderbook, MemoryPool<RawBuffer>* bufferPool,LogQueue* logQueue, BufferQueue* bufferQueue) :
        orderBook_(orderbook),
        bufferPool_(bufferPool),
        logQueue_(logQueue),
        bufferQueue_(bufferQueue),
        splicedMessage(40)
        {}

    ~Engine() {}

    void run();
    auto start(int coreId){
        run_.store(true, std::memory_order_release);
        createThread(coreId, "Engine", [this](){run();});
    };

    auto stop(){
        run_.store(false, std::memory_order_release);
        //this will only get called on an interrupt or when we have run through all binary data
    }

    auto readMessage();
    void handleMessage(const char* message, MessageType type);
    void handleBuffer(const ReadBuffer* bufPtr);

private:
    //we will need some sort of queue
    std::atomic<bool> run_; //will there be namespace issues?
    std::vector<char> splicedMessage;
    Orderbook* orderBook_;
    MemoryPool<RawBuffer>* bufferPool_;
    BufferQueue* bufferQueue_;
    LFQueue<std::string>* logQueue_;
    std::thread* engineThread;
};
