# Chat++/apps/client/CMakeLists.txt

find_package(ftxui REQUIRED)
# Define the executable target
add_executable(client
        src/main.cpp
)

# Link against dependencies
target_link_libraries(client
        PRIVATE
        common_lib
        json_struct
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)

# INSTALL FTXUI with yay -S ftxui