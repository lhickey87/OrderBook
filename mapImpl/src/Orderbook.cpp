#include "../include/Orderbook.h"
#include <cstddef>

Trades Orderbook::addOrder(Order order){
    if (!orders_.contains(order.getOrderId())){
        return {}; //this order already exists
    }

    auto orderPtr = toOrderPointer(order);
    if (order.getSide() == Side::BUY){
        //so now we must add this order to a particular price level
        const auto& bidVector = bids_[order.getLevel()]; //this gives us
        size_t index = bidVector.size()+1;
        bidVector.push_back(orderPtr);
    } else {
        const auto& askVector = asks_[order.getLevel()];
        size_t index = askVector.size()+1;
        askVector.push_back(orderPtr);
    }

    //if we have a current order's orderId we can find it's exact location in our vector
    orders_.insert({orderPtr->getOrderId(), bookEntry{orderPtr, index} } );
    LevelOrderAdded(orderPtr);

    return Match();
}

//would be interesting to use templates here in some fashion
void levelOrderEdit(Side side, Price price, Quantity quantity){
    if (side == Side::BUY){
        LevelData<Side::BUY>::editLevelData(price, quantity);
    } else {
        LevelData<Side::SELL>::editLevelData(price, quantity);
    }
}

void Orderbook::LevelOrderAdded(OrderPointer order){
    //we must add to levelData
    levelOrderEdit(order->getSide(), order->getLevel(), order->getRemaining());
}


void Orderbook::cancelOrder(OrderId orderId){}

Trades Orderbook::Match() {
    if (bids_.empty() || asks_.empty()){
       return { }; //no trades matched
    }

    Trades trades;
    trades.reserve(orders_.size());
    while (true){
        //get bids and asks front
        const auto& [bestBid, bidVector] = *bids_.begin();
        const auto& [bestAsk, askVector] = *asks_.begin();

        if (bidVector.empty() || askVector.empty() || bestAsk > bestBid){
            break;
        }

        while (!bidVector.empty() && askVector.empty()){
            auto ask = askVector.front();
            auto bid = bidVector.front();
            Quantity quantity = std::min(ask->getRemaining(),bid->getRemaining());

            ask->Fill(quantity);
            bid->Fill(quantity);

            if (ask->isFilled()){
                //if it's filled we definitely should remove from orders
                orders_.erase(ask->getOrderId());
            }

            if (bid->isFilled()){
                //bidVector.erase();
            }

        }

        //now time to match

    }
}

OrderPointer Orderbook::toOrderPointer(Order order){
   return std::make_unique<Order>(order);
}

//should this be from the levelData instance?
// probably should maintain a head
