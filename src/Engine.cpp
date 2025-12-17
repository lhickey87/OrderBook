#include "../include/Engine.h"
#include <iostream>

void Engine::run(){
    while (true){
        while (bufferQueue_->isEmpty()){
            std::this_thread::yield();
            continue;
        }
        const ReadBuffer* bufPtr = bufferQueue_->getReadElement();

        //this will be true once DataFeed has read through entire binary file
        if (bufPtr->size == 0) [[unlikely]]{ break;}

        handleBuffer(bufPtr);
        bufferQueue_->incReadIndex();
    }
    //logger_->logStop();
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
            auto msg = AddOrderMessage::parseMessage(message);
            msg.print(std::cout);
            //orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_);
            //logger_->logOrderAdd(msg.orderId_, msg.orderQuantity_, msg.price_, msg.side_);
            break;
        }

        case(MessageType::ADD_ORDER_MPID): {
            auto msg = IdAddOrderMessage::parseMessage(message);
            msg.print(std::cout);
            //orderBook_->add(msg.orderId_, msg.side_,msg.price_, msg.orderQuantity_, msg.clientId_);
            //logger_->logOrderAdd(msg.orderId_, msg.orderQuantity_, msg.price_, msg.side_);
            break;
        }

        case(MessageType::DELETE_ORDER): {
            auto msg = DeleteMessage::parseMessage(message);
            msg.print(std::cout);
            //orderBook_->deleteOrder(msg.cancelOrderId);
//            logger_->logOrderDelete(msg.cancelOrderId);
            break;
        }

        case(MessageType::EXECUTE_ORDER): {
            auto msg = ExecMessage::parseMessage(message);
            msg.print(std::cout);
            //orderBook_->executeOrder(msg.orderId_,msg.numShares);
 //           logger_->logOrderExec(msg.orderId_, msg.numShares);
            break;
        }

        case(MessageType::EXECUTE_ORDER_WITH_PRICE): {
            auto msg = ExecPriceMessage::parseMessage(message);
            msg.print(std::cout);
            //orderBook_->executeOrderAtPrice(msg.orderId_,msg.numShares);
            //logger_->logOrderExec(msg.orderId_, msg.numShares);
            break;
        }

        case(MessageType::REDUCE_ORDER): {
            auto msg = ReduceOrderMessage::parseMessage(message);
            msg.print(std::cout);
            //orderBook_->reduceOrder(msg.orderId_,msg.cancelledShares);
            //logger_->logOrderReduce(msg.orderId_, msg.cancelledShares);
            break;
        }

        case(MessageType::REPLACE_ORDER): {
            auto msg = ReplaceMessage::parseMessage(message);
            msg.print(std::cout);
            //orderBook_->modifyOrder(msg.oldOrderId, msg.newOrderId,msg.newPrice,msg.numShares);
            //logger_->logOrderModify(msg.oldOrderId, msg.newOrderId, msg.numShares, msg.newPrice);
            break;
        }

        case(MessageType::BULLSHIT):
            std::cout << "Non standard message!" << std::endl;
            break;

        case(MessageType::TRADE): {
            auto msg = TradeMessage::parseMessage(message);
            msg.print(std::cout);
            //logger_->logTrade(msg.sharesMatched, msg.price_);
            break;
        }
    }
}
