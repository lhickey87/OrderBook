#pragma once
#include <thread>
#include <iostream>
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
    OrderId orderId;
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
    INFO_MSG // Fallback for generic text
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

class Logger {
public:
    Logger(LFQueue<LogElement>* logQueue) : running_(true), queue_(logQueue) {
        logFile_ = std::fopen("orderbook.log", "w");
    }

    ~Logger() {
        running_ = false;
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

    void logTrade(OrderId orderId, Quantity quantity, Price price) noexcept {
        LogElement* logElement = queue_->getWriteElement();
        logElement->timestamp = Timer::GetTimeNanos();
        logElement->type = LogType::TRADE;
        logElement->u.trade = {orderId,quantity,price};
    }

private:

    void consumeLogs() noexcept {
        while (running_ && !queue_->isEmpty()){
            if (queue_->isEmpty())[[unlikely]]{
                std::this_thread::yield();
            }

            auto logElement = queue_->getReadElement();
            formatWrite(logElement);
        }
    }

    void writeAdd(const LogElement* logElement) noexcept {

    }

    auto writeReduce(const LogElement* logElement) noexcept {

    }

    void writeFill(const LogElement* logElement) noexcept {

    }

    void writeModify(const LogElement* logElement) noexcept {

    }

    void writeExec(const LogElement* logElement) noexcept {

    }

    void writeOrderDelete(const LogElement* logElement) noexcept {}

    void writeTrade(const LogElement* logElement) noexcept {

    }


    void formatWrite(const LogElement* logElement){
        switch (logElement->type){
            case LogType::ORDER_ADD:
            writeAdd(logElement);
            break;

            case LogType::ORDER_REDUCE:
            writeReduce(logElement);
            break;

            case LogType::ORDER_FILL:
            writeFill(logElement);
            break;

            case LogType::ORDER_MODIFY:
            writeModify(logElement);
            break;

            case LogType::ORDER_EXEC:
            writeExec(logElement);
            break;

            case LogType::ORDER_DELETE:
            writeOrderDelete(logElement);
            break;

            case LogType::TRADE:
            writeTrade(logElement);
            break;

            case LogType::INFO_MSG:
            break;
        }
    }
    std::atomic<bool> running_;
    LFQueue<LogElement>* queue_; // Queue of pointers
    std::thread* loggerThread_;
    std::FILE* logFile_;
};
