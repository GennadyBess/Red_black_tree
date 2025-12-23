cmake_minimum_required(VERSION 3.20)
project(RBTreeProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# Main executable
add_executable(main src/main.cpp src/rb_tree.hpp src/rb_node.hpp src/buffer.hpp)
target_include_directories(main PRIVATE src)

# Test executable
add_executable(test_tree test/test_tree.cpp src/rb_tree.hpp src/rb_node.hpp src/buffer.hpp)
target_include_directories(test_tree PRIVATE src)
target_link_libraries(test_tree GTest::gtest GTest::gtest_main)

add_test(NAME RBTreeTests COMMAND test_tree)

# MinGW settings
if(MINGW)
    target_compile_options(main PRIVATE -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8)
    target_compile_definitions(main PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# MSVC settings
if(MSVC)
    target_compile_options(main PRIVATE /W4 /utf-8)
endif()
