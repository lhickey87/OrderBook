#include "../include/Engine.h"


auto Engine::readMessage() {

    while (bufferQueue_->isEmpty()){
        std::this_thread::yield();
    }

    const ReadBuffer* bufPtr = bufferQueue_->getReadElement();
}

auto Engine::handleBuffer(const ReadBuffer* bufPtr) {
    size_t bufferSize = bufPtr->size;
    size_t remainingBytes = bufferSize;
    const char* buffer = bufPtr->buffer->data();
    while (remainingBytes > 0){
        //here is where we will read messages

    }

}

//could assume that we call handleMessage after getting the length of the message
auto Engine::handleMessage(const char* message){
    const MessageType msgType = MessageType(*message); //points to 3rd bit containing message Type
    message += 1; //moves the pointer past the message type, now pointing to first element
    switch (msgType){
        case (MessageType::ADD_ORDER): {
            Message<MessageType::ADD_ORDER> msg = Message<MessageType::ADD_ORDER>::parseMessage(message);
            orderBook->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);
            //there will also be a timestamp which we would pass to our logger queue
            break;
        }

        case(MessageType::ADD_ORDER_MPID): {
            Message<MessageType::ADD_ORDER> msg = Message<MessageType::ADD_ORDER>::parseMessage(message);
            orderBook->add(msg.orderId_,msg.side_,msg.price_,msg.orderQuantity_);
            break;
        }

        case(MessageType::DELETE_ORDER): {
            Message<MessageType::DELETE_ORDER> msg = Message<MessageType::DELETE_ORDER>::parseMessage(message);
            orderBook->deleteOrder(msg.cancelOrderId);
            break;
        }

        case(MessageType::EXECUTE_ORDER): {
            auto msg = Message<MessageType::EXECUTE_ORDER>::parseMessage(message);
            orderBook->fillPassiveOrder(msg.orderId_,msg.numShares, false);
            break;
        }

        case(MessageType::EXECUTE_ORDER_WITH_PRICE): {
            auto msg = Message<MessageType::EXECUTE_ORDER_WITH_PRICE>::parseMessage(message);
            orderBook->fillPassiveOrder(msg.orderId_,msg.numShares, true);
            break;
        }

        case(MessageType::REDUCE_ORDER): {
            auto msg = Message<MessageType::REDUCE_ORDER>::parseMessage(message);
            orderBook->reduceOrder(msg.orderId_,msg.cancelledShares);
            break;
        }

        case(MessageType::REPLACE_ORDER): {
            auto msg = Message<MessageType::REPLACE_ORDER>::parseMessage(message);
            orderBook->modifyOrder(msg.oldOrderId, msg.newOrderId,msg.newPrice,msg.numShares);
            break;
        }

        case(MessageType::TRADE): {
            auto msg = Message<MessageType::TRADE>::parseMessage(message);
            break;
        }
    }
}
