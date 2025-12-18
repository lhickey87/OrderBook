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
    -fsanitize=address
    -march=native
    -fno-omit-frame-pointer
)

# Executable: compile AND link with ASan
target_compile_options(orderbook PRIVATE
    -O3 -g
    -fsanitize=address
    -march=native
    -fno-omit-frame-pointer
)

target_link_options(orderbook PRIVATE
    -fsanitize=address
)

#[[
target_compile_options(orderbook_lib PRIVATE
    -O2 -g
    -fno-omit-frame-pointer
    -march=native
    -Wall -Wextra -Wpedantic -Werror
)
]]

#[[
target_compile_options(orderbook PRIVATE
    -O2 -g
    -fno-omit-frame-pointer
    -march=native
    -Wall -Wextra -Wpedantic -Werror
)
]]
