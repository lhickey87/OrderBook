#pragma once
#include "../include/Logger.h"

namespace BENCH {
    constexpr std::array<const char*, 7> type = {
        "Add Order", "Order Delete", "Order Fill", "Order Reduce", "Order Modify", "Execute Order", "Trade"
    };
}

struct BenchTimer {
    uint64_t totalOps = 0;
    uint64_t totalNanos = 0;
    auto record(uint64_t duration){
        totalNanos += duration;
        totalOps++;
    }
};

using TypeCounter = std::array<BenchTimer, 8>;
