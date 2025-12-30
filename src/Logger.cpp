#include "../include/Logger.h"

void Logger::logOrderAdd(const AddOrderMessage& msg) noexcept {
    auto logElement = queue_->getWriteElement();
    logElement->type = LogType::ORDER_ADD;
    logElement->u.orderAdd = {msg.orderId_,msg.price_,msg.orderQuantity_,msg.side_};
    queue_->incWriteIndex();
}

void Logger::logOrderAdd(const IdAddOrderMessage& msg) noexcept {
    auto logElement = queue_->getWriteElement();
    logElement->type = LogType::ORDER_ADD;
    logElement->u.orderAdd = {msg.orderId_,msg.price_,msg.orderQuantity_,msg.side_};
    queue_->incWriteIndex();
}

void Logger::logOrderReduce(const ReduceOrderMessage& msg) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    logElement->type = LogType::ORDER_REDUCE;
    logElement->u.orderReduce = {msg.orderId_, msg.cancelledShares};
    queue_->incWriteIndex();
}

void Logger::logOrderExec(const ExecMessage& msg) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    logElement->type = LogType::ORDER_EXEC;
    logElement->u.orderExec = {msg.orderId_, msg.numShares};
    queue_->incWriteIndex();
}

void Logger::logOrderExec(const ExecPriceMessage& msg) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    logElement->type = LogType::ORDER_EXEC;
    logElement->u.orderExec = {msg.orderId_, msg.numShares};
    queue_->incWriteIndex();
}

void Logger::logOrderDelete(const DeleteMessage& msg) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    logElement->type = LogType::ORDER_DELETE;
    logElement->u.orderDelete = {msg.cancelOrderId};
    queue_->incWriteIndex();
}

void Logger::logOrderModify(const ReplaceMessage& msg) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    logElement->type = LogType::ORDER_MODIFY;
    logElement->u.orderModify = {msg.oldOrderId, msg.newOrderId, msg.numShares, msg.newPrice};
    queue_->incWriteIndex();
}

void Logger::logTrade(const TradeMessage& msg) noexcept {
    LogElement* logElement = queue_->getWriteElement();
    logElement->type = LogType::TRADE;
    logElement->u.trade = {msg.sharesMatched,msg.price_};
    queue_->incWriteIndex();
}

void Logger::logStop() noexcept {
    LogElement* logElement = queue_->getWriteElement();
    logElement->type = LogType::STOP;
    logElement->u.stop = {"Finished"};
    queue_->incWriteIndex();
}

void Logger::flushCounters() noexcept{
    char buffer[1024];
    for (int i =0; i < typeCounter.size(); ++i){
        const auto& timer = typeCounter[i];
        const char* type = BENCH::type[i];

        if (timer.totalOps == 0){ continue;}

        const double average = static_cast<double>(timer.totalNanos) / timer.totalOps;

        const int len = std::snprintf(buffer, sizeof(buffer),"Type: %s, Total Operations: %llu Average Latency: %f \n",type, timer.totalOps, average);

        std::fwrite(buffer, sizeof(Byte), len, logFile_);
    }
    std::fflush(logFile_);
}

void Logger::run() noexcept {
    std::vector<char> buffer_;

    buffer_.resize(1<<15);
    while (true){
        while (queue_->isEmpty()){ std::this_thread::yield();}

        const auto logElement = queue_->getReadElement();

        if (logElement->type == LogType::STOP)[[unlikely]] {
            #if BENCHMARK
               flushCounters();
            #else
                formatWrite(logElement,buffer_);
            #endif
            break;
        }

        #if !BENCHMARK
        formatWrite(logElement, buffer_);
        #endif

        queue_->incReadIndex();
    }
}
