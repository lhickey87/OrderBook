#include <cstdio>
#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include "LFQueue.h"

enum class LogType : int8_t {
    CHAR = 0,
    INTEGER = 1,
    LONG_INTEGER = 2,
    LONG_LONG_INTEGER = 3,
    UNSIGNED_INTEGER = 4,
    UNSIGNED_LONG_INTEGER = 5,
    UNSIGNED_LONG_LONG_INTEGER = 6,
    FLOAT = 7,
    DOUBLE = 8
};

struct LogElement {
    LogType type_ = LogType::CHAR;
    union {
        char c;
        int i;
        long l;
        long long ll;
        unsigned u;
        unsigned long ul;
        unsigned long long ull;
        float f;
        double d;
    } u_;
};

class Logger {

public:
    explicit Logger(const std::string& fileName):
        file_name(fileName),
        logQueue_(1 << 20){
            file_.open(file_name);
        }


private:
    const std::string file_name;
    std::ofstream file_;
    std::atomic<bool> running_;
    LFQueue<LogElement> logQueue_;
    std::thread *loggerThread = nullptr;
};
