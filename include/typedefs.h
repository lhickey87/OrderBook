#pragma once
#include <cstdint>
#include <__stddef_size_t.h>

enum class Side {UNITIALIZED,BUY,SELL};

using Quantity = std::uint32_t;
using Price = std::int64_t;
using Time = uint64_t;
using OrderId = std::uint64_t;
using TickerId = uint16_t;

constexpr size_t MAXLEVELS = 1 << 10;
constexpr size_t MAX_ORDERS = 1 << 20;
