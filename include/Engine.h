#include "Logger.h"
#include "Orderbook.h"
#include "typedefs.h"
#include "Message.h"
#include <cstdint>
#include <unordered_map>


struct ReadBuffer;
template<typename T> class LFQueue;

class Engine {

public:
    explicit Engine(MemoryPool<RawBuffer>* bufferPool,LogQueue* logQueue, LFQueue<ReadBuffer>* bufferQueue) :
        bufferPool_(bufferPool),
        logQueue_(logQueue),
        bufferQueue_(bufferQueue)
        {}

    ~Engine() {}

    auto readMessage();


    auto handleMessage(const char* message);
    auto handleBuffer(const ReadBuffer* bufPtr);

private:
    //we will need some sort of queue
    MemoryPool<RawBuffer>* bufferPool_;
    LFQueue<ReadBuffer>* bufferQueue_;
    LogQueue* logQueue_;
    Orderbook* orderBook;
};
