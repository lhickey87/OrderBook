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

    Order* getOrder(OrderId orderId) noexcept {return orderMap.at(orderId);}

    PriceLevelOrders* getPriceLevel(Price price) const noexcept {
        return priceLevelsMap.at(priceToIndex(price));
    }

    PriceLevelOrders* getSide(Side side) const {
        return (side == Side::BUY) ? bids_ : asks_;
    }

    Orderbook(Orderbook&&) = delete;
    Orderbook(const Orderbook&) = delete;
    Orderbook& operator=(const Orderbook&) = delete;
    Orderbook& operator=(Orderbook&&) = delete;

private:

    MemoryPool<Order> orderPool;
    MemoryPool<PriceLevelOrders*> priceLevelPool;
    PriceLevelOrders* bids_ = nullptr;
    PriceLevelOrders* asks_ = nullptr;
    //std::vector<Order*> orderMap;
    //std::vector<PriceLevelOrders*> priceLevelsMap;
    std::unordered_map<OrderId, Order*> orderMap;
    std::unordered_map<Price, PriceLevelOrders*> priceLevelsMap;
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
            addPriceLevel(*newPriceLevel);
        } else {
            auto BestSideLevel = (order->side_ == Side::BUY) ? bids_ : asks_;
            while (!priceLevelCompare(order->side_, BestSideLevel->price_, order->price_)){
                BestSideLevel = BestSideLevel->nextPrice_;
            }
            //once this is true, we have found the correct side Level now we need to add order to the back
            //
            addOrderToTail(order, BestSideLevel);
        }
    }

    void addOrderToTail(Order* newOrder, PriceLevelOrders* orderPriceLevel){
        auto tail = orderPriceLevel->headOrder->prevOrder_;
        auto head = orderPriceLevel->headOrder;
        tail->nextOrder_ = newOrder;
        newOrder->prevOrder_ = tail;

        newOrder->nextOrder_ = head;
        head->prevOrder_ = newOrder;
    }

    void addPriceLevel(PriceLevelOrders* newLevel) noexcept {
        //add to price level Hash map
        // find the spot where this price level can Rest
        auto& bestSideLevel = (newLevel->side_ == Side::BUY) ? bids_ : asks_;

        if (!bestSideLevel) [[unlikely]]{
            bestSideLevel = newLevel;
            newLevel->prevPrice_ = newLevel->nextPrice_ = newLevel;
            return;
        }

        auto currentPriceLevel = bestSideLevel;
        if (priceLevelCompare(newLevel->side_, currentPriceLevel->price_, newLevel->price_)){
            insertLevelBefore(bestSideLevel,newLevel);
            bestSideLevel = newLevel;
            return;
        }

        currentPriceLevel = currentPriceLevel->nextPrice_;
        while (currentPriceLevel != bestSideLevel && !priceLevelCompare(newLevel->side_, currentPriceLevel->price_, newLevel->price_)){
            currentPriceLevel = currentPriceLevel->nextPrice_;
        }

        insertLevelBefore(currentPriceLevel, newLevel);
    }

    void insertLevelBefore(PriceLevelOrders* currentLevel, PriceLevelOrders* newLevel) noexcept {
        auto prev = currentLevel->prevPrice_;

        newLevel->nextPrice_ = currentLevel;
        newLevel->prevPrice_ = prev;

        currentLevel->prevPrice_ = newLevel;
        prev->nextPrice_ = newLevel;
    }

    bool priceLevelCompare(Side side, Price currentPrice, Price newLevelPrice) noexcept {
        if (side == Side::BUY){
            return currentPrice < newLevelPrice;
        } else {
            return currentPrice > newLevelPrice;
        }
    }

    size_t priceToIndex(Price price) const {
        return price % MAXLEVELS;
    }


    void removePriceLevel(Price price, Side side) noexcept {}

};
