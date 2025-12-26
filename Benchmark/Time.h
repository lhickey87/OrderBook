#pragma once
#include <cstdint>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mutex>
#include "../include/Logger.h"

class Timer {
public:
    Timer() { initializeTimeBase(); }

    static inline uint64_t GetTimeNanos() {
        initializeTimeBase();
        uint64_t raw = mach_absolute_time();
        return (raw * info.numer) / info.denom;
    }

    inline void StartTimer() {
        startTime = mach_absolute_time();
    }

    inline uint64_t getNanoDuration() const {
        uint64_t duration = mach_absolute_time() - startTime;
        return (duration * info.numer) / info.denom;
    }

private:
    static void initializeTimeBase() {
        static std::once_flag flag;
        std::call_once(flag, [] {
            mach_timebase_info(&info);
        });
    }

    inline static mach_timebase_info_data_t info = {0,0};
    uint64_t startTime = 0;
};



//type does not need to be stored by benchTimer, instead we will use the index as an entry into BenchTimer Metrics
