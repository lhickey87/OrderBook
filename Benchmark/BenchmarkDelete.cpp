#include <benchmark/benchmark.h>
#include "../include/Orderbook.h"

static void BM_DeleteOrder(benchmark::State& state) {
    const size_t order_count = state.range(0);
    Orderbook book;
    std::vector<uint64_t> ids;
    ids.reserve(order_count);

    for (size_t i = 0; i < order_count; ++i) {
        uint64_t id = 1000 + i;
        book.add(id, Side::BUY, 10, 2445000);
        ids.push_back(id);
    }

    std::reverse(ids.begin(), ids.end());

    size_t i = 0;
    for (auto _ : state) {
        uint64_t target_id = ids[i];

        book.deleteOrder(target_id);

        benchmark::DoNotOptimize(book);
        benchmark::ClobberMemory();

        state.PauseTiming();
        book.add(target_id, Side::BUY, 10, 2445000);

        if (++i == order_count) [[unlikely]] {
            i = 0;
        }
        state.ResumeTiming();
    }

    state.SetItemsProcessed(state.iterations());
}

BENCHMARK(BM_DeleteOrder)
    ->Args({1024})    // Fits in L1/L2 cache
    ->Args({16384})   // Likely L3 cache
    ->Args({131072})  // Likely Main Memory (RAM)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();
