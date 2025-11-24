#include <fstream>
#include <string>
#include <thread>
#include "typedefs.h"
#include "MemoryPool.h"
#include "Message.h"
#include "LFQueue.h"


class DataFeed {

public:
    explicit DataFeed(MemoryPool<RawBuffer>* bufferPool, LFQueue<ReadBuffer>* bufferQueue, const std::string& fileName):
        bufferPool_(bufferPool),
        bufferQueue_(bufferQueue),
        readFile_(fileName, std::ios::binary)
        {
        }

    //DataFeed should be entirely responsible for reading in buffers, and sending everything to ITCHParser to consume
    auto readBuffer(){

    }

    DataFeed() = delete;
    DataFeed& operator=(const DataFeed&) = delete;
    DataFeed& operator=(DataFeed&&) = delete;
    DataFeed(const DataFeed&) = delete;
    DataFeed(DataFeed&&) = delete;
private:
    MemoryPool<RawBuffer>* bufferPool_;
    LFQueue<ReadBuffer>* bufferQueue_;
    size_t fileLine;
    std::ifstream readFile_;
    std::thread* readThread = nullptr;
};
