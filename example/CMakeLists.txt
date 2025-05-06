cmake_minimum_required(VERSION 3.26)

project(example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable modules if using a supporting compiler
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC's module support is partial and nonstandard as of 13.x
    message(WARNING "GCC modules support is experimental")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # No need for flags; MSVC handles /interface and /module automatically
endif()

# Add sources and modules
add_executable(example
    main.cpp
    ../include/ct_string/ct_string.ixx
)