#include <vector>
#include <atomic>

template<typename T>
class LFQueue {
public:

    explicit LFQueue(size_t size) :
        buffer_(size, T()) {}

    auto getWriteElement() noexcept {
        return &buffer_[writeIndex_];
    }

    auto incWriteIndex() noexcept {
        writeIndex_ = (writeIndex_+1) & (buffer_.size()-1);
        numElements_++;
    }

    auto getReadElement() const {
        return (Size() ? &buffer_[readIndex_] : nullptr);
    }

    auto incReadIndex() noexcept {
        if (Size() != 0) [[likely]] {
            readIndex_ = (readIndex_+1) & (buffer_.size()-1);
            numElements_--;
        }
    }

    size_t Size() const {
        return numElements_.load();
    }

    bool isEmpty() noexcept {
        return (writeIndex_.load() == readIndex_.load());
    }

    bool isFull() noexcept {
        return Size() - 1== buffer_.size();
    }

    LFQueue() = delete;
    LFQueue(const LFQueue&) = delete;
    LFQueue(LFQueue&&) = delete;
    LFQueue& operator=(const LFQueue&) = delete;
    LFQueue& operator=(LFQueue&&) = delete;

private:
    std::vector<T> buffer_;
    std::atomic<size_t> writeIndex_{0};
    std::atomic<size_t> readIndex_{0};
    std::atomic<size_t> numElements_{0};
};
