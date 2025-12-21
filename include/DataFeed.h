#pragma once
#include <exception>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include <chrono>
#include "MemoryPool.h"
#include "Message.h"
#include "LFQueue.h"
#include "threads.h"


class DataFeed {
public:
    //explicit DataFeed(MemoryPool<RawBuffer>* bufferPool, LFQueue<ReadBuffer>* bufferQueue, const std::string& fileName):
    explicit DataFeed(MemoryPool<RawBuffer>* pool,BufferQueue* buffer, const std::string& fileName):
        leftover_(256),
        bufferPool_(pool),
        bufferQueue_(buffer)
        {
            fd_ = ::open(fileName.c_str(),O_RDONLY);
            if (fd_ != 1){
                std::cout << "file opened \n";
            }
        }

    ~DataFeed(){
        ::close(fd_);
    }

    //this is the function that will be called from main loop via DataFeed->start()
    void start(){
        dataThread = std::thread([this](){run();});
    }

    void join(){
        if (dataThread.joinable()){
            dataThread.join();
        }
    }

    DataFeed() = delete;
    DataFeed& operator=(const DataFeed&) = delete;
    DataFeed& operator=(DataFeed&&) = delete;
    DataFeed(const DataFeed&) = delete;
    DataFeed(DataFeed&&) = delete;
private:
    std::thread dataThread;
    std::vector<Byte> leftover_; //max Size 40
    MemoryPool<RawBuffer>* bufferPool_;
    BufferQueue* bufferQueue_;
    size_t total = 0;
    int fd_;
    size_t leftoverSize = 0;

    void run();
    void flushFinalBuffer(RawBuffer* buffer);

    size_t getBoundary(const Byte* messagebuffer, size_t validBytes) noexcept;

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

        ASSERT(partialSize <= leftover_.capacity(), "End of message too large for our leftover buffer");

        std::memcpy(leftover_.data(), src, partialSize);
        leftoverSize = partialSize;   // store the actual number of bytes
    }
   };
