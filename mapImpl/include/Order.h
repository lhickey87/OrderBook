#include <cstdint>
#include <vector>
#include "Side.h"
#include "Alias.h"


class Order {

public:
    Order(OrderId orderId, Quantity quantity, Price price, Side side)
    : orderId_(orderId),
    quantity_(quantity),
    price_(price),
    side_(side),
    remaining_(quantity) {}

    const OrderId getOrderId() const { return orderId_;}

    const Quantity getRemaining() const { return quantity_;}

    const Price getLevel() const {return price_;}

    const Side getSide() const {return side_;}

    bool isFilled() const { return remaining_ == 0;}
    void Fill(Quantity quantity){
       if (quantity > quantity_){
           //error occured
       }
       quantity_ -= quantity;
    }

private:
    OrderId orderId_;
    Quantity quantity_;
    Price price_;
    Side side_; //buy or sell
    Quantity remaining_;
};

using OrderPointer = std::unique_ptr<Order>;
