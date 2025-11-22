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
    explicit Engine(MemoryPool<RawBuffer>* bufferPool,LogQueue* logQueue, BufferQueue* bufferQueue) :
        bufferPool_(bufferPool),
        logQueue_(logQueue),
        bufferQueue_(bufferQueue)
        {}

    ~Engine() {}

    auto readMessage();
    void handleMessage(const char* message);
    auto handleBuffer(const ReadBuffer* bufPtr);

private:
    //we will need some sort of queue
    char* remainingbuffer;
    MemoryPool<RawBuffer>* bufferPool_;
    BufferQueue* bufferQueue_;
    LogQueue* logQueue_;
    Orderbook* orderBook;
};
