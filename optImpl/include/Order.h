#pragma once
#include "typedefs.h"


struct Order {
    OrderId orderId_;
    Price price_;
    Quantity quantity_;
    Side side_;

    Order* nextOrder_ = nullptr;
    Order* prevOrder_ = nullptr;

    Order() = default;
    explicit Order(OrderId orderId, Price price, Quantity quantity, Side side, Order* nextOrder,Order* prevOrder) noexcept :
        orderId_(orderId),
        price_(price),
        quantity_(quantity),
        side_(side),
        nextOrder_(nextOrder),
        prevOrder_(prevOrder) {}
};
