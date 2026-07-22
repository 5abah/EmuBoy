cmake_minimum_required(VERSION 4.3)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
    "451f2fe2-a8a2-47c3-bc32-94786d8fc91b"
)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

project(GameBoyEmu
    VERSION 0.1.0
    LANGUAGES CXX
)

add_executable(GameBoyEmu)


target_sources(GameBoyEmu
    PRIVATE
        core/platform/main.cpp
        core/cpu_opcodes.cpp
    PRIVATE
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS  
            "${CMAKE_CURRENT_SOURCE_DIR}/core"  # This fixes the lookup failure
        FILES
            core/apu.cppm
            core/bus.cppm
            core/cartridge.cppm
            core/cpu.cppm
            core/gameboy.cppm
            core/interrupts.cppm
            core/joypad.cppm
            core/memliterals.cppm
            core/ppu.cppm
            core/timer.cppm
            core/mbc/mbc.cppm

)


set_source_files_properties(
    core/apu.cppm
    core/bus.cppm
    core/cartridge.cppm
    core/cpu.cppm
    core/gameboy.cppm
    core/interrupts.cppm
    core/joypad.cppm
    core/memliterals.cppm
    core/ppu.cppm
    core/timer.cppm
    core/mbc/mbc.cppm
    PROPERTIES LANGUAGE CXX
)

target_compile_features(GameBoyEmu PRIVATE cxx_std_23)

set_target_properties(GameBoyEmu PROPERTIES
    CXX_MODULE_STD ON
    CXX_EXTENSIONS OFF
)

target_compile_options(GameBoyEmu PRIVATE
    -stdlib=libc++
    -Wall
    -Wextra
    -Wpedantic
)

target_link_options(GameBoyEmu PRIVATE
    -stdlib=libc++
)
