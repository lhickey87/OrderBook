#pragma once
#include "LFQueue.h"
#include "../Benchmark/Time.h"
#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <array>
#include <thread>

enum class Side {UNITIALIZED,BUY,SELL};

using Byte = uint8_t;
using ClientId = uint32_t;
using Quantity = std::uint32_t;
using Price = std::int32_t;
using Time = uint64_t;
using OrderId = std::uint64_t;
using TickerId = uint16_t;

constexpr size_t BUFFER_SIZE = 1 << 15;
constexpr size_t MAXLEVELS = 1 << 14;
constexpr size_t MAX_ORDERS = 1 << 18;
constexpr size_t MAX_BUFFERS = 1024;

static constexpr auto HEADER_BYTES = 2;

using RawBuffer = std::array<Byte,BUFFER_SIZE+40>;

struct ReadBuffer {
    RawBuffer* buffer;
    size_t size;
};

using BufferQueue = LFQueue<ReadBuffer>;

inline void logArgs() {std::cerr << std::endl;}

template<typename T, typename... Args>
inline void logArgs(T&& t, Args&&... args){
    std::cerr << t;
    logArgs(std::forward<Args>...);
}

template<typename... Args>
inline auto ASSERT(bool cond, const char* msg, Args&&... args) noexcept {
    if (!cond) [[unlikely]]{
        std::cerr << "ASSERT ERROR:" << msg;
        logArgs(std::forward<Args>(args)...);
        exit(EXIT_FAILURE);
    }
}
