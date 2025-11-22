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
    addOrder(newOrder);
}

void Orderbook::deleteOrder(OrderId orderId){}



void Orderbook::recordAggressiveOrder(OrderId orderId, Price price, Quantity quantity){

}

void Orderbook::fillPassiveOrder(OrderId orderId, Quantity quantity, bool withPrice){}


void Orderbook::reduceOrder(OrderId orderId, Quantity cancelled){}

void Orderbook::modifyOrder(OrderId oldOrderId, OrderId newOrderId, Price newPrice, Quantity quantity){}
