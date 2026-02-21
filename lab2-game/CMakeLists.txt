cmake_minimum_required(VERSION 3.16)
project(BomberMVC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML location
set(SFML_ROOT "C:/SFML-2.6.1")
set(SFML_DIR  "${SFML_ROOT}/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS system window graphics audio REQUIRED)

add_executable(BomberMVC
    src/main.cpp
    src/GameController.cpp
    src/GameView.cpp
    src/GameModel.cpp
    src/Map.cpp
    src/Objects.cpp
)

# Include dirs (ВАЖНО, иначе <SFML/Graphics.hpp> не найдётся)
target_include_directories(BomberMVC PRIVATE
    src
    "${SFML_ROOT}/include"
)

# Library dirs (для MinGW бывает нужно явно)
target_link_directories(BomberMVC PRIVATE
    "${SFML_ROOT}/lib"
)

# Link SFML modules (это именно то, что нужно твоей сборке)
target_link_libraries(BomberMVC PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Copy DLLs next to exe
if (WIN32)
    add_custom_command(TARGET BomberMVC POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_ROOT}/bin/sfml-graphics-2.dll"
            "${SFML_ROOT}/bin/sfml-window-2.dll"
            "${SFML_ROOT}/bin/sfml-system-2.dll"
            "${SFML_ROOT}/bin/sfml-audio-2.dll"
            "${SFML_ROOT}/bin/openal32.dll"
            "$<TARGET_FILE_DIR:BomberMVC>"
    )
endif()
