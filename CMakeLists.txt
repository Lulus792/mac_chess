cmake_minimum_required(VERSION 3.0)

project(Chess VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPILER)
  add_compile_options(
    -Wall
    -Wextra
    -Werror
    -Wpedantic
    -pedantic
    -pedantic-errors
  )
endif()

find_package(raylib)

if (NOT raylib_FOUND)
  message(FATAL_ERROR "Raylib not found")
endif()

set(SOURCES
  src/main.cpp
  src/chess.cpp
)

set(HEADERS
  src/chess.h
)

add_executable(Chess ${SOURCES} ${HEADERS})

target_link_libraries(Chess PRIVATE raylib)
