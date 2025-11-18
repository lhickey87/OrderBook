#pragma once
#include "typedefs.h"

struct Order {
    Order() = default;
    explicit Order(OrderId orderId, Price price, Quantity quantity, Side side, Order* nextOrder,Order* prevOrder) noexcept :
        orderId_(orderId),
        price_(price),
        quantity_(quantity),
        side_(side),
        nextOrder_(nextOrder),
        prevOrder_(prevOrder) {}

    OrderId orderId_;
    Price price_;
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
