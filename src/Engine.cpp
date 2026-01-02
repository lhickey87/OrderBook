#include "../include/Engine.h"

template <LogType Type, typename Func, typename T>
void Engine::dispatch(const T& msg, Func&& func) {
    if constexpr (toBenchmark){
        const auto start = Timer::GetTimeNanos();
        func();
        const auto end = Timer::GetTimeNanos();
        logger_->logLatency<Type>(end-start);
    } else {
        func();
        logger_->log<Type>(msg);
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
    logger_->log<STOP>();
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

void Engine::handleMessage(const Byte* message,MessageType type){
    switch (type){
        case (MessageType::ADD_ORDER): {
            auto msg = AddOrderMessage::parseMessage(message);
            dispatch<ORDER_ADD>(msg, [&](){orderBook_->add(msg.orderId_, msg.side_, msg.price_, msg.orderQuantity_);});
            break;
        }

        case(MessageType::ADD_ORDER_MPID): {
            auto msg = IdAddOrderMessage::parseMessage(message);
            dispatch<ORDER_ADD>(msg,[&](){orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);});
            break;
        }

        case(MessageType::DELETE_ORDER): {
            auto msg = DeleteMessage::parseMessage(message);
            dispatch<ORDER_DELETE>(msg, [&](){orderBook_->deleteOrder(msg.cancelOrderId);});
            break;
        }

        case(MessageType::EXECUTE_ORDER): {
            auto msg = ExecMessage::parseMessage(message);
            dispatch<ORDER_EXEC>(msg,[&](){orderBook_->executeOrder(msg.orderId_,msg.numShares);});
            break;
        }

        case(MessageType::EXECUTE_ORDER_WITH_PRICE): {
            auto msg = ExecPriceMessage::parseMessage(message);
            dispatch<ORDER_EXEC>(msg, [&](){orderBook_->executeOrder(msg.orderId_,msg.numShares);});
            break;
        }

        case(MessageType::REDUCE_ORDER): {
            auto msg = ReduceOrderMessage::parseMessage(message);
            dispatch<ORDER_REDUCE>(msg, [&](){orderBook_->reduceOrder(msg.orderId_,msg.cancelledShares);});
            break;
        }

        case(MessageType::REPLACE_ORDER): {
            auto msg = ReplaceMessage::parseMessage(message);
            dispatch<ORDER_MODIFY>(msg, [&](){
            orderBook_->modifyOrder(msg.oldOrderId, msg.newOrderId,msg.newPrice,msg.numShares);});
            break;
        }

        case(MessageType::TRADE): {
            auto msg = TradeMessage::parseMessage(message);
            logger_->log<TRADE>(msg);
            break;
        }
    }
}
