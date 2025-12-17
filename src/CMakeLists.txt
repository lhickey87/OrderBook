add_library(orderbook_lib
    Engine.cpp
    Orderbook.cpp
    DataFeed.cpp
)

add_executable(orderbook
    main.cpp
)

target_link_libraries(orderbook PRIVATE orderbook_lib)

target_compile_options(orderbook_lib PRIVATE
    -Wall -Wextra -Werror -Wpedantic
)

target_compile_options(orderbook PRIVATE
    -Wall -Wextra -Werror -Wpedantic
)
