#include "../include/Engine.h"

void Engine::run(){
    while (true){
        while (bufferQueue_->isEmpty()){ continue; }

        const ReadBuffer* bufPtr = bufferQueue_->getReadElement();

        if (bufPtr->buffer == nullptr) {
            std::cout << "called to exit" << std::endl;
            break;
        }

        handleBuffer(bufPtr);
        bufferQueue_->incReadIndex();
    }
    std::cout << "broke from engine run, time to join" << std::endl;
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

void Engine::handleMessage(const Byte* message,MessageType type){
    //need to handle the messageHeader which would be total of 5 bytes
    // 1 byte for type, 2 bytes for locate, 2 bytes for tracking num
    switch (type){
        case (MessageType::ADD_ORDER): {
            // AddOrderMessage::parseMessage(message);
            auto msg = AddOrderMessage::parseMessage(message);
            // msg.print(std::cout);
            logger_->logOrderAdd(msg.orderId_, msg.price_, msg.orderQuantity_, msg.side_);
            orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);
            break;
        }

        case(MessageType::ADD_ORDER_MPID): {
            // IdAddOrderMessage::parseMessage(message);
            auto msg = IdAddOrderMessage::parseMessage(message);
            // msg.print(std::cout);
            logger_->logOrderAdd(msg.orderId_, msg.price_, msg.orderQuantity_,msg.side_);
            orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);
            break;
        }

        case(MessageType::DELETE_ORDER): {
            // DeleteMessage::parseMessage(message);
            auto msg = DeleteMessage::parseMessage(message);
            // msg.print(std::cout);
            logger_->logOrderDelete(msg.cancelOrderId);
            orderBook_->deleteOrder(msg.cancelOrderId);
            break;
        }

        case(MessageType::EXECUTE_ORDER): {
            // ExecMessage::parseMessage(message);
            auto msg = ExecMessage::parseMessage(message);
            // msg.print(std::cout);
            logger_->logOrderExec(msg.orderId_, msg.numShares);
            orderBook_->executeOrder(msg.orderId_,msg.numShares);
            break;
        }

        case(MessageType::EXECUTE_ORDER_WITH_PRICE): {
            // ExecPriceMessage::parseMessage(message);
            auto msg = ExecPriceMessage::parseMessage(message);
            // msg.print(std::cout);
            logger_->logOrderExec(msg.orderId_, msg.numShares);
            orderBook_->executeOrderAtPrice(msg.orderId_,msg.numShares);
            break;
        }

        case(MessageType::REDUCE_ORDER): {
            // ReduceOrderMessage::parseMessage(message);
            auto msg = ReduceOrderMessage::parseMessage(message);
            // msg.print(std::cout);
            logger_->logOrderReduce(msg.orderId_, msg.cancelledShares);
            orderBook_->reduceOrder(msg.orderId_,msg.cancelledShares);
            break;
        }

        case(MessageType::REPLACE_ORDER): {
            // ReplaceMessage::parseMessage(message);
            auto msg = ReplaceMessage::parseMessage(message);
            // msg.print(std::cout);
            logger_->logOrderModify(msg.oldOrderId, msg.newOrderId, msg.numShares, msg.newPrice);
            orderBook_->modifyOrder(msg.oldOrderId, msg.newOrderId,msg.newPrice,msg.numShares);
            break;
        }

        case(MessageType::TRADE): {
            // TradeMessage::parseMessage(message);
            auto msg = TradeMessage::parseMessage(message);
            // msg.print(std::cout);
            logger_->logTrade(msg.sharesMatched, msg.price_);
            break;
        }
    }
}
