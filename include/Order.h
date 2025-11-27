#pragma once
#include <optional>
#include "typedefs.h"



struct Order {
    Order() = default;
    explicit Order(OrderId orderId, Price price, Quantity quantity, Side side,ClientId clientId) noexcept :
        orderId_(orderId),
        price_(price),
        quantity_(quantity),
        side_(side),
        clientId_(clientId){}

    static constexpr ClientId NO_MPID = -1;

    OrderId orderId_;
    Price price_;
    ClientId clientId_;
    Quantity quantity_;
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
