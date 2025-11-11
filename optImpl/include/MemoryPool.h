#include <memory>
#include <vector>

template<typename T>
class MemoryPool {

public:
    //default constructs the object type we are storing in each of the (size) entries of our vector
    explicit MemoryPool(size_t size){
        pool_(size, T());
    }

    template<typename... Args>
    T* Allocate(Args... args) noexcept {
        auto memoryBlock = &(pool_[nextAvailableIndex]);

        T* valueAtBlock = &(memoryBlock->data_);
        valueAtBlock = new(valueAtBlock) T(args...);

        updateNextIndex();

        return valueAtBlock;
    }

    void deallocate(const T* element) noexcept {
        const auto nextAvailable = (reinterpret_cast<Block*>(element)-&pool_[0]); //gives us the index of the block to deallocate
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
        bool isFreeBlock;
    };

    auto updateNextIndex() noexcept {
        while (!pool_[nextAvailableIndex].isFreeBlock){
            nextAvailableIndex++;
        }
    }

    std::vector<Block> pool_;
    size_t nextAvailableIndex = 0;
};
