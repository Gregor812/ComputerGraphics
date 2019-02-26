cmake_minimum_required(VERSION 3.5)

project(ComputerGraphics
    LANGUAGES ASM C CXX
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -g3 -pg -O0")

include_directories(
    ${CMAKE_SOURCE_DIR}/TgaImage
    ${CMAKE_SOURCE_DIR}/Primitives
    ${CMAKE_SOURCE_DIR}/Models
)

add_library(
    TgaImage
    ${CMAKE_SOURCE_DIR}/TgaImage/TgaImage.cpp
)

set_property(TARGET TgaImage PROPERTY CXX_STANDARD 14)

add_library(
    Models
    ${CMAKE_SOURCE_DIR}/Models/Model.cpp
)

set_property(TARGET Models PROPERTY CXX_STANDARD 14)

add_executable(
    App
    ${CMAKE_SOURCE_DIR}/main.cpp
)

set_property(TARGET App PROPERTY CXX_STANDARD 14)

target_link_libraries(
    App
    TgaImage
    Models
)
