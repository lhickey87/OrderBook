#pragma once
#include <cstdio>
#include <thread>
#include <iostream>
#include "Message.h"
#include "Order.h"
#include "threads.h"
#include "typedefs.h"
#include "LFQueue.h"
#include "../Benchmark/BenchTimer.h"

struct OrderAddLog {
    OrderId orderId;
    Price price;
    Quantity quantity;
    Side side; // 'B' or 'S'
};

struct OrderExecLog {
    OrderId orderId;
    Quantity quantity;
};

struct OrderReduceLog {
    OrderId orderId;
    Quantity quantity;
};

struct OrderDeleteLog {
    OrderId orderId;
};

struct OrderModifyLog {
    OrderId oldOrderId;
    OrderId newOrderId;
    Quantity quantity;
    Price price;
};

struct TradeLog {
    Quantity quantity;
    Price price;
};

struct StopLog {
    char message[64];
};

enum class LogType {
    ORDER_ADD,
    ORDER_DELETE,
    ORDER_FILL,
    ORDER_REDUCE,
    ORDER_MODIFY,
    ORDER_EXEC,
    TRADE,
    STOP
};

using enum LogType;

struct LogElement {
    uint64_t timestamp;
    LogType type;

    union {
        OrderAddLog orderAdd;
        OrderExecLog orderExec;
        OrderReduceLog orderReduce;
        OrderDeleteLog orderDelete;
        OrderModifyLog orderModify;
        TradeLog trade;
        StopLog stop;
    } u;
};

#if BENCHMARK
    constexpr const char* LOG_FILE = "Benchmarks.log";
    constexpr bool toBenchmark = true;
    constexpr size_t bufferSize = 512;
#else
    constexpr const char* LOG_FILE = "Orderbook.log";
    constexpr bool toBenchmark = false;
    constexpr size_t bufferSize = 512*1024;
#endif

using LogQueue = LFQueue<LogElement>;

class Logger {
public:
    Logger(LFQueue<LogElement>* logQueue) : queue_(logQueue) {
        logFile_ = std::fopen(LOG_FILE, "w");
        static char fileBuffer[512 * 1024]; // 512 KB
        setvbuf(logFile_, fileBuffer, _IOFBF, sizeof(fileBuffer));
    }

    void start() noexcept {
        loggerThread_ = std::thread([this](){run();});
    }

    void join(){
        if (loggerThread_.joinable()){
            loggerThread_.join();
        }
    }

    ~Logger() {
        if (logFile_) std::fclose(logFile_);
    }

    template<LogType Type>
    inline void logLatency(uint64_t duration) noexcept {
        typeCounter[static_cast<size_t>(Type)].record(duration);
    }

    void logOrderAdd(const AddOrderMessage& msg) noexcept;

    void logOrderAdd(const IdAddOrderMessage& msg) noexcept;

    void logOrderDelete(const DeleteMessage& msg) noexcept;

    void logOrderReduce(const ReduceOrderMessage& msg) noexcept;

    void logOrderExec(const ExecMessage& msg) noexcept;

    void logOrderExec(const ExecPriceMessage& msg) noexcept;

    void logOrderModify(const ReplaceMessage& msg) noexcept;

    void logTrade(const TradeMessage& msg) noexcept;

    void logStop() noexcept;

    template<LogType type>
    void log(){
        if constexpr (type == STOP){
            logStop();
        }
    }

    template<LogType type, typename T>
    constexpr void log(const T& msg){
        if constexpr(type == ORDER_ADD) logOrderAdd(msg);
        else if constexpr (type == ORDER_DELETE) logOrderDelete(msg);
        else if constexpr (type == ORDER_EXEC) logOrderExec(msg);
        else if constexpr (type == ORDER_FILL) logOrderReduce(msg);
        else if constexpr (type == ORDER_MODIFY) logOrderModify(msg);
        else if constexpr (type == ORDER_REDUCE) logOrderReduce(msg);
        else if constexpr (type == TRADE) logTrade(msg);
    }

private:
    LogQueue* queue_; // Queue of pointers
    std::FILE* logFile_;
    std::thread loggerThread_;
    TypeCounter typeCounter{};

    void flushCounters() noexcept;

    void run() noexcept;

    inline void writeAdd(const OrderAddLog& orderAdd, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "ADD ORDER: orderId: %llu, Quantity: %u, Price: %d \n",
                                           orderAdd.orderId, orderAdd.quantity, orderAdd.price);
        std::fwrite(buffer.data(),sizeof(Byte), requiredBytes, logFile_);
    }

    inline auto writeReduce(const OrderReduceLog& orderReduce, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "REDUCE ORDER: orderId: %llu, Shares Reduced: %u \n",
                                           orderReduce.orderId, orderReduce.quantity);
        std::fwrite(buffer.data(), sizeof(Byte), requiredBytes, logFile_);
    }

    inline void writeFill(const OrderExecLog& fillLog, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "FILL ORDER: orderId: %llu, Shares Reduced: %u \n",
                                           fillLog.orderId, fillLog.quantity);
        std::fwrite(buffer.data(), sizeof(Byte), requiredBytes, logFile_);
    }

    inline void writeModify(const OrderModifyLog& modifyLog, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "MODIFY ORDER: Old OrderId: %llu, New OrderId: %llu, New Price: %d, Quantity: %u \n",
                                           modifyLog.oldOrderId, modifyLog.newOrderId, modifyLog.price, modifyLog.quantity);
        std::fwrite(buffer.data(),sizeof(Byte), requiredBytes, logFile_);
    }

    inline void writeExec(const OrderExecLog& execLog, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "EXECUTE ORDER: OrderId: %llu, Shares Executed: %u \n",
                                           execLog.orderId, execLog.quantity);
        std::fwrite(buffer.data(), sizeof(Byte), requiredBytes, logFile_);
    }

    inline void writeOrderDelete(const OrderDeleteLog& deleteLog,std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "DELETE ORDER: Deleted Order OrderId: %llu \n",
                                           deleteLog.orderId);
        std::fwrite(buffer.data(), sizeof(Byte), requiredBytes, logFile_);
    }

    inline void writeTrade(const TradeLog& tradeLog, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "TRADE: Quantity: %u, Price: %d \n",
                                           tradeLog.quantity, tradeLog.price);
        std::fwrite(buffer.data(), sizeof(Byte), requiredBytes, logFile_);
    }

    inline void writeStop(std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),"Trading Day Finished! \n");
        std::fwrite(buffer.data(),sizeof(Byte), requiredBytes, logFile_);
    }

    void formatWrite(const LogElement* logElement, std::vector<char>& buffer){
        switch (logElement->type){
            case LogType::ORDER_ADD:
            writeAdd(logElement->u.orderAdd, buffer);
            break;

            case LogType::ORDER_REDUCE:
            writeReduce(logElement->u.orderReduce, buffer);
            break;

            case LogType::ORDER_FILL:
            writeFill(logElement->u.orderExec, buffer);
            break;

            case LogType::ORDER_MODIFY:
            writeModify(logElement->u.orderModify, buffer);
            break;

            case LogType::ORDER_EXEC:
            writeExec(logElement->u.orderExec, buffer);
            break;

            case LogType::ORDER_DELETE:
            writeOrderDelete(logElement->u.orderDelete, buffer);
            break;

            case LogType::TRADE:
            writeTrade(logElement->u.trade, buffer);
            break;

            case LogType::STOP:
            writeStop(buffer);
            break;
        }
    }
};
