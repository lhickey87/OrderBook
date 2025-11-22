#include "../include/Orderbook.h"


void Orderbook::add(OrderId orderId, Side side, Price price, Quantity quantity){
    //in must allocate an orderFirst
    auto newOrder = orderPool.Allocate(orderId,side,price,quantity);

}

void Orderbook::deleteOrder(OrderId orderId){}



void Orderbook::recordAggressiveOrder(OrderId orderId, Price price, Quantity quantity){

}

void Orderbook::fillPassiveOrder(OrderId orderId, Quantity quantity, bool withPrice){}


void Orderbook::reduceOrder(OrderId orderId, Quantity cancelled){}

void Orderbook::modifyOrder(OrderId oldOrderId, OrderId newOrderId, Price newPrice, Quantity quantity){}
