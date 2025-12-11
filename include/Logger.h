#pragma once
#include <cstdio>
#include <thread>
#include <iostream>
#include "threads.h"
#include "typedefs.h"
#include "LFQueue.h"
#include "../Benchmark/Time.h"

struct OrderAddLog {
    OrderId orderId;
    Price price;
    Quantity quantity;
    char side; // 'B' or 'S'
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

// 3. The LogElement (The "Union" Wrapper)
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
        char genericMsg[64]; // Fallback fixed buffer
    } u;
};

using LogQueue = LFQueue<LogElement>;

class Logger {
public:
    Logger(LFQueue<LogElement>* logQueue) : queue_(logQueue) {
        logFile_ = std::fopen("orderbook.log", "w");
        static char fileBuffer[512 * 1024]; // 512 KB
        setvbuf(logFile_, fileBuffer, _IOFBF, sizeof(fileBuffer));
    }

    void start(int coreId) noexcept {
        ASSERT(Threads::createThread(coreId, "Logger Thread", [this](){run();}) != nullptr, "Unable to start logger thread");
    }

    ~Logger() {
        if (logFile_) std::fclose(logFile_);
    }

    void logOrderAdd(OrderId oid, Price price, Quantity qty, char side) noexcept {
        auto elem = queue_->getWriteElement();
        elem->timestamp = Timer::GetTimeNanos(); // Use your fast timer
        elem->type = LogType::ORDER_ADD;
        elem->u.orderAdd = {oid, price, qty, side};
        queue_->incWriteIndex();
    }

    void logOrderFill(OrderId orderId, Quantity quantity) noexcept {
        LogElement* logElement = queue_->getWriteElement();
        logElement->timestamp = Timer::GetTimeNanos();
        logElement->type = LogType::ORDER_FILL;
        logElement->u.orderReduce = {orderId, quantity};
        queue_->incWriteIndex();
    }

    void logOrderExec(OrderId orderId, Quantity quantity) noexcept {
        LogElement* logElement = queue_->getWriteElement();
        logElement->timestamp = Timer::GetTimeNanos();
        logElement->type = LogType::ORDER_EXEC;
        logElement->u.orderExec = {orderId, quantity};
        queue_->incWriteIndex();
    }

    void logOrderDelete(OrderId orderId) noexcept {
        LogElement* logElement = queue_->getWriteElement();
        logElement->timestamp = Timer::GetTimeNanos();
        logElement->type = LogType::ORDER_DELETE;
        logElement->u.orderDelete = {orderId};
        queue_->incWriteIndex();
    }

    void logOrderModify(OrderId oldOrderId, OrderId newOrderId, Quantity quantity, Price price) noexcept {
        LogElement* logElement = queue_->getWriteElement();
        logElement->timestamp = Timer::GetTimeNanos();
        logElement->type = LogType::ORDER_MODIFY;
        logElement->u.orderModify = {oldOrderId, newOrderId, quantity, price};
        queue_->incWriteIndex();
    }

    void logTrade(Quantity quantity, Price price) noexcept {
        LogElement* logElement = queue_->getWriteElement();
        logElement->timestamp = Timer::GetTimeNanos();
        logElement->type = LogType::TRADE;
        logElement->u.trade = {quantity,price};
        queue_->incWriteIndex();
    }

private:
    LogQueue* queue_; // Queue of pointers
    std::FILE* logFile_;

    void run() noexcept {
        std::vector<char> buffer_;

        buffer_.reserve(4096);
        while (true){
            if (queue_->isEmpty())[[unlikely]]{ std::this_thread::yield();}

            auto logElement = queue_->getReadElement();

            if (logElement->type == LogType::STOP) { break;}

            formatWrite(logElement, buffer_);
            queue_->incReadIndex();
        }
    }

    void writeAdd(const OrderAddLog& orderAdd, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "ADD ORDER: orderId: %llu, Quantity: %u, Price: %d \n",
                                           orderAdd.orderId, orderAdd.quantity, orderAdd.quantity);

        ASSERT(requiredBytes < buffer.size(), "Buffer is too small");
        std::fwrite(buffer.data(),1, requiredBytes, logFile_);
    }

    auto writeReduce(const OrderReduceLog& orderReduce, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "REDUCE ORDER: orderId: %llu, Shares Reduced: %u \n",
                                           orderReduce.orderId, orderReduce.quantity);

        ASSERT(requiredBytes < buffer.size(), "Buffer is too small");

        std::fwrite(buffer.data(), 1, requiredBytes, logFile_);
    }

    void writeFill(const OrderExecLog& fillLog, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "FILL ORDER: orderId: %llu, Shares Reduced: %u \n",
                                           fillLog.orderId, fillLog.quantity);

        ASSERT(requiredBytes < buffer.size(), "Buffer is too small");

        std::fwrite(buffer.data(), 1, requiredBytes, logFile_);
    }

    void writeModify(const OrderModifyLog& modifyLog, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "MODIFY ORDER: Old OrderId: %llu, New OrderId: %llu, New Price: %d, Quantity: %u \n",
                                           modifyLog.oldOrderId, modifyLog.newOrderId, modifyLog.price, modifyLog.quantity);

        ASSERT(requiredBytes < buffer.size(), "Buffer is too small, cannot stream formatted string into buffer");

        std::fwrite(buffer.data(), 1, requiredBytes, logFile_);
    }

    void writeExec(const OrderExecLog& execLog, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "EXECUTE ORDER: OrderId: %llu, Shares Executed: %u \n",
                                           execLog.orderId, execLog.quantity);

        ASSERT(requiredBytes < buffer.size(), "Buffer is too small");
        std::fwrite(buffer.data(), 1, requiredBytes, logFile_);
    }

    void writeOrderDelete(const OrderDeleteLog& deleteLog,std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "DELETE ORDER: Deleted Order OrderId: %llu \n",
                                           deleteLog.orderId);

        ASSERT(requiredBytes < buffer.size(), "Buffer is too small");

        std::fwrite(buffer.data(), 1, requiredBytes, logFile_);
    }

    void writeTrade(const TradeLog& tradeLog, std::vector<char>& buffer) noexcept {
        auto requiredBytes = std::snprintf(buffer.data(), buffer.size(),
                                           "TRADE: Quantity: %u, Price: %d \n",
                                           tradeLog.quantity, tradeLog.price);

        ASSERT(requiredBytes < buffer.size(), "Buffer is too small");

        std::fwrite(buffer.data(), 1, requiredBytes, logFile_);
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
            break;
        }
    }
};
