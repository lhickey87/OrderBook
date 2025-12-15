#pragma once
#include <thread>
#include <iostream>
#include <string_view>
#if defined(__APPLE__)
    //none for now
#elif defined(__linux__)
    #include <pthread.h>
    #include <sched.h>

#elif defined(_WIN32)
    #include <windows.h>
#endif

namespace Threads {
    // inline bool setThreadCore() {
    // #if defined(__APPLE__)
    //     // macOS does not support true core pinning.
    //     // if we measure cache line ping pong then we might consider setting tags for certain threads (hinting)
    //     return true;

    // #elif defined(_WIN32)
    //     DWORD_PTR mask = (1ULL << coreId);
    //     return SetThreadAffinityMask(GetCurrentThread(), mask) != 0;

    // #elif defined(__linux__)
    //     cpu_set_t cpuset;
    //     CPU_ZERO(&cpuset);
    //     CPU_SET(coreId, &cpuset);
    //     return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t),&cpuset) == 0;

    // #else
    //     return false; // unknown OS
    // #endif
    // }

    template <typename Func>
    auto createThread(std::string_view threadName, Func&& func){
        auto thread = new std::thread([&](){std::forward<Func>(func)();});
        std::cerr << "Starting thread for: " << threadName << "\n";
        //may have to sleep main thread
        return thread;
    }

}
