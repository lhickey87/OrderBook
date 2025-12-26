#pragma once
#include <vector>
#include <atomic>

template<typename T>
class LFQueue {
public:

    explicit LFQueue(size_t capacity) :
        capacity_(capacity), buffer_(capacity, T()) {}

    auto getWriteElement() noexcept {
        return &buffer_[writeIndex_];
    }

    auto incWriteIndex() noexcept {
        writeIndex_ = (writeIndex_+1) & (buffer_.size()-1);
    }

    auto getReadElement() const {
        return (Size() ? &buffer_[readIndex_] : nullptr);
    }

    auto incReadIndex() noexcept {
        if (Size() != 0) [[likely]] {
            readIndex_ = (readIndex_+1) & (buffer_.size()-1);
        }
    }

    size_t Size() const {
        const auto write = writeIndex_.load(std::memory_order_relaxed);
        const auto read = readIndex_.load(std::memory_order_relaxed);
        if (write > read){
            return write-read;
        } else {
            return capacity_ - (read-write);
        }
    }

    bool isEmpty() noexcept {
        return (writeIndex_.load() == readIndex_.load());
    }

    bool isFull() const noexcept {
        return Size() == capacity_ - 1;
    }

    LFQueue() = delete;
    LFQueue(const LFQueue&) = delete;
    LFQueue(LFQueue&&) = delete;
    LFQueue& operator=(const LFQueue&) = delete;
    LFQueue& operator=(LFQueue&&) = delete;

private:
    size_t capacity_;
    std::vector<T> buffer_;
    alignas(64) std::atomic<size_t> writeIndex_{0};
    alignas(64) std::atomic<size_t> readIndex_{0};
};
