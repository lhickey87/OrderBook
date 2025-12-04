#pragma once
#include <chrono>
#include <format>
#include <iostream>
#include <array>

enum class Side {UNITIALIZED,BUY,SELL};

constexpr size_t BUFFER_SIZE = 1024*1024+40;

//arbitrary for now, our current data file is 8Gb so this will hold enough

using ClientId = uint32_t;
using Quantity = std::uint32_t;
using Price = std::int32_t;
using Time = uint64_t;
using OrderId = std::uint64_t;
using TickerId = uint16_t;

constexpr size_t MAXLEVELS = 1 << 10;
constexpr size_t MAX_ORDERS = 1 << 20;

using RawBuffer = std::array<char,BUFFER_SIZE>;

struct ReadBuffer {
    RawBuffer* buffer;
    size_t size;
};

inline auto ASSERT(bool cond, const std::string& msg) noexcept {
    if (!cond) [[unlikely]]{
        std::cerr << "ASSERT ERROR:" << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}
