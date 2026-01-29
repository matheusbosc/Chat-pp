# ChatPP/libs/nlohmann_json/CMakeLists.txt

add_library(nlohmann INTERFACE)

# Public include directory (so dependents can #include <nlohmann_json/json_struct.h>)
target_include_directories(nlohmann
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Ensure the library uses the global C++ standard
target_compile_features(nlohmann INTERFACE cxx_std_26)

