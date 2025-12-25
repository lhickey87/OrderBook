#include <benchmark/benchmark.h>
#include "../include/Orderbook.h"

static void BM_ADD(benchmark::State& state){
    const int count = state.range(0);

    Orderbook book;
    for (int i = 0; i < count; ++i) {
        auto side = ((i & 1) == 0) ? Side::BUY : Side::SELL;
        book.add(1000 + i, side, 10, 24450);
    }

    std::vector<Price> prices(count);
    for (int i = 0; i < count; ++i){
        prices[i] = 2440000;
    }

    uint64_t id = 2000;
    size_t idx = 0;

    for (auto _ : state) {
        int price = prices[idx++ % prices.size()];
        auto side = ((idx & 1) == 0)? Side::BUY : Side::SELL;
        book.add(id, side, 10, price);
        benchmark::ClobberMemory();

        state.PauseTiming();
        book.deleteOrder(id);  // keep size constant
        ++id;
        state.ResumeTiming();
    }
}

BENCHMARK(BM_ADD)->Args({1000});
BENCHMARK_MAIN();
