#pragma once
#include <memory>
#include <ostream>
#include "typedefs.h"
#include "endian.h"

enum class MessageType {
    ADD_ORDER = 'A',         // 36
    ADD_ORDER_MPID = 'F',
    EXECUTE_ORDER = 'E',
    EXECUTE_ORDER_WITH_PRICE = 'C',
    REDUCE_ORDER = 'X',
    DELETE_ORDER = 'D',
    REPLACE_ORDER = 'U',
    TRADE = 'P'
};


static auto get16bit(const Byte* memPtr){
    uint16_t result;
    std::memcpy(&result,memPtr,sizeof(result));
    return be16toh(result);
}

static auto get32bit(const Byte* memPtr){
    uint32_t result;
    std::memcpy(&result,memPtr,sizeof(result));
    return be32toh(result);
}

static auto get64bit(const Byte* memPtr) {
    uint64_t result; //this will be the destination of std::memcpy
    std::memcpy(&result, memPtr,sizeof(result));
    return be64toh(result);
}

static auto getOrderId(const Byte* msgPtr){return get64bit(msgPtr);}
static auto getQuantity(const Byte* msgPtr){return get32bit(msgPtr);}
static auto getPrice(const Byte* msgPtr){return get32bit(msgPtr);}
static auto getTicker(const Byte* msgPtr){ return get64bit(msgPtr);}
static auto getSide(const Byte* msgPtr){return Side(*msgPtr);}
static Time getTime(const Byte* msgPtr){
    uint64_t result;
    Byte* memPtr = (Byte *)&result;
    std::memcpy(memPtr, msgPtr, 6);
    return (get64bit(memPtr) >> 16);
}
static inline auto getMsgLength(const Byte *data){
    return get16bit(data);
}

template<MessageType msg>
struct Message {
    static constexpr MessageType messageType_ = msg;

    static Message parse(const Byte *ptr) {
        static_cast<void>(ptr);
        return Message();
    }

    void print(std::ostream& outputStream) const {
       outputStream << "base \n";
    }
};

using AddOrderMessage = Message<MessageType::ADD_ORDER>;
template<>
struct Message<MessageType::ADD_ORDER> {

    Message(Time time, OrderId orderId, TickerId ticker,
            Quantity quantity, Price price, Side side)
            : time_(time),
              orderId_(orderId),
              stockTicker_(ticker),
              orderQuantity_(quantity),
              price_(price),
              side_(side){}

    static constexpr uint16_t LENGTH = 36;

    const Time time_; //8 bytes
    const OrderId orderId_; //8 bytes
    const TickerId stockTicker_;
    const Quantity orderQuantity_;
    const Price price_;
    const Side side_; //1 byte

    static Message parseMessage(const Byte* bufPtr){
    return AddOrderMessage(getTime(bufPtr+5), getOrderId(bufPtr+11), getTicker(bufPtr+24),
                           getQuantity(bufPtr+20), getPrice(bufPtr+32), getSide(bufPtr+19));
    }

    auto print(std::ostream& outputStream) const {
        outputStream << "- Add Order Message -" << "\n"
        << "OrderId: " << orderId_ << "\n"
        << "Quantity: " << orderQuantity_ << "\n"
        << "Price: " << price_ << "\n"
        << "Ticker: " << stockTicker_ << "\n";
    }
};

using IdAddOrderMessage = Message<MessageType::ADD_ORDER_MPID>;
template<>
struct Message<MessageType::ADD_ORDER_MPID> {
    Message(Time time,OrderId orderId, Quantity quantity,
            TickerId ticker, Price price,ClientId clientId,
            Side side)
            : time_(time),
              orderId_(orderId),
              orderQuantity_(quantity),
              stockTicker_(ticker),
              price_(price),
              clientId_(clientId),
              side_(side){}

    static constexpr uint16_t LENGTH = 40;
    //add order and add order MPID are very similar, what could be done to
    const Time time_; //4 bytes
    const OrderId orderId_; //8 bytes
    const Quantity orderQuantity_;
    const TickerId stockTicker_;
    const Price price_;
    const ClientId clientId_;
    const Side side_; //1 byte
    static Message parseMessage(const Byte* bufPtr){
    return IdAddOrderMessage(getTime(bufPtr+5),getOrderId(bufPtr+11),getQuantity(bufPtr+20),
                             getTicker(bufPtr+24), getPrice(bufPtr+32), get32bit(bufPtr+36),
                            getSide(bufPtr+19));
    }

    auto print(std::ostream& outputStream) const {
        outputStream << "- Add Order Message -" << "\n"
        << "OrderId: " << orderId_ << "\n"
        << "Quantity: " << orderQuantity_ << "\n"
        << "Price: " << price_ << "\n"
        << "Ticker: " << stockTicker_ << "\n";
    }
};

using ExecMessage = Message<MessageType::EXECUTE_ORDER>;
template<>
struct Message<MessageType::EXECUTE_ORDER> {

    Message(OrderId orderId, Time time, uint64_t matchNum, Quantity quantity)
        : orderId_(orderId),
          time_(time),
          matchNumber(matchNum),
          numShares(quantity){}

    static constexpr uint16_t LENGTH = 31;
    const OrderId orderId_; //64 bits
    const Time time_; //
    const uint64_t matchNumber; //8 bits
    const Quantity numShares; //4 bits

    static Message parseMessage(const Byte* bufPtr){
        return ExecMessage(getOrderId(bufPtr+11),getTime(bufPtr+5),get64bit(bufPtr+23),getQuantity(bufPtr+19));
    }

    auto print(std::ostream& outputStream) const {
        outputStream << "- Execute Order Message -" << "\n"
        << "OrderId: " << orderId_ << "\n"
        << "Quantity: " << numShares << "\n"
        << "Match Number" << matchNumber << "\n";
    }
};


using ExecPriceMessage = Message<MessageType::EXECUTE_ORDER_WITH_PRICE>;
template<>
struct Message<MessageType::EXECUTE_ORDER_WITH_PRICE> {

    Message(Time time,  OrderId orderId, uint64_t matchNum,
            Quantity quantity, Price price)
            : time_(time),
              orderId_(orderId),
              matchNumber(matchNum),
              numShares(quantity),
              execPrice(price){}

    static constexpr uint16_t LENGTH = 36;
    const Time time_;
    const OrderId orderId_;
    const uint64_t matchNumber;
    const Quantity numShares;
    const Price execPrice;
    static Message parseMessage(const Byte* bufPtr){
        return ExecPriceMessage(getTime(bufPtr+5), getOrderId(bufPtr+11), get64bit(bufPtr+23),
                                getQuantity(bufPtr+19), getPrice(bufPtr+32));
    }

    auto print(std::ostream& outputStream) const {
        outputStream << "- Execute Order With Price Message -" << "\n"
        << "New OrderId: " << orderId_ << "\n"
        << "Quantity: " << numShares << "\n"
        << "Price: " << execPrice << "\n"
        << "Match Number" << matchNumber << "\n";
    }
};

using ReduceOrderMessage = Message<MessageType::REDUCE_ORDER>;
template<>
struct Message<MessageType::REDUCE_ORDER> {

    Message(Time time, OrderId orderId, Quantity quantity)
           : time_(time),
             orderId_(orderId),
             cancelledShares(quantity){}
    static constexpr uint16_t LENGTH = 23;

    const Time time_;
    const OrderId orderId_;
    const Quantity cancelledShares;
    static Message parseMessage(const Byte* bufPtr){
        return ReduceOrderMessage(getTime(bufPtr+5),getOrderId(bufPtr+11), getQuantity(bufPtr+19));
    }

    auto print(std::ostream& outputStream) const {
        outputStream << "- Reduce Order Message -" << "\n"
        << "OrderId: " << orderId_ << "\n"
        << "Quantity Canceled: " << cancelledShares << "\n";
    }
};

using DeleteMessage = Message<MessageType::DELETE_ORDER>;
template<>
struct Message<MessageType::DELETE_ORDER> {

    Message(Time time, OrderId orderId)
        : time_(time),
          cancelOrderId(orderId){}
    static constexpr uint16_t LENGTH = 19;

    const Time time_;
    const OrderId cancelOrderId;
    static Message parseMessage(const Byte* bufPtr){
        return DeleteMessage(getTime(bufPtr+5), getOrderId(bufPtr+11));
    }

    auto print(std::ostream& outputStream) const {
        outputStream << "- Delete Order Message -" << "\n"
        << "OrderId: " << cancelOrderId << "\n";
    }
};

using ReplaceMessage =Message<MessageType::REPLACE_ORDER>;
template<>
struct Message<MessageType::REPLACE_ORDER> {

    Message(Time time, OrderId oldOrder, OrderId newOrder,
            Quantity quantity, Price price)
            : time_(time),
              oldOrderId(oldOrder),
              newOrderId(newOrder),
              numShares(quantity),
              newPrice(price){}
    static constexpr uint16_t LENGTH = 35;

    const Time time_;
    const OrderId oldOrderId;
    const OrderId newOrderId;
    const Quantity numShares;
    const Price newPrice;

    static Message parseMessage(const Byte* bufPtr){
        return ReplaceMessage(getTime(bufPtr+5), getOrderId(bufPtr+11),getOrderId(bufPtr+19),
                                                   getQuantity(bufPtr+27), getPrice(bufPtr+31));
    }

    auto print(std::ostream& outputStream) const {
        outputStream << "- Replace Order Message -" << "\n"
        << "Old OrderId: " << oldOrderId << "\n"
        << "New OrderId: " << newOrderId << "\n"
        << "Quantity: " << numShares << "\n"
        << "Price: " << newPrice << "\n";
    }
};

using TradeMessage = Message<MessageType::TRADE>;
template<>
struct Message<MessageType::TRADE> {

    Message(Time time, OrderId orderId,uint64_t matchNum,
            TickerId ticker,Quantity quantity, Price price,
            Side side)
            : time_(time),
              orderId_(orderId),
              matchNumber(matchNum),
              ticker_(ticker),
              sharesMatched(quantity),
              price_(price),
              side_(side){}

    static constexpr uint16_t LENGTH = 44;

    const Time time_;
    const OrderId orderId_;
    const uint64_t matchNumber;
    const TickerId ticker_;
    const Quantity sharesMatched;
    const Price price_;
    const Side side_;

    static Message parseMessage(const Byte* bufPtr){
        return TradeMessage(getTime(bufPtr+5), getOrderId(bufPtr+11), get64bit(bufPtr+36),
                            getTicker(bufPtr+24), getQuantity(bufPtr+20), getPrice(bufPtr+32),
                            getSide(bufPtr+19));
    }

    auto print(std::ostream& outputStream) const {
        outputStream << "- Trade Message -" << "\n"
        << "OrderId: " << orderId_ << "\n"
        << "Ticker: " << ticker_ << "\n"
        << "Quantity: " << sharesMatched << "\n"
        << "Price: " << price_ << "\n";
    }
};

template <MessageType MsgType>
std::ostream& operator<<(std::ostream& outputStream, const Message<MsgType>& msg){
    msg.print(outputStream);
    return outputStream;
}


static constexpr std::array<MessageType, 45> typeTable() {
    std::array<MessageType, 45> typeMap{};

    // initialize all entries to a default value if needed
    int count = 0;
    while (count < 45) {
        typeMap[count] = MessageType::BULLSHIT; // or some invalid/default type
        count++;
    }

    typeMap[DeleteMessage::LENGTH] = MessageType::DELETE_ORDER;
    typeMap[ReduceOrderMessage::LENGTH] = MessageType::REDUCE_ORDER;
    typeMap[ReplaceMessage::LENGTH] = MessageType::REPLACE_ORDER;
    typeMap[AddOrderMessage::LENGTH] = MessageType::ADD_ORDER;
    typeMap[IdAddOrderMessage::LENGTH] = MessageType::ADD_ORDER_MPID;
    typeMap[ExecMessage::LENGTH] = MessageType::EXECUTE_ORDER;
    typeMap[TradeMessage::LENGTH] = MessageType::TRADE;

    return typeMap;
}

static constexpr std::array<MessageType,45> MessageTable = typeTable();
