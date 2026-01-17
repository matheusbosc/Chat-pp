# calculator_project/libs/common_lib/CMakeLists.txt

# Define the library target (static by default; use SHARED for dynamic)
add_library(common_lib STATIC
        src/common_lib.cpp  # Private source
)

# Public include directory (so dependents can #include <common_lib/common_lib.h>)
target_include_directories(common_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  # For build tree
        $<INSTALL_INTERFACE:include>  # For install tree (if needed)
)

# Ensure the library uses the global C++ standard
target_compile_features(common_lib PUBLIC cxx_std_17)

