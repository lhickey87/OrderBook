#pragma once
#include "Order.h"


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
};
