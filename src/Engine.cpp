#include "../include/Engine.h"


auto Engine::readMessage() {

    while (bufferQueue_->isEmpty()){
        std::this_thread::yield();
    }

    const ReadBuffer* bufPtr = bufferQueue_->getReadElement();
}

auto Engine::handleBuffer(const ReadBuffer* bufPtr) {
    size_t remainingBytes = bufPtr->size;
    const char* buffer = bufPtr->buffer->data(); //this is the actual std::array of chars
    while (remainingBytes > 0){
        //here is where we will read messages
        const auto length = get16(buffer);
        if (length > remainingBytes) [[unlikely]]{
            //will have to call std::memcpy
        } //otherwise we can handle the Message
        handleMessage(buffer);

    }
}

//could assume that we call handleMessage after getting the length of the message
auto Engine::handleMessage(const char* message){
    const MessageType msgType = MessageType(*message); //points to 3rd bit containing message Type
    switch (msgType){
        case (MessageType::ADD_ORDER): {
            auto msg = AddOrderMessage::parseMessage(message);
            orderBook->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);
            //there will also be a timestamp which we would pass to our logger queue
            message += AddOrderMessage::msgLength;
            break;
        }

        case(MessageType::ADD_ORDER_MPID): {
            auto msg = IdAddOrderMessage::parseMessage(message);
            orderBook->add();
            message += IdAddOrderMessage::msgLength;
            break;
        }

        case(MessageType::DELETE_ORDER): {
            auto msg = DeleteOrderMessage::parseMessage(message);
            orderBook->deleteOrder(msg.cancelOrderId);
            message += DeleteOrderMessage::msgLength;
            break;
        }

        case(MessageType::EXECUTE_ORDER): {
            auto msg = ExecuteOrderMessage::parseMessage(message);
            orderBook->fillPassiveOrder(msg.orderId_,msg.numShares, false);
            message += ExecuteOrderMessage::msgLength;
            break;
        }

        case(MessageType::EXECUTE_ORDER_WITH_PRICE): {
            auto msg = ExecutePriceOrderMessage::parseMessage(message);
            orderBook->fillPassiveOrder(msg.orderId_,msg.numShares, true);
            message += ExecutePriceOrderMessage::msgLength;
            break;
        }

        case(MessageType::REDUCE_ORDER): {
            auto msg = ReduceOrderMessage::parseMessage(message);
            orderBook->reduceOrder(msg.orderId_,msg.cancelledShares);
            message +=
            break;
        }

        case(MessageType::REPLACE_ORDER): {
            auto msg = ReplaceOrderMessage::parseMessage(message);
            orderBook->modifyOrder(msg.oldOrderId, msg.newOrderId,msg.newPrice,msg.numShares);
            break;
        }

        case(MessageType::TRADE): {
            auto msg = TradeMessage::parseMessage(message);
            //have to LOG this trade, a message for Order Executed would have been processed prior to this
            break;
        }
    }
}
