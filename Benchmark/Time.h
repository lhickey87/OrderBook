#pragma once
#include <cstdint>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <mutex>

class Timer {
public:
    Timer() { initializeTimeBase(); }

    static uint64_t GetTimeNanos() {
        initializeTimeBase();
        uint64_t raw = mach_absolute_time();
        return (raw * info.numer) / info.denom;
    }

    void StartTimer() {
        startTime = mach_absolute_time();
    }

    uint64_t getNanoDuration() const {
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

    static mach_timebase_info_data_t info;
    uint64_t startTime = 0;
};
