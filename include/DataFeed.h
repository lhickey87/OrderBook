#include <fstream>
#include <string>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include "typedefs.h"
#include "MemoryPool.h"
#include "Message.h"
#include "LFQueue.h"


class DataFeed {

public:
    explicit DataFeed(MemoryPool<RawBuffer>* bufferPool, LFQueue<ReadBuffer>* bufferQueue, const std::string& fileName):
        leftover_(40),
        bufferPool_(bufferPool),
        bufferQueue_(bufferQueue)
        {
            fd_ = ::open(fileName.c_str(),O_RDONLY);
            if (fd_ < 0){
                //deal with error
            }
        }
    ~DataFeed(){
        ::close(fd_);
    }

    //DataFeed should be entirely responsible for reading in buffers, and sending everything to ITCHParser to consume
    auto readBuffer();
    size_t getBoundary(char* messagebuffer, size_t validBytes);
    void getLastCompleteMessage();

    DataFeed() = delete;
    DataFeed& operator=(const DataFeed&) = delete;
    DataFeed& operator=(DataFeed&&) = delete;
    DataFeed(const DataFeed&) = delete;
    DataFeed(DataFeed&&) = delete;
private:
    std::vector<char> leftover_; //max Size 40
    MemoryPool<RawBuffer>* bufferPool_;
    LFQueue<ReadBuffer>* bufferQueue_;
    int fd_;
    size_t leftoverSize;
    std::ifstream readFile_;
    std::thread* readThread = nullptr;
};
