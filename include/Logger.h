#pragma once
#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include <format>
#include "typedefs.h"
#include "LFQueue.h"

using LogQueue = LFQueue<std::string>;

constexpr size_t LOG_QUEUE_SIZE = 8 * 1024 * 1024;

class Logger {
public:
    explicit Logger(const std::string &fileName): fileName_(fileName), queue_(LOG_QUEUE_SIZE)
    {
       //loggerThread = std::thread([this] { flushQueue(); });
    }

    ~Logger() {

    }

private:

    std::atomic<bool> run_;

    LogQueue queue_;
    std::string fileName_;
    std::ofstream file;
    std::atomic<bool> running_{false};
    std::thread loggerThread;

};
