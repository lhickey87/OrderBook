#include "../include/Engine.h"

template <LogType Type, typename Func, typename... Args>
inline void Engine::dispatch(Func&& func, Args&&... args) {
    if constexpr (toBenchmark){
        const auto start = Timer::GetTimeNanos();
        func(std::forward<Args>(args)...);
        // std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        const auto end = Timer::GetTimeNanos();
        // logger_->logLatency(Type, end-start);
    } else {
        logger_->log<Type>(std::forward<Args>(args)...);
        // std::invoke(std::forward<Func>(func),std::forward<Args>(args)...);
        func(std::forward<Args>(args)...);
    }
}

void Engine::run(){
    while (true){
        while (bufferQueue_->isEmpty()){ continue; }

        const ReadBuffer* bufPtr = bufferQueue_->getReadElement();

        if (bufPtr->buffer == nullptr)[[unlikely]] { break;}

        handleBuffer(bufPtr);
        bufferQueue_->incReadIndex();
    }
    logger_->logStop();
}

void Engine::handleBuffer(const ReadBuffer* bufPtr) {
    size_t remainingBytes = bufPtr->size;
    const Byte* buf = bufPtr->buffer->data();

    uint16_t msgLen;
    size_t totalLength; //this length includes LENGTH header
    MessageType type;

    while (remainingBytes > 0) {
        msgLen = get16bit(buf);   // reads bytes [0..1]
        totalLength = msgLen + HEADER_BYTES;  // HEADER_BYTES = 3

        buf += HEADER_BYTES;
        type = static_cast<MessageType>(*buf);
        handleMessage(buf, type);
        buf += msgLen;
        remainingBytes -= totalLength;
    }
}

//if we wanted to turn this into compile time?
void Engine::handleMessage(const Byte* message,MessageType type){
    // 1 byte for type, 2 bytes for locate, 2 bytes for tracking num
    switch (type){
        case (MessageType::ADD_ORDER): {
            auto msg = AddOrderMessage::parseMessage(message);
            logger_->logOrderAdd(msg.orderId_, msg.price_, msg.orderQuantity_, msg.side_);
            const auto start = Timer::GetTimeNanos();
            orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);
            const auto duration = Timer::GetTimeNanos();
            break;
        }

        case(MessageType::ADD_ORDER_MPID): {
            auto msg = IdAddOrderMessage::parseMessage(message);
            logger_->logOrderAdd(msg.orderId_, msg.price_, msg.orderQuantity_,msg.side_);
            orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);
            break;
        }

        case(MessageType::DELETE_ORDER): {
            auto msg = DeleteMessage::parseMessage(message);
            logger_->logOrderDelete(msg.cancelOrderId);
            orderBook_->deleteOrder(msg.cancelOrderId);
            break;
        }

        case(MessageType::EXECUTE_ORDER): {
            auto msg = ExecMessage::parseMessage(message);
            logger_->logOrderExec(msg.orderId_, msg.numShares);
            orderBook_->executeOrder(msg.orderId_,msg.numShares);
            break;
        }

        case(MessageType::EXECUTE_ORDER_WITH_PRICE): {
            auto msg = ExecPriceMessage::parseMessage(message);
            logger_->logOrderExec(msg.orderId_, msg.numShares);
            orderBook_->executeOrderAtPrice(msg.orderId_,msg.numShares);
            break;
        }

        case(MessageType::REDUCE_ORDER): {
            auto msg = ReduceOrderMessage::parseMessage(message);
            logger_->logOrderReduce(msg.orderId_, msg.cancelledShares);
            orderBook_->reduceOrder(msg.orderId_,msg.cancelledShares);
            break;
        }

        case(MessageType::REPLACE_ORDER): {
            auto msg = ReplaceMessage::parseMessage(message);
            logger_->logOrderModify(msg.oldOrderId, msg.newOrderId, msg.numShares, msg.newPrice);
            orderBook_->modifyOrder(msg.oldOrderId, msg.newOrderId,msg.newPrice,msg.numShares);
            break;
        }

        case(MessageType::TRADE): {
            auto msg = TradeMessage::parseMessage(message);
            logger_->logTrade(msg.sharesMatched, msg.price_);
            break;
        }
    }
}
