#pragma once
#include "Alias.h"
#include "Side.h"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>
// where prices = index, once indexed, we get a queue

static constexpr size_t NO_LEVEL = -1;
static constexpr Price SCALING = 10000; //place holder for now, will need to determine most optimal scaling later on
static constexpr Price MAX_INDEX = std::numeric_limits<Price>::max();

struct LevelInfo {
    Quantity quantity_;
    size_t nextLevel_;
    size_t prevLevel_;
    LevelInfo() : quantity_{}, nextLevel_{NO_LEVEL}, prevLevel_{NO_LEVEL} {} //no nextLevel can be zero,
    void clear() {
        quantity_ = 0;
        nextLevel_ = NO_LEVEL;
        prevLevel_ = NO_LEVEL;
    }
};

using LevelVector = std::vector<LevelInfo>;

static size_t getPriceIndex(Price price){
    return static_cast<size_t>(price-SCALING);
}

//levelVec price will give us a levelInfo, assuming in order book we default initialize the entire vec
//we can assume that the price point index WILL exist
static void updateLevel(LevelVector& levelVec, Price price, Quantity quantity){
    size_t ind;
    if (ind = getPriceIndex(price); ind >=MAX_INDEX ) [[unlikely]] {
        return;
    }

    if (levelVec[price].quantity_ += quantity; levelVec[price].quantity_ <= 0){
        levelVec[price].clear();
    }
}

template <Side side>
class LevelData;

template <>
class LevelData<Side::BUY> {

public:
    static void editLevelData(Price price, Quantity quantity){
        updateLevel(data_, price, quantity);
    }

    static const LevelVector& getData() {
        return data_;
    }

    static const LevelInfo& getLevelInfo(Price price){
        return data_[getPriceIndex(price)]; //calling getPrice index in ordere to scale price suitable for indices
    }

    static size_t bestBidIndex() {
        return bestBidInd;
    }

private:
    static LevelVector data_;
    static size_t bestBidInd; //if we instead save the bestBid into a different struc,
    static Price bestBid_;
};

template <>
class LevelData<Side::SELL>{

public:
    static void editLevelData(Price price, Quantity quantity){
       updateLevel(data_,price,quantity);
    }
    //would this ever need to be rvalue ref?
    static const LevelVector& getData() {
        return data_;
    }

    static size_t bestAskIndex() {
        return bestAskInd;
    }


    static const LevelInfo& getLevelInfo(Price price){
        return data_[getPriceIndex(price)]; //calling getPrice index in ordere to scale price suitable for indices
    }

private:
    static LevelVector data_;
    static size_t bestAskInd;
    static Price bestAsk_;
};
