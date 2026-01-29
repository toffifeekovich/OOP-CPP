cmake_minimum_required(VERSION 4.0)
project(lab0b)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab0b src/main.cpp
        src/Reader.cpp
        src/Reader.h
        src/Sorter.cpp
        src/Sorter.h
        src/CsvOutputer.cpp
        src/CsvOutputer.h
)
