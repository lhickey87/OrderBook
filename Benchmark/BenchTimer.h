#include "../include/Logger.h"

struct BenchTimer {
    uint64_t totalOps{};
    uint64_t totalNanos{};

    auto record(uint64_t duration){
        totalNanos += duration;
        totalOps++;
    }
};
