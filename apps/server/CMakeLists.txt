# Chat++/apps/server/CMakeLists.txt

# Define the executable target
add_executable(server
        src/main.cpp
)

# Link against dependencies
target_link_libraries(server
        PRIVATE
        common_lib
        json_struct
)