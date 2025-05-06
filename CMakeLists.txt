cmake_minimum_required(VERSION 3.14)
project(ct_string LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable modules support
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "enabled")

# Create actual target for the module interface
add_library(ct_string)
target_sources(ct_string
  PRIVATE
    include/ct_string/ct_string.ixx
)

target_compile_features(ct_string PRIVATE cxx_std_20)

# Required for import to work in tests
target_include_directories(ct_string
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Optional install rules
include(GNUInstallDirs)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Tests
enable_testing()
add_subdirectory(test)
