
cmake_minimum_required(VERSION 4.3)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")

project(GameBoyEmu CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(GameBoyEmu main.cpp)

set_target_properties(GameBoyEmu PROPERTIES CXX_MODULE_STD ON)

target_sources(GameBoyEmu
  PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
      gameboy.cppm
      readrom.cppm
      memliterals.cppm
      opcode.cppm
)

