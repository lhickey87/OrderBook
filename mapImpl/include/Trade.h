#include "Order.h"


//trades should be an object
class Trade {
    //will old both orderId's
public:
    Trade(Order buyOrder, Order sellOrder, Quantity quantity, Price price) :
        buyOrder_{std::make_shared<Order>(buyOrder)},
        sellOrder_{std::make_shared<Order>(sellOrder)},
        quantity_(quantity),
        price_(price) {}

private:
    OrderPointer buyOrder_;
    OrderPointer sellOrder_;
    Quantity quantity_;
    Price price_;
};

using Trades = std::vector<Trade>;
