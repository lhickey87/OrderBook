#include "../include/Engine.h"

void Engine::run(){
    while (true){
        while (bufferQueue_->isEmpty()){}
        auto bufPtr = bufferQueue_->getReadElement();

        //this will be true once DataFeed has read through entire binary file
        if (bufPtr->size == 0) [[unlikely]]{ break;}

        handleBuffer(bufPtr);
        bufferQueue_->incReadIndex();
    }
}

void Engine::handleBuffer(const ReadBuffer* bufPtr) {
    size_t remainingBytes = bufPtr->size;
    const Byte* buffer = bufPtr->buffer->data(); //this is the actual std::array of chars

    auto length = getMsgLength(buffer)+HEADER_BYTES;
    buffer += HEADER_BYTES;
    while (remainingBytes > 0){

        if (length > remainingBytes) [[unlikely]]{
            std::memcpy(&splicedMessage, buffer, remainingBytes);
            break;
        }

        handleMessage(buffer, MessageType(*buffer));
        remainingBytes -= length;
        length = getMsgLength(buffer)+HEADER_BYTES;
    }
}

void Engine::handleMessage(const Byte* message,MessageType type){
    //need to handle the messageHeader which would be total of 5 bytes
    // 1 byte for type, 2 bytes for locate, 2 bytes for tracking num
    switch (type){
        case (MessageType::ADD_ORDER): {
            auto msg = AddOrderMessage::parseMessage(message);
            orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);
            message += AddOrderMessage::LENGTH + HEADER_BYTES;
            break;
        }

        case(MessageType::ADD_ORDER_MPID): {
            auto msg = IdAddOrderMessage::parseMessage(message);
            orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_, msg.clientId_);
            message += AddOrderMessage::LENGTH + HEADER_BYTES;
            break;
        }

        case(MessageType::DELETE_ORDER): {
            auto msg = DeleteMessage::parseMessage(message);
            orderBook_->deleteOrder(msg.cancelOrderId);
            message += DeleteMessage::LENGTH + HEADER_BYTES;
            break;
        }

        case(MessageType::EXECUTE_ORDER): {
            auto msg = ExecMessage::parseMessage(message);
            orderBook_->executeOrder(msg.orderId_,msg.numShares);
            message += ExecMessage::LENGTH + HEADER_BYTES;
            break;
        }

        case(MessageType::EXECUTE_ORDER_WITH_PRICE): {
            auto msg = ExecPriceMessage::parseMessage(message);
            orderBook_->executeOrderAtPrice(msg.orderId_,msg.numShares, msg.execPrice);
            message += ExecPriceMessage::LENGTH + HEADER_BYTES;
            break;
        }

        case(MessageType::REDUCE_ORDER): {
            auto msg = ReduceOrderMessage::parseMessage(message);
            orderBook_->reduceOrder(msg.orderId_,msg.cancelledShares);
            message += ReduceOrderMessage::LENGTH + HEADER_BYTES;
            break;
        }

        case(MessageType::REPLACE_ORDER): {
            auto msg = ReplaceMessage::parseMessage(message);
            orderBook_->modifyOrder(msg.oldOrderId, msg.newOrderId,msg.newPrice,msg.numShares);
            message += ReduceOrderMessage::LENGTH + HEADER_BYTES;
            break;
        }

        case(MessageType::TRADE): {
            auto msg = TradeMessage::parseMessage(message);
            //have to LOG this trade, a message for Order Executed would have been processed prior to this
            //logger.log
            message += TradeMessage::LENGTH + HEADER_BYTES;
            break;
        }
    }
}
