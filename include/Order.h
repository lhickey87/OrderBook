#pragma once
#include <optional>
#include "typedefs.h"



struct Order {
    Order() = default;
    explicit Order(OrderId orderId, Price price, Quantity quantity, ClientId clientId,Side side) noexcept :
        orderId_(orderId),
        price_(price),
        quantity_(quantity),
        clientId_(clientId),
        side_(side){}

    static constexpr ClientId NO_MPID = -1;

    OrderId orderId_;
    Price price_;
    Quantity quantity_;
    ClientId clientId_;
    Side side_;
    bool isFilled = false;

    Order* nextOrder_ = nullptr;
    Order* prevOrder_ = nullptr;


    void Fill(Quantity quantity){
        quantity_ -= quantity;
        if (quantity_ < 0){
            quantity_ = 0;
            isFilled = true;
        }
    }
};
