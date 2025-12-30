
option(USE_BENCHMARK "Enable high-precision benchmarking" OFF)

add_library(orderbook_lib
    Engine.cpp
    Orderbook.cpp
    DataFeed.cpp
    Logger.cpp
)

add_executable(orderbook
    main.cpp
)

target_link_libraries(orderbook PRIVATE orderbook_lib)

if(USE_BENCHMARK)
    # This defines BENCHMARK as 1
    target_compile_definitions(orderbook_lib PUBLIC BENCHMARK=1)
    message(STATUS "Build Mode: BENCHMARKING (Logs to Benchmarks.log)")
else()
    # This defines BENCHMARK as 0
    target_compile_definitions(orderbook_lib PUBLIC BENCHMARK=0)
    message(STATUS "Build Mode: PRODUCTION (Logs to Orderbook.log)")
endif()


target_compile_options(orderbook_lib PRIVATE
    -O3
    -mcpu=native
    -fno-omit-frame-pointer
)

target_compile_options(orderbook PRIVATE
    -O3
    -mcpu=native
    -fno-omit-frame-pointer
)
