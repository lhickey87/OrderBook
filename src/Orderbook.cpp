#include "../include/Orderbook.h"
#include <stdexcept>

void Orderbook::add(OrderId orderId, Side side, Price price, Quantity quantity, ClientId client){
    //in must allocate an orderFirst
    Order* newOrder = orderPool.Allocate(orderId,price,quantity,client, side);

    if (nextOrderId != newOrder->orderId_){
        //error here, maybe try to ASSERT so we don't deal with this at run time
    }
    orderMap[newOrder->orderId_] = newOrder;
    ++nextOrderId;
    //logger_->log("{} {}",__FILE__, __LINE__,__FUNCTION__,getCurrentTimeStr());
    addOrder(newOrder);
}

void Orderbook::deleteOrder(OrderId orderId){
    auto order = getOrder(orderId);
    if (!order){throw std::out_of_range("Order not in map");}
    if (order->prevOrder_ == order){
        removePriceLevel(order->price_, order->side_);
        return;
    }
    const auto prevOrder = order->prevOrder_;
    const auto nextOrder = order->nextOrder_;
    prevOrder->nextOrder_ = nextOrder;
    nextOrder->prevOrder_ = prevOrder;

    //orderMap[orderId] = nullptr;
    orderPool.deallocate(order);
}

void Orderbook::executeOrderAtPrice(OrderId orderId, Quantity quantity){
    auto order = getOrder(orderId);
    if (!order){
        throw std::out_of_range("Not in map");
    }
    //logger->log("order: {} executed at price:{} with quantity: {}");
    order->Fill(quantity);
}

void Orderbook::executeOrder(OrderId orderId, Quantity quantity){
    auto order = getOrder(orderId);
    if (!order){throw std::out_of_range("not in map");}
    order->Fill(quantity);
}

void Orderbook::reduceOrder(OrderId orderId, Quantity cancelled){
    auto order = getOrder(orderId);
    if (!order){throw std::out_of_range("not in map");}
    order->Fill(cancelled);
    if (order->isFilled){
        deleteOrder(orderId);
    }
}

void Orderbook::modifyOrder(OrderId oldOrderId, OrderId newOrderId, Price newPrice, Quantity quantity){
    auto oldOrder = getOrder(oldOrderId);
    if (!oldOrder){throw std::out_of_range("not in map");}
    Side side = oldOrder->side_;
    deleteOrder(oldOrderId);
    oldOrder = nullptr;
    add(newOrderId,side,newPrice,quantity);
}
