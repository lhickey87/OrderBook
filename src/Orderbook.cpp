#include "../include/Orderbook.h"

void Orderbook::add(OrderId orderId, Side side, Price price, Quantity quantity) noexcept {
    Order* newOrder = orderPool.Allocate(orderId,price,quantity, side);
    orderMap[newOrder->orderId_] = newOrder;
    addOrder(newOrder);
}

void Orderbook::deleteOrder(OrderId orderId) noexcept {
    auto order = getOrder(orderId);
    if (!order)[[unlikely]]{ std::cout << "returning" << std::endl; return;}

    if (order->prevOrder_ == order){
        removePriceLevel(order->price_, order->side_);
        return;
    }
    const auto prevOrder = order->prevOrder_;
    const auto nextOrder = order->nextOrder_;
    prevOrder->nextOrder_ = nextOrder;
    nextOrder->prevOrder_ = prevOrder;

    orderMap.erase(orderId);
    orderPool.deallocate(order);
}

void Orderbook::executeOrderAtPrice(OrderId orderId, Quantity quantity) noexcept {
    auto order = getOrder(orderId);

    if (!order)[[unlikely]]{ std::cout << "returning" << std::endl; return;}
    order->Fill(quantity);
}

void Orderbook::executeOrder(OrderId orderId, Quantity quantity) noexcept {
    auto order = getOrder(orderId);

    if (!order)[[unlikely]]{ std::cout << "returning" << std::endl; return;}

    order->Fill(quantity);
}

void Orderbook::reduceOrder(OrderId orderId, Quantity cancelled) noexcept{
    auto order = getOrder(orderId);

    if (!order)[[unlikely]]{ std::cout << "returning" << std::endl; return;}


    order->Fill(cancelled);
}

void Orderbook::modifyOrder(OrderId oldOrderId, OrderId newOrderId, Price newPrice, Quantity quantity) noexcept {
    auto oldOrder = getOrder(oldOrderId);

    if (!oldOrder)[[unlikely]]{ std::cout << "returning" << std::endl; return;}

    Side side = oldOrder->side_;
    deleteOrder(oldOrderId);
    add(newOrderId,side,newPrice,quantity);
}
