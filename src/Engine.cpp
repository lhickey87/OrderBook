#include "../include/Engine.h"
#include <cstring>

//this would likely be in
auto Engine::readMessage() {

    while (bufferQueue_->isEmpty()){
        std::this_thread::yield();
    }

    auto bufPtr = bufferQueue_->getReadElement();
    handleBuffer(bufPtr);
    //what if we have leftover buffer
}

void Engine::handleBuffer(const ReadBuffer* bufPtr) {
    size_t remainingBytes = bufPtr->size;
    const char* buffer = bufPtr->buffer->data(); //this is the actual std::array of chars
    uint16_t length;
    while (remainingBytes > 0){
        // may not actually be neccessary to hgave a handleMessage method
        length = get16bit(buffer);
        if (length > remainingBytes) [[unlikely]]{
            std::memcpy(&splicedMessage, buffer, remainingBytes);
            //will need to figure out how to handle the spliced message
            break;
        } //otherwise we can handle the Message
        handleMessage(buffer);
        remainingBytes -= length;
    }
}

//could assume that we call handleMessage after getting the length of the message
void Engine::handleMessage(const char* message){
    const MessageType msgType = MessageType(*message); //points to 3rd bit containing message Type
    switch (msgType){
        case (MessageType::ADD_ORDER): {
            auto msg = AddOrderMessage::parseMessage(message);
            orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);
            //there will also be a timestamp which we would pass to our logger queue
            message += AddOrderMessage::msgLength;
            break;
        }

        case(MessageType::ADD_ORDER_MPID): {
            auto msg = IdAddOrderMessage::parseMessage(message);
            message += IdAddOrderMessage::msgLength;
            break;
        }

        case(MessageType::DELETE_ORDER): {
            auto msg = DeleteMessage::parseMessage(message);
            orderBook_->deleteOrder(msg.cancelOrderId);
            message += DeleteMessage::msgLength;
            break;
        }

        case(MessageType::EXECUTE_ORDER): {
            auto msg = ExecMessage::parseMessage(message);
            orderBook_->fillPassiveOrder(msg.orderId_,msg.numShares, WITHOUT_PRICE);
            message += ExecMessage::msgLength;
            break;
        }

        case(MessageType::EXECUTE_ORDER_WITH_PRICE): {
            auto msg = ExecPriceMessage::parseMessage(message);
            orderBook_->fillPassiveOrder(msg.orderId_,msg.numShares, WITH_PRICE);
            message += ExecPriceMessage::msgLength;
            break;
        }

        case(MessageType::REDUCE_ORDER): {
            auto msg = ReduceOrderMessage::parseMessage(message);
            orderBook_->reduceOrder(msg.orderId_,msg.cancelledShares);
            message += ReduceOrderMessage::msgLength;
            break;
        }

        case(MessageType::REPLACE_ORDER): {
            auto msg = ReplaceMessage::parseMessage(message);
            orderBook_->modifyOrder(msg.oldOrderId, msg.newOrderId,msg.newPrice,msg.numShares);
            message += ReplaceMessage::msgLength;
            break;
        }

        case(MessageType::TRADE): {
            auto msg = TradeMessage::parseMessage(message);
            //have to LOG this trade, a message for Order Executed would have been processed prior to this
            message += TradeMessage::msgLength;
            break;
        }
    }
}
