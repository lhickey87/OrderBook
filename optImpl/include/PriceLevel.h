#pragma once
#include "Order.h"


struct PriceLevelOrders {
    Side side_;
    Price price_;

    Order* headOrder;

    PriceLevelOrders* nextPrice_;
    PriceLevelOrders* prevPrice_;

    explicit PriceLevelOrders(Side side, Price price, PriceLevelOrders* nextPrice, PriceLevelOrders* prevPrice):
        side_(side),
        price_(price),
        nextPrice_(nextPrice),
        prevPrice_(prevPrice) {}
};
