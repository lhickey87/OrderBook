#pragma once
#include "typedefs.h"
#include <memory>
#include "endian.h"

enum class MessageType {
    ADD_ORDER = 'A',         // 36
    ADD_ORDER_MPID = 'F',
    EXECUTE_ORDER = 'E',
    EXECUTE_ORDER_WITH_PRICE = 'C',
    REDUCE_ORDER = 'X',
    DELETE_ORDER = 'D',
    REPLACE_ORDER = 'U',
    TRADE = 'P',
};

static auto get16bit(const char* memPtr){
    uint16_t result;
    std::memcpy(&result,memPtr,sizeof(result));
    return be16toh(result);
}

static auto get32bit(const char* memPtr){
    uint32_t result;
    std::memcpy(&result,memPtr,sizeof(result));
    return be32toh(result);
}

static auto get64bit(const char* memPtr) {
    uint64_t result; //this will be the destination of std::memcpy
    std::memcpy(&result, memPtr,sizeof(result));
    return be64toh(result);
}

static auto getOrderId(const char* msgPtr){return get64bit(msgPtr);}
static auto getQuantity(const char* msgPtr){return get32bit(msgPtr);}
static auto getPrice(const char* msgPtr){return get32bit(msgPtr);}
static auto getTicker(const char* msgPtr){ return get64bit(msgPtr);}
static auto getSide(const char* msgPtr){return Side(*msgPtr);}
static auto getTime(const char* msgPtr){ return get32bit(msgPtr);}

template<MessageType msg>
struct Message {
    static constexpr MessageType messageType_ = msg;
    static Message parse(const char *ptr) {
        static_cast<void>(ptr);
        return Message();
    }
};

template<>
struct Message<MessageType::ADD_ORDER> {

    Message(Time time, OrderId orderId, Side side,
            Quantity quantity, TickerId ticker, Price price)
        : timeStamp_(time),
          orderId_(orderId),
          side_(side),
          orderQuantity_(quantity),
          stockTicker_(ticker),
          price_(price) {}
    static constexpr uint16_t msgLength = 36;
    const Time timeStamp_; //4 bytes
    const OrderId orderId_; //8 bytes
    const Side side_; //1 byte
    const Quantity orderQuantity_;
    const TickerId stockTicker_;
    const Price price_;

    static Message parseMessage(const char* msgPtr){
        return Message<MessageType::ADD_ORDER>(getTime(msgPtr+1),getOrderId(msgPtr+5), getSide(msgPtr+13),
                                               getQuantity(msgPtr+14), getTicker(msgPtr+18),getPrice(msgPtr+26));
    }
};

using AddOrderMessage = Message<MessageType::ADD_ORDER>;

template<>
struct Message<MessageType::ADD_ORDER_MPID> {

    Message(const AddOrderMessage baseMsg, uint32_t clientId) : base(baseMsg), clientId_(clientId){}
    static constexpr uint8_t msgLength = 40;
    const AddOrderMessage base;
    const uint32_t clientId_;
    static Message parseMessage(const char* bufPtr){
        return Message(AddOrderMessage::parseMessage(bufPtr), get32bit(bufPtr+36));
    }
};
using IdAddOrderMessage = Message<MessageType::ADD_ORDER_MPID>;

template<>
struct Message<MessageType::EXECUTE_ORDER> {
    Message(Time time, OrderId orderId, Quantity quantity, uint64_t matchNum)
        : time_(time),
          orderId_(orderId),
          numShares(quantity),
          matchNumber(matchNum){}

    static constexpr uint16_t msgLength = 31;
    const Time time_;
    const OrderId orderId_;
    const Quantity numShares;
    const uint64_t matchNumber;

    static Message parseMessage(const char* bufPtr){
        return Message<MessageType::EXECUTE_ORDER>(getTime(bufPtr+1), getOrderId(bufPtr+5),getQuantity(bufPtr+13), get64bit(bufPtr+17));
    }
};
using ExecMessage = Message<MessageType::EXECUTE_ORDER>;

using ExecPriceMessage = Message<MessageType::EXECUTE_ORDER_WITH_PRICE>;

template<>
struct Message<MessageType::EXECUTE_ORDER_WITH_PRICE> {
    Message(Time time, OrderId orderId, Quantity quantity,
            uint64_t matchNum, Price price)
            : time_(time),
              orderId_(orderId),
              numShares(quantity),
              matchNumber(matchNum),
              execPrice(price){}
    static constexpr uint16_t msgLength = 36;
    const Time time_;
    const OrderId orderId_;
    const Quantity numShares;
    const uint64_t matchNumber;
    const Price execPrice;
    static Message parseMessage(const char* bufPtr){
        return ExecPriceMessage(getTime(bufPtr+1), getOrderId(bufPtr+5),getQuantity(bufPtr+13),
                                get64bit(bufPtr+17), getPrice(bufPtr+26));
    }
};


using ReduceOrderMessage = Message<MessageType::REDUCE_ORDER>;
template<>
struct Message<MessageType::REDUCE_ORDER> {

    Message(Time time, OrderId orderId, Quantity quantity)
        : time_(time),
          orderId_(orderId),
          cancelledShares(quantity){}
    static constexpr uint16_t msgLength = 23;
    const Time time_;
    const OrderId orderId_;
    const Quantity cancelledShares;
    static Message parseMessage(const char* bufPtr){
        return ReduceOrderMessage(getTime(bufPtr+1),getOrderId(bufPtr+5),getQuantity(bufPtr+13));
    }
};

using DeleteMessage =Message<MessageType::DELETE_ORDER>;
template<>
struct Message<MessageType::DELETE_ORDER> {
    Message(Time time, OrderId orderId)
        : time_(time),
          cancelOrderId(orderId){}
    static constexpr uint16_t msgLength = 19;
    const Time time_;
    const OrderId cancelOrderId;
    static Message parseMessage(const char* bufPtr){
        return DeleteMessage(getTime(bufPtr+1),getOrderId(bufPtr+5));
    }
};

using ReplaceMessage =Message<MessageType::REPLACE_ORDER>;
template<>
struct Message<MessageType::REPLACE_ORDER> {
    Message(Time time, OrderId oldOrder, OrderId newOrder,
            Quantity quantity,Price price)
            : time_(time),
              oldOrderId(oldOrder),
              newOrderId(newOrder),
              numShares(quantity),
              newPrice(price){}
    static constexpr uint16_t msgLength = 35;
    const Time time_;
    const OrderId oldOrderId;
    const OrderId newOrderId;
    const Quantity numShares;
    const Price newPrice;

    static Message parseMessage(const char* bufPtr){
        return ReplaceMessage(getTime(bufPtr+1),getOrderId(bufPtr+5),getOrderId(bufPtr+13),
                              getQuantity(bufPtr+21),getPrice(bufPtr+25));
    }
};

using TradeMessage = Message<MessageType::TRADE>;
template<>
struct Message<MessageType::TRADE> {
    Message(Time time, Quantity quantity,
        TickerId ticker,Price price, uint64_t matchNum)
        : time_(time),
          sharesMatched(quantity),
          ticker_(ticker),
          matchPrice(price),
          matchNumber(matchNum){}
    static constexpr uint16_t msgLength = 44;
    const Time time_;
    const Quantity sharesMatched;
    const TickerId ticker_;
    const Price matchPrice;
    const uint64_t matchNumber;

    static Message parseMessage(const char* bufPtr){
        return TradeMessage(getTime(bufPtr+1), getQuantity(bufPtr+14), getTicker(bufPtr+18),
            getPrice(bufPtr+26),get64bit(bufPtr+30));
    }
};
