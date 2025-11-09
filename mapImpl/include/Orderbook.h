#include "LevelData.h"
#include "Trade.h"


//we should be storing our orders in a preInitialized
using BuyLevelData = LevelData<Side::BUY>;
using SellLevelData = LevelData<Side::SELL>;

class Orderbook {

public:


    void cancelOrder(OrderId orderId);
    Trades addOrder(Order order);
    Trades Match();

private:
    //std::unordered_map<Price, LevelInfo> levels_; probably don't need this as we will use static LevelData instead
    //also need an order to keep track of orders, when we want to delete an order, we would need to keep track of an iterator
    //so our orders data structure could be simply an unordered_map with key OrderId and value being OrderData
    // where OrderData would contain the orderPointer to the order, and an iterator to where it is in the deque?
    std::unordered_map<OrderId, OrderPointer> orders_; //may be best to implement orderID from scratch instead of using inefficient unordered_map
    Price getBestBid();
    Price getBestAsk();
    Quantity getLevelQuantity();

};

//what the fuck is this font 


//what data structure to use here?
//
