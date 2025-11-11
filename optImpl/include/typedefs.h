#include <cstdint>

enum class Side {UNITIALIZED,BUY,SELL};

using Quantity = std::uint32_t;
using Price = std::int64_t;
using OrderId = std::uint64_t;
