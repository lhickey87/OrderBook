#include "../include/Engine.h"
#include <cstring>

void Engine::run(){
    while (run_.load(std::memory_order_acquire)){

        while (bufferQueue_->isEmpty())[[unlikely]]{}
        auto bufPtr = bufferQueue_->getReadElement();

        handleBuffer(bufPtr);
    }
}

void Engine::handleBuffer(const ReadBuffer* bufPtr) {
    size_t remainingBytes = bufPtr->size;
    const char* buffer = bufPtr->buffer->data(); //this is the actual std::array of chars
    uint8_t length;
    while (remainingBytes > 0){
        // first byte here will be the type of the message, can use this to get it's length
        length = MsgLengthMap[*buffer];
        if (length > remainingBytes) [[unlikely]]{
            std::memcpy(&splicedMessage, buffer, remainingBytes);
            break;
        } //otherwise we can handle the Message

        handleMessage(buffer, MessageType(*buffer));
        remainingBytes -= length;
    }
}

//could assume that we call handleMessage after getting the length of the message
void Engine::handleMessage(const char* message,MessageType type){
    //need to handle the messageHeader which would be total of 5 bytes
    // 1 byte for type, 2 bytes for locate, 2 bytes for tracking num
    switch (type){
        case (MessageType::ADD_ORDER): {
            auto msg = AddOrderMessage::parseMessage(message);
            orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);
            message += AddOrderMessage::LENGTH;
            break;
        }

        case(MessageType::ADD_ORDER_MPID): {
            auto msg = IdAddOrderMessage::parseMessage(message);
            orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_, msg.clientId_);
            message += AddOrderMessage::LENGTH;
            break;
        }

        case(MessageType::DELETE_ORDER): {
            auto msg = DeleteMessage::parseMessage(message);
            orderBook_->deleteOrder(msg.cancelOrderId);
            message += DeleteMessage::LENGTH;
            break;
        }

        case(MessageType::EXECUTE_ORDER): {
            auto msg = ExecMessage::parseMessage(message);
            orderBook_->executeOrder(msg.orderId_,msg.numShares);
            message += ExecMessage::LENGTH;
            break;
        }

        case(MessageType::EXECUTE_ORDER_WITH_PRICE): {
            auto msg = ExecPriceMessage::parseMessage(message);
            orderBook_->executeOrderAtPrice(msg.orderId_,msg.numShares);
            message += ExecPriceMessage::LENGTH;
            break;
        }

        case(MessageType::REDUCE_ORDER): {
            auto msg = ReduceOrderMessage::parseMessage(message);
            orderBook_->reduceOrder(msg.orderId_,msg.cancelledShares);
            message += ReduceOrderMessage::LENGTH;
            break;
        }

        case(MessageType::REPLACE_ORDER): {
            auto msg = ReplaceMessage::parseMessage(message);
            orderBook_->modifyOrder(msg.oldOrderId, msg.newOrderId,msg.newPrice,msg.numShares);
            message += ReduceOrderMessage::LENGTH;
            break;
        }

        case(MessageType::TRADE): {
            auto msg = TradeMessage::parseMessage(message);
            //have to LOG this trade, a message for Order Executed would have been processed prior to this
            //logger.log
            message += TradeMessage::LENGTH;
            break;
        }
    }
}
