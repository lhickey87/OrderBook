#pragma once
#include "MemoryPool.h"
#include "LFQueue.h"
#include "Message.h"
#include "PriceLevelOrders.h"
#include "typedefs.h"
#include <unordered_map>

class Orderbook {

public:
    Orderbook(): orderPool(MAX_ORDERS),priceLevelPool(MAXLEVELS), orderMap(MAX_ORDERS), priceLevelsMap(MAXLEVELS){}

    void add(OrderId orderId, Side side, Price price, Quantity quantity, ClientId mpid = Order::NO_MPID);

    void deleteOrder(OrderId orderId); //cancel from order's hashmap by setting to nullptr

    void executeOrderAtPrice(OrderId orderId, Quantity quantity);

    void executeOrder(OrderId orderId, Quantity quantity);

    void reduceOrder(OrderId orderId, Quantity cancelled);

    void modifyOrder(OrderId oldOrderId, OrderId newOrderId, Price newPrice, Quantity quantity);
   //Orderbook() = delete;
    Order* getOrder(OrderId orderId) noexcept {
        auto it = orderMap.find(orderId);
        if (it == orderMap.end()) [[unlikely]] {return nullptr;}
        return it->second; // Direct access, no second lookup!
    }

    PriceLevelOrders* getPriceLevel(Price price) noexcept {
        auto it = priceLevelsMap.find(price);
        if (it == priceLevelsMap.end()) {return nullptr;}
        return it->second;
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
    MemoryPool<PriceLevelOrders> priceLevelPool;
    PriceLevelOrders* bids_ = nullptr;
    PriceLevelOrders* asks_ = nullptr;

    std::unordered_map<OrderId, Order*> orderMap;
    std::unordered_map<Price, PriceLevelOrders*> priceLevelsMap;
    size_t nextOrderId = 1;

    //void logEvent() noexcept {}
    //if using particular exchange we may have to implement exchanged specific logic
    auto getNextOrderId() noexcept {
        return nextOrderId++;
    }

    void addOrder(Order* order) noexcept {
        //this will be called if our order is only partially filled, or not filled at all
        const auto levelOrders = getPriceLevel(order->price_);
        if (!levelOrders){
            order->nextOrder_ = order->prevOrder_ = order;
            auto newPriceLevel = priceLevelPool.Allocate(order->side_, order->price_, order);
            addPriceLevel(newPriceLevel);
            newPriceLevel->headOrder = order;
        } else {
            // auto BestSideLevel = (order->side_ == Side::BUY) ? bids_ : asks_;
            addOrderToTail(order, levelOrders);
            // while (!priceLevelCompare(order->side_, BestSideLevel->price_, order->price_)){
                // BestSideLevel = BestSideLevel->nextPrice_;
            // }
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
        auto& bestSideLevel = (newLevel->side_ == Side::BUY) ? bids_ : asks_;
        priceLevelsMap[newLevel->price_] = newLevel;

        if (!bestSideLevel) [[unlikely]]{
            bestSideLevel = newLevel;
            newLevel->prevPrice_ = newLevel->nextPrice_ = newLevel;
            return;
        }

        auto& currentPriceLevel = bestSideLevel;
        if (priceLevelCompare(newLevel->side_, currentPriceLevel->price_, newLevel->price_)){
            insertLevelBefore(currentPriceLevel,newLevel);
            bestSideLevel = newLevel;
            return;
        }

        currentPriceLevel = currentPriceLevel->nextPrice_;
        while (currentPriceLevel != bestSideLevel && !priceLevelCompare(newLevel->side_, currentPriceLevel->price_, newLevel->price_)){
            currentPriceLevel = currentPriceLevel->nextPrice_;
        }

        if (currentPriceLevel == bestSideLevel){
            insertNewTail(bestSideLevel, newLevel);
            return;
        }

        insertLevelBefore(currentPriceLevel, newLevel);
    }

    void insertNewTail(PriceLevelOrders* bestLevel, PriceLevelOrders* newLevel) noexcept {
        auto& prev = bestLevel->prevPrice_;

        prev->nextPrice_ = newLevel;
        newLevel->nextPrice_ = bestLevel;
        newLevel->prevPrice_ = prev;
        bestLevel->prevPrice_ = newLevel;
    }

    void insertLevelBefore(PriceLevelOrders* currentLevel, PriceLevelOrders* newLevel) noexcept {
        auto& prev = currentLevel->prevPrice_;

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

    void removePriceLevel(Price price, Side side) noexcept {
        auto& bestLevel = (side == Side::BUY) ? bids_ : asks_;
        auto priceLevel = getPriceLevel(price);
        if (!priceLevel){
            std::cout << "Price level doesnt exist" << "\n";
            return;
        }

        priceLevel->nextPrice_->prevPrice_ = priceLevel->prevPrice_;
        priceLevel->prevPrice_->nextPrice_ = priceLevel->nextPrice_;

        if (priceLevel == bestLevel){
            bestLevel = priceLevel->nextPrice_;
        }

        priceLevel->nextPrice_ = priceLevel->prevPrice_ = nullptr;
        priceLevelsMap.at(priceLevel->price_) = nullptr;

        priceLevelPool.deallocate(priceLevel);
    }
};
