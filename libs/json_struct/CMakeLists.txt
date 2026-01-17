# ChatPP/libs/json_struct/CMakeLists.txt

add_library(json_struct INTERFACE)

# Public include directory (so dependents can #include <json_struct/json_struct.h>)
target_include_directories(json_struct
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Ensure the library uses the global C++ standard
target_compile_features(json_struct INTERFACE cxx_std_26)

