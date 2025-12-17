#pragma once
#include "LFQueue.h"
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
constexpr size_t MAXLEVELS = 1 << 10;
constexpr size_t MAX_ORDERS = 1 << 20;
constexpr size_t MAX_BUFFERS = 64;

static constexpr auto HEADER_BYTES = 2;

using RawBuffer = std::array<Byte,BUFFER_SIZE+40>;

struct ReadBuffer {
    RawBuffer* buffer;
    size_t size;
};

using BufferQueue = LFQueue<ReadBuffer>;

inline auto ASSERT(bool cond, const std::string& msg) noexcept {
    if (!cond) [[unlikely]]{
        std::cerr << "ASSERT ERROR:" << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}
