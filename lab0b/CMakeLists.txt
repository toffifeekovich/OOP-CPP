cmake_minimum_required(VERSION 4.0)
project(lab0b)

set(CMAKE_CXX_STANDARD 20)

add_executable(lab0b main.cpp
        Reader.cpp
        Reader.h
        Sorter.cpp
        Sorter.h
        CsvOutputer.cpp
        CsvOutputer.h
)
