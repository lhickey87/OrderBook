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

inline bool setThreadCore(int coreId) {
#if defined(__APPLE__)
    // macOS does not support true core pinning.
    // if we measure cache line ping pong then we might consider setting tags for certain threads (hinting)
    return true;

#elif defined(_WIN32)
    DWORD_PTR mask = (1ULL << coreId);
    return SetThreadAffinityMask(GetCurrentThread(), mask) != 0;

#elif defined(__linux__)
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(coreId, &cpuset);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t),&cpuset) == 0;

#else
    return false; // unknown OS
#endif
}

template <typename Func>
auto createThread(int coreId, std::string_view threadName, Func&& func){
    auto thread = new std::thread([&](){
        if (!setThreadCore(coreId)){
            std::cerr << "could not pin thread to core: " << coreId << "\n";
            exit(EXIT_FAILURE);
        }
        std::forward<Func>(func)();
    });

    //may have to sleep main thread
    return thread;
}
