#include <cstdint>
#include <vector>

using OrderId = std::uint64_t;
using Price = std::int32_t;
using Quantity = std::uint32_t;

enum class Side {BUY, SELL};

class Order {
    Order(OrderId orderId, Quantity quantity, Price price, Side side)
    : orderId_(orderId),
    quantity_(quantity),
    price_(price),
    side_(side),
    remaining_(quantity) {}

    OrderId getOrderId() const { return orderId_;}
    Quantity getRemaining() const { return quantity_;}
    Price getLevel() const {return price_;}
    Side getSide() const {return side_;}
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
