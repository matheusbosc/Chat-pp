# Chat++/apps/client/CMakeLists.txt

find_package(ftxui REQUIRED)
# Define the executable target
add_executable(client
        src/client/main.cpp
        src/client/client.cpp
        src/client/ui.cpp
)

# Include directories for headers
target_include_directories(client
        PRIVATE
        src/client
)

# Link against dependencies
target_link_libraries(client
        PRIVATE
        common_lib
        json_struct
        nlohmann

        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)