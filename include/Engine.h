#pragma once
#include "Logger.h"
#include "Orderbook.h"
#include "Message.h"
#include "../Benchmark/Time.h"
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>

struct ReadBuffer;
template<typename T> class LFQueue;

class Engine {

public:
    explicit Engine(MemoryPool<RawBuffer>* bufferPool, Logger* logger, BufferQueue* bufferQueue) :
        logger_(logger),
        orderBook_(std::make_unique<Orderbook>()),
        bufferPool_(bufferPool),
        bufferQueue_(bufferQueue)
        {}

    ~Engine() {}

    void run();
    auto start(){
        engineThread = std::thread([this](){run();});
    };

    template<LogType Type, typename Func, typename T>
    inline void dispatch(const T& msg, Func&& func);

    void join(){
        if (engineThread.joinable()){
            engineThread.join();
        }
    }

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
    std::thread engineThread;
    uint64_t average{};
    uint64_t count{};
    Logger* logger_;
    std::unique_ptr<Orderbook> orderBook_;
    MemoryPool<RawBuffer>* bufferPool_;
    BufferQueue* bufferQueue_;
};
