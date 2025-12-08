#pragma once
#include <cstdint>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <sys/time.h>

class Timer {
    Timer() {
        if (info.denom == 0){
            mach_timebase_info(&info);
        }
    }

    inline void StartTimer(){
        startTime = mach_absolute_time();
    }

    inline uint64_t getNanoDuration(){
        endTime = mach_absolute_time();

        const auto duration = endTime - startTime;

        return (duration*info.numer)/info.denom;
    }

private:
    uint64_t startTime;
    uint64_t endTime;
    mach_timebase_info_data_t info;
};
