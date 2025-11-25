


add_library(orderbook_lib STATIC
    DataFeed.cpp
    Engine.cpp
    Orderbook.cpp
)

target_include_directories(orderbook_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(orderbook main.cpp)


target_link_libraries(orderbook PRIVATE orderbook_lib)
