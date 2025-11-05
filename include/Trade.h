#include "order.h"
#include <vector>


//trades should be an object
class Trade {
    //will old both orderId's

    Trade() :
    {}

private:
    OrderId buyOrder;
    OrderId sellOrder;
    Quantity quantity_;
    Price price_;

};
