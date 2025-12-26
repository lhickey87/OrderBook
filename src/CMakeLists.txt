
option(ENABLE_BENCHMARK "Enable benchmarking code" OFF)

add_library(orderbook_lib
    Engine.cpp
    Orderbook.cpp
    DataFeed.cpp
)

add_executable(orderbook
    main.cpp
)

target_link_libraries(orderbook PRIVATE orderbook_lib)


if (ENABLE_BENCHMARK)
    target_compile_definitions(orderbook_lib PRIVATE BENCHMARK=1)
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
