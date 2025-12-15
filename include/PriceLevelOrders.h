#pragma once
#include "Order.h"
#include "typedefs.h"

struct PriceLevelOrders {
    Side side_;
    Price price_;

    Order* headOrder;

    PriceLevelOrders* nextPrice_;
    PriceLevelOrders* prevPrice_;

    explicit PriceLevelOrders(Side side, Price price, Order* head):
        side_(side),
        price_(price),
        headOrder(head){}

    PriceLevelOrders& operator=(PriceLevelOrders&&) = delete;
    PriceLevelOrders(PriceLevelOrders&&) = delete;

    PriceLevelOrders() = default;
};
