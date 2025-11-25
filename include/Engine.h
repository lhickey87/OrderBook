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
        bufferQueue_(bufferQueue)
        {}

    ~Engine() {}

    auto readMessage();
    void handleMessage(const char* message);
    void handleBuffer(char* remainingBuf,const ReadBuffer* bufPtr);

private:
    //we will need some sort of queue
    MemoryPool<RawBuffer>* bufferPool_;
    BufferQueue* bufferQueue_;
    LogQueue* logQueue_;
    std::thread* engineThread;
    Orderbook* orderBook;
};
