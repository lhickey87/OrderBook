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

static auto get16(const char* memPtr){
    uint16_t result;
    std::memcpy(&result,memPtr,sizeof(result));
    return be16toh(result);
}

static auto get32(const char* memPtr){
    uint32_t result;
    std::memcpy(&result,memPtr,sizeof(result));
    return be32toh(result);
}

static auto get64(const char* memPtr) {
    uint64_t result; //this will be the destination of std::memcpy
    std::memcpy(&result, memPtr,sizeof(result));
    return be64toh(result);
}

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
    }
};


template<>
struct Message<MessageType::ADD_ORDER_MPID> {
    static constexpr uint8_t msgLength = 40;
    static Message parseMessage;
};

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
    }
};

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
    }
};

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
    }
};

template<>
struct Message<MessageType::DELETE_ORDER> {
    Message(Time time, OrderId orderId)
        : time_(time),
          cancelOrderId(orderId){}
    static constexpr uint16_t msgLength = 19;
    const Time time_;
    const OrderId cancelOrderId;
    static Message parseMessage(const char* bufPtr){}
};

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
    }
};

template<>
struct Message<MessageType::TRADE> {
    Message(Time time, OrderId orderId, Side side, Quantity quantity,
        TickerId ticker,Price price, uint64_t matchNum)
        : time_(time),
          orderId_(orderId),
          side_(side),
          sharesMatched(quantity),
          ticker_(ticker),
          matchPrice(price),
          matchNumber(matchNum){}
    static constexpr uint16_t msgLength = 44;
    const Time time_;
    const OrderId orderId_;
    const Side side_;
    const Quantity sharesMatched;
    const TickerId ticker_;
    const Price matchPrice;
    const uint64_t matchNumber;

    static Message parseMessage(const char* bufPtr){
    }
};
