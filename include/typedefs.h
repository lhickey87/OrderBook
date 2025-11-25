#pragma once
#include <cstdint>
#include <__stddef_size_t.h>
#include <array>

enum class Side {UNITIALIZED,BUY,SELL};
//adding 40 bytes to allow for potential leftover from spliced message at end of buffer
constexpr size_t BUFFER_SIZE = 1024*1024+40;

using Quantity = std::uint32_t;
using Price = std::int64_t;
using Time = uint64_t;
using OrderId = std::uint64_t;
using TickerId = uint16_t;

constexpr size_t MAXLEVELS = 1 << 10;
constexpr size_t MAX_ORDERS = 1 << 20;

constexpr bool WITH_PRICE = true;
constexpr bool WITHOUT_PRICE = false;

using RawBuffer = std::array<char,BUFFER_SIZE>;

struct ReadBuffer {
    RawBuffer* buffer;
    size_t size;
};
