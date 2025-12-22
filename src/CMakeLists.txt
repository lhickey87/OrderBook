add_library(orderbook_lib
    Engine.cpp
    Orderbook.cpp
    DataFeed.cpp
)

add_executable(orderbook
    main.cpp
)

target_link_libraries(orderbook PRIVATE orderbook_lib)

# Library: compile with ASan
target_compile_options(orderbook_lib PRIVATE
    -O3 -g
    -march=native
    -fno-omit-frame-pointer
)

# Executable: compile AND link with ASan
target_compile_options(orderbook PRIVATE
    -O3 -g
    -march=native
    -fno-omit-frame-pointer
)

target_link_options(orderbook PRIVATE
    -fsanitize=address
)
