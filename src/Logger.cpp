#include "../include/Logger.h"

void Logger::logOrderAdd(OrderId oid, Price price, Quantity quantity, Side side) noexcept {
        auto logElement = queue_->getWriteElement();
        // logElement->timestamp = Timer::GetTimeNanos(); // Use your fast timer
        logElement->type = LogType::ORDER_ADD;
        logElement->u.orderAdd = {oid,price,quantity,side};
        queue_->incWriteIndex();
    }


void Logger::logOrderReduce(OrderId orderId, Quantity quantity) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    // logElement->timestamp = Timer::GetTimeNanos();
    logElement->type = LogType::ORDER_REDUCE;
    logElement->u.orderReduce = {orderId, quantity};
    queue_->incWriteIndex();
}


void Logger::logOrderExec(OrderId orderId, Quantity quantity) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    // logElement->timestamp = Timer::GetTimeNanos();
    logElement->type = LogType::ORDER_EXEC;
    logElement->u.orderExec = {orderId, quantity};
    queue_->incWriteIndex();
}

void Logger::logOrderDelete(OrderId orderId) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    // logElement->timestamp = Timer::GetTimeNanos();
    logElement->type = LogType::ORDER_DELETE;
    logElement->u.orderDelete = {orderId};
    queue_->incWriteIndex();
}

void Logger::logOrderModify(OrderId oldOrderId, OrderId newOrderId, Quantity quantity, Price price) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    // logElement->timestamp = Timer::GetTimeNanos();
    logElement->type = LogType::ORDER_MODIFY;
    logElement->u.orderModify = {oldOrderId, newOrderId, quantity, price};
    queue_->incWriteIndex();
}

void Logger::logTrade(Quantity quantity, Price price) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    // logElement->timestamp = Timer::GetTimeNanos();
    logElement->type = LogType::TRADE;
    logElement->u.trade = {quantity,price};
    queue_->incWriteIndex();
}

void Logger::logStop() noexcept {
    LogElement* logElement = queue_->getWriteElement();
    // logElement->timestamp = Timer::GetTimeNanos();
    logElement->type = LogType::STOP;
    logElement->u.stop = {"Finished"};
    queue_->incWriteIndex();
}


void Logger::run() noexcept {
    std::vector<char> buffer_;

    buffer_.resize(1<<15);
    while (true){
        while (queue_->isEmpty()){ std::this_thread::yield();}

        const auto logElement = queue_->getReadElement();

        if (logElement->type == LogType::STOP)[[unlikely]] {
            LOG(logElement,buffer_);
            queue_->incReadIndex();
            break;
        }

        LOG(logElement, buffer_);
        queue_->incReadIndex();
    }
}
