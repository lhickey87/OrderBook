#pragma once
#include <memory>
#include <vector>

template<typename T>
class MemoryPool {

public:
    //default constructs the object type we are storing in each of the (size) entries of our vector
    explicit MemoryPool(std::size_t size) : pool_(size) {
    }

    template<typename... Args>
    T* Allocate(Args... args) noexcept {
        auto memoryBlock = &(pool_[nextAvailableIndex]);

        T* valueAtBlock = &(memoryBlock->data_);
        valueAtBlock = new(valueAtBlock) T(args...);
        memoryBlock->isFreeBlock = false;

        updateNextIndex();

        return valueAtBlock;
    }

    void deallocate(const T* element) noexcept {
        const auto blockPtr = reinterpret_cast<const Block*>(element);

        const auto nextAvailable = blockPtr - &pool_[0];

        pool_[nextAvailable].isFreeBlock = true;
    }

    MemoryPool() = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) = delete;
private:
    struct Block {
        T data_;
        bool isFreeBlock = true;
    };

    auto updateNextIndex() noexcept {
        while (!pool_[nextAvailableIndex].isFreeBlock){
            nextAvailableIndex++;
        }
    }

    std::vector<Block> pool_;
    size_t nextAvailableIndex = 0;
};
