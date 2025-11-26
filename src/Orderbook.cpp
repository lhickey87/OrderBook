#include "../include/Orderbook.h"

void Orderbook::add(OrderId orderId, Side side, Price price, Quantity quantity){
    //in must allocate an orderFirst
    Order* newOrder = orderPool.Allocate(orderId,side,price,quantity);
    //once order is Added, now we have to place it into the orderMap, given Orders are sequential we can simply place it at the next index
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
    auto priceLevel = getPriceLevel(order->price_);
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

void Orderbook::recordAggressiveOrder(OrderId orderId, Price price, Quantity quantity){
    //not sure if needed, need to understand message sequencing more,
}

void Orderbook::fillPassiveOrder(OrderId orderId, Quantity quantity, bool withPrice){
    auto order = getOrder(orderId);
    order->Fill(quantity);
}

void Orderbook::reduceOrder(OrderId orderId, Quantity cancelled){
    auto order = getOrder(orderId);
    order->Fill(cancelled);
    if (order->isFilled){
        deleteOrder(orderId);
    }
}

void Orderbook::modifyOrder(OrderId oldOrderId, OrderId newOrderId, Price newPrice, Quantity quantity){
    auto oldOrder = getOrder(oldOrderId);
    Side side = oldOrder->side_;
    deleteOrder(oldOrderId);
    oldOrder = nullptr;
    add(newOrderId,side,newPrice,quantity);
}
