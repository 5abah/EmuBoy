cmake_minimum_required(VERSION 4.3)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
    "451f2fe2-a8a2-47c3-bc32-94786d8fc91b"
)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

project(GameBoyEmu
    VERSION 0.1.0
    LANGUAGES CXX
)

add_executable(GameBoyEmu)

target_sources(GameBoyEmu
    PRIVATE
        core/platform/main.cpp

    PRIVATE
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS
            core
        FILES
            core/apu.cppm
            core/bus.cppm
            core/cartridge.cppm
            core/cpu.cppm
            core/cpu_opcodes.cppm
            core/gameboy.cppm
            core/interrupts.cppm
            core/joypad.cppm
            core/memliterals.cppm
            core/ppu.cppm
            core/timer.cppm
            core/platform/mbc.cppm
)

target_compile_features(GameBoyEmu PRIVATE cxx_std_23)

set_target_properties(GameBoyEmu PROPERTIES
    CXX_MODULE_STD ON
    CXX_EXTENSIONS OFF
)

target_compile_options(GameBoyEmu PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
