#include "MemoryPool.h"
#include "PriceLevelOrders.h"
#include "typedefs.h"
#include <unordered_map>

class Orderbook {

public:
    Orderbook(): orderPool(MAX_ORDERS),priceLevelPool(MAXLEVELS), orderMap(MAX_ORDERS), priceLevelsMap(MAXLEVELS){}

    //private API's such as priceCrosses -> Match, addToPriceLevel, addOrder will all be called from this public method
    void add(OrderId orderId, Side side, Price price, Quantity quantity);
    void deleteOrder(OrderId orderId); //cancel from order's hashmap by setting to nullptr
    void recordAggressiveOrder(OrderId orderId, Price price, Quantity quantity);
    void fillPassiveOrder(OrderId orderId, Quantity quantity, bool withPrice);
    void reduceOrder(OrderId orderId, Quantity cancelled);
    void modifyOrder(OrderId oldOrderId, OrderId newOrderId, Price newPrice, Quantity quantity);

    //telling compiler not to generate any special member funtionms
   //Orderbook() = delete;
    Orderbook(Orderbook&&) = delete;
    Orderbook(const Orderbook&) = delete;
    Orderbook& operator=(const Orderbook&) = delete;
    Orderbook& operator=(Orderbook&&) = delete;

private:

    MemoryPool<Order*> orderPool;
    MemoryPool<PriceLevelOrders*> priceLevelPool;
    PriceLevelOrders* bids_ = nullptr;
    PriceLevelOrders* asks_ = nullptr;
    std::vector<Order*> orderMap;
    std::vector<PriceLevelOrders*> priceLevelsMap;
    size_t nextOrderId = 1;

    //if using particular exchange we may have to implement exchanged specific logic
    auto getNextOrderId() noexcept {
        return nextOrderId++;
    }

    void addOrder(Order* order) noexcept {
        //this will be called if our order is only partially filled, or not filled at all

        const auto levelOrders = getPriceLevel(order->price_);
        if (!levelOrders){
            order->nextOrder_ = order->prevOrder_ = order;
            auto newPriceLevel = priceLevelPool.Allocate(order->side_, order->price_, order, nullptr, nullptr);
            addToPriceLevel(*newPriceLevel);
        } else {

        }
    }

    void addToPriceLevel(PriceLevelOrders* newLevel) noexcept {

    }

    size_t priceToIndex(Price price) const {
        return price % MAXLEVELS;
    }

    PriceLevelOrders* getPriceLevel(Price price) const noexcept {
        return priceLevelsMap.at(priceToIndex(price));
    }

    //find price level
    void removePriceLevel(Price price) noexcept {}

    bool priceCrosses(Side side, Price price) const noexcept {
        if (side == Side::BUY){
            return price > asks_->price_;
        } else {
            //if sell, bid should be higher than sell
            return price < bids_->price_;
        }
    }

    //match will be called from within add IF we determine priceCrosses
    auto match(Side side, Price price, Quantity quantity) noexcept {
        Quantity remainingQuantity; //we can return this to our add function
        //in this setup, we will actually just match based on an agressor
        if (side == Side::BUY){

        } else {

        }

        return remainingQuantity;
    }


};
