#include <string>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <atomic>
#include "typedefs.h"
#include "MemoryPool.h"
#include "Message.h"
#include "LFQueue.h"
#include "threads.h"

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

    void flushFinalBuffer(RawBuffer* buffer);
    //DataFeed should be entirely responsible for reading in buffers, and sending everything to ITCHParser to consume
    size_t getBoundary(Byte* messagebuffer, size_t validBytes);

    void run();
    //this is the function that will be called from main loop via DataFeed->start()
    auto start(int coreId)
    {
        readThread = createThread(coreId,"DataFeed",[this](){run();});
    }

    DataFeed() = delete;
    DataFeed& operator=(const DataFeed&) = delete;
    DataFeed& operator=(DataFeed&&) = delete;
    DataFeed(const DataFeed&) = delete;
    DataFeed(DataFeed&&) = delete;
private:

    inline void enqueueBuffer(RawBuffer* buffer, size_t size)
    {
        ReadBuffer* slot = bufferQueue_->getWriteElement();
        slot->buffer = buffer;
        slot->size   = size;
        bufferQueue_->incWriteIndex();
    }

    inline void prependLeftover(Byte* dst) noexcept
    {
        if (leftoverSize == 0) return;
        std::memcpy(dst, leftover_.data(), leftoverSize);
    }

    inline void storeLeftover(const Byte* src, size_t partialSize) noexcept
    {
        if (partialSize == 0) {
            leftoverSize = 0;
            return;
        }

        leftoverSize = partialSize;
        std::memcpy(leftover_.data(), src, partialSize);
    }

    std::vector<Byte> leftover_; //max Size 40
    MemoryPool<RawBuffer>* bufferPool_;
    LFQueue<ReadBuffer>* bufferQueue_;
    int fd_;
    size_t leftoverSize;
    std::thread* readThread = nullptr;
};
