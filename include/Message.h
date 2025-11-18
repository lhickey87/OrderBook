#pragma once
#include "typedefs.h"

enum class MessageType {

};

struct Message {
    Time timeStamp_;
    OrderId orderId_;
    OrderId newOrderId_;
    OrderId aggressiveOrderId;
    TickerId ticker_;
    Price price_;
    Quantity tradeVolume;
    Side aggressorSide_;


};
