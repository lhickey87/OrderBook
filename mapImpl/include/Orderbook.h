#include "LevelData.h"
#include "Trade.h"
#include <map>



//we should be storing our orders in a preInitialized
using levelVector = std::vector<LevelInfo>; //map will be indexed by price
using OrderVector = std::deque<OrderPointer>;

class Orderbook {

public:

    void cancelOrder(OrderId orderId);
    Trades addOrder(Order order);
    Trades Match();
    void LevelOrderAdded(OrderPointer order);
    auto& getBids() {return bids_;}
    auto& getAsks() {return asks_;}

private:
    // where OrderData would contain the orderPointer to the order, and an iterator to where it is in the deque?
    struct OrderMetaData {
        OrderPointer orderPointer;

        size_t index;
    };
    std::unordered_map<OrderId, OrderMetaData> orders_; //may be best to implement orderID from scratch instead of using inefficient unordered_map
    std::map<Price, OrderVector, std::greater<Price>> bids_;
    std::map<Price, OrderVector, std::less<Price>> asks_;
    void levelOrderEdit(Side side, Price price, Quantity quantity);
    OrderPointer toOrderPointer(Order order);
    Price bestBidLevel();
    Price bestAskLevel();
    void updateLevel(Price price);
    Quantity getLevelQuantity();
};

//what the fuck is this font


//what data structure to use here?
//
