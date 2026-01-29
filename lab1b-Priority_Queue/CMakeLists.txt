cmake_minimum_required(VERSION 4.0)

project(lab1b LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Основной exe с демкой ---

add_executable(lab1b
    src/main.cpp
    src/PriorityQueue.cpp
    src/PriorityQueue.hpp  # просто чтобы IDE его видела
)

# --- Google Test + юнит-тесты ---

enable_testing()

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# Для Windows/MSVC, но не мешает
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Нам нужен только gtest, без gmock (иначе MinGW ругается)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON  CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(tests
    tests/PriorityQueueTests.cpp
    src/PriorityQueue.cpp
    src/PriorityQueue.hpp
)

target_link_libraries(tests
    PRIVATE
        gtest
        gtest_main
)

# Тестам нужен доступ к заголовку в src/
target_include_directories(tests PRIVATE src)

add_test(NAME PriorityQueueTests COMMAND tests)
