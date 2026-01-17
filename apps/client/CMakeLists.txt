# Chat++/apps/client/CMakeLists.txt

# Define the executable target
add_executable(client
        src/main.cpp
)

# Link against dependencies
target_link_libraries(client
        PRIVATE
        common_lib
        json_struct
)