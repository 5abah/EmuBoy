
cmake_minimum_required(VERSION 4.3)

# 1. CRITICAL: The experimental gate MUST be set BEFORE the project() call.
# This allows the toolchain detection sequence to find 'import std' dependencies.
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")

project(GameBoyEmu CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # Ensure some code quality is taken care of.
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 2. Declare the target executable first
add_executable(GameBoyEmu main.cpp)

# 3. Explicitly tell CMake this target imports the standard library module
set_target_properties(GameBoyEmu PROPERTIES CXX_MODULE_STD ON)

# 4. Attach your custom modules to the executable
target_sources(GameBoyEmu
  PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
      cpu.cppm
      readrom.cppm
)

