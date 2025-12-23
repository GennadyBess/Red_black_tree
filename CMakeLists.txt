cmake_minimum_required(VERSION 3.20)
project(RBTreeProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch GoogleTest automatically
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# Main executable from src
add_executable(main src/main.cpp src/rb_tree.hpp src/rb_node.hpp src/buffer.hpp)
target_include_directories(main PRIVATE src)

# Test executable from test (links GoogleTest)
add_executable(test_tree test/test_tree.cpp src/rb_tree.hpp src/rb_node.hpp src/buffer.hpp)
target_include_directories(test_tree PRIVATE src)
target_link_libraries(test_tree GTest::gtest GTest::gtest_main)

# Enable CTest for running tests
include(GoogleTest)
gtest_discover_tests(test_tree)

# MinGW-specific settings
if(MINGW)
    target_compile_options(main PRIVATE -Wall -Wextra)
    target_compile_options(test_tree PRIVATE -Wall -Wextra)
endif()

# MSVC settings
if(MSVC)
    target_compile_options(main PRIVATE /W4 /utf-8)
    target_compile_options(test_tree PRIVATE /W4 /utf-8)
endif()
if(MINGW)
    target_compile_options(main PRIVATE -Wall -Wextra)
    # UTF-8 вывод для Windows
    target_compile_definitions(main PRIVATE _CRT_SECURE_NO_WARNINGS)
    
    # UTF-8 консоль при запуск
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Setting UTF-8 console..."
        COMMENT "UTF-8 console setup"
    )
endif()
