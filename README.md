# EmuBoy

A GameBoy emulator written in modern C++ ,with an eventual goal of cross-compiling to run bare-metal on an STM32
Nucleo microcontroller.

## Status

This project is in active, early development. Here's what's real today
versus what's planned:

**Implemented**
- [x] ROM file loading with `std::expected`-based error handling
- [x] Header parsing: cartridge destination/region code, old/new licensee
      (publisher) code, and cartridge (MBC) type
- [x] CPU register model (`Registers` struct: `A`-`L`, flags, `SP`, `PC`)
- [X] ROM size / RAM size
      
**Not implemented yet** -- in progress
- [ ] CPU instruction decode + execution (`CPU::step()` is currently
      declared but has no body)
- [ ] Memory map / MMU (translating 16-bit addresses to ROM banks, RAM,
      I/O registers, etc.)
- [ ] PPU (graphics) emulation
- [ ] Input handling
- [ ] Bare-metal STM32 Nucleo port

## Build

Requires a compiler with C++23 module support (recent Clang or MSVC; GCC
module support is still catching up).

```bash
cmake -G Ninja -B build 
ninja -C build
```

## Usage

```bash
./EmuBoy path/to/rom.gb
```

This currently prints the parsed header fields (destination code,
licensee code, cartridge type) and exits -- there's no CPU execution or
display yet.

## Known issues / next steps
CPU instruction decoding and execution is next.

## Files

| File | Purpose |
|---|---|
|`memliterals.cppm` | Literals for storage capacity |
| `readrom.cppm` | ROM loading + header parsing |
| `cpu.cppm` | CPU register model (execution not yet implemented) |
| `main.cpp` | Entry point / current smoke test |
