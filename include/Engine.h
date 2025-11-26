#include "Logger.h"
#include "Orderbook.h"
#include "typedefs.h"
#include "Message.h"
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

    auto readMessage();
    void handleMessage(const char* message);
    void handleBuffer(const ReadBuffer* bufPtr);

private:
    //we will need some sort of queue
    std::vector<char> splicedMessage;
    Orderbook* orderBook_;
    MemoryPool<RawBuffer>* bufferPool_;
    BufferQueue* bufferQueue_;
    LogQueue* logQueue_;
    std::thread* engineThread;
};
