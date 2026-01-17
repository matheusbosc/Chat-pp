cmake_minimum_required(VERSION 4.1)
project(
        ChatPP
        VERSION 1.0
        DESCRIPTION "A terminal based networked chat application."
        LANGUAGES CXX
)

# Set C++ standard (e.g., C++26)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable compiler-specific extensions

# Set output directories for all targets
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)  # Executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)  # Shared libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)  # Static libraries

# Add libraries first (math_lib and utils_lib)
add_subdirectory(libs)

# Then add executables (calculator_app)
add_subdirectory(apps)

# Finally, add tests (if enabled)
#include(CTest)  # Enables testing support
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()