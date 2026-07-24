# EmuBoy

A GameBoy emulator written in modern C++, with an eventual goal of cross-compiling to run bare-metal on an STM32
Nucleo microcontroller.

## Status

This project is in active, early development. Here's what's real today
versus what's planned:

**Implemented**
- [x] CPU register model (`Registers` struct: 8-bit registers indexed via
      `REG8`, register pairs via `RP`, flag bits via a `std::bitset<4>`,
      `PC`, ALU operation tags)
- [x] `CPU::step()` opcode dispatch: the unprefixed opcode table decode
      (`x`/`y`/`z`/`p`/`q` extraction) is wired for most of the table --
      8-bit/16-bit loads (including `(HL+)`/`(HL-)` forms), 8-bit/16-bit
      ALU ops, `INC`/`DEC` (register and register-pair), unconditional and
      conditional `JP`/`JR`, conditional `CALL`/`RET`, `RST`, `PUSH`/`POP`,
      `DI`/`EI`, `HALT` (returns without executing further, no wake logic
      yet), and the accumulator-targeted rotate/shift/`DAA`/`CPL`/`SCF`/`CCF`
      block
- [x] CB-prefixed opcode functions implemented and unit-tested directly
      (rotate/shift/swap/`BIT`/`SET`/`RES`), though CB dispatch through
      `step()` itself isn't wired in yet (see Known issues)
- [x] `Bus` with a flat 64 KiB address space and owning instances of
      `PPU`, `Timer`, `Joypad`, and `Interrupts` (IME/IE/IF)
- [x] Unit tests (`cpu_opcode_test.cpp`) covering the above via both direct
      function calls and full `step()` round-trips, including CPU-driven
      `RET cc`/`JP cc`/`CALL cc`/`RST`/rotate instructions

**Not implemented yet** -- in progress
- [ ] Cartridge/ROM loading and header parsing (`cartridge.cppm` is
      currently just an empty class stub; a memory bank controller module
      is planned at `core/mbc/mbc.cppm` but doesn't exist yet)
- [ ] `PPU`, `APU`, `Timer`, and `Joypad` are all empty stub classes with no
      behavior yet -- `apu.cppm` isn't even wired into `Bus` currently
      (commented out), and `APU`/`Cartridge` aren't marked `export`, so
      they can't be used outside their own translation units yet
- [ ] `GameBoy::step()` (the top-level frontend tying `Bus` and `CPU`
      together) is declared but has no implementation
- [ ] A handful of remaining unprefixed opcodes in `CPU::step()`:
      `LD (BC/DE),A` and their reverse, `LDH (n),A` / `LDH A,(n)`,
      `LD (C),A` / `LD A,(C)`, `LD (nn),A` / `LD A,(nn)`, `ADD SP,e`,
      unconditional `RET`/`JP`/`CALL`, and the immediate-operand ALU form
      (`ADD A,n8` etc.)
- [ ] CB-prefixed dispatch inside `step()` (the underlying CB opcode
      functions exist and are tested standalone, but nothing routes the
      `0xCB` prefix byte to them yet)
- [ ] Input handling
- [ ] Bare-metal STM32 Nucleo port

## Build

Requires a compiler with C++23 module support (recent Clang with libc++;
GCC module support is still catching up). CMake 4.3+ is required for the
experimental `import std` support this project relies on.

```bash
cmake -G Ninja -B build
ninja -C build
```

This produces three targets:
- `GameBoyCore` -- static library containing all emulator modules
- `GameBoyEmu` -- the executable frontend (`core/platform/main.cpp`)
- `GameBoyTests` -- the CPU opcode test suite (`core/tests/cpu_opcode_test.cpp`)

## Usage

```bash
./GameBoyEmu path/to/rom.gb
```

There's no cartridge loading, display, or input yet -- the executable
currently only exists as a build target linking against `GameBoyCore`.

Run the opcode test suite with:

```bash
./GameBoyTests
```

## Known issues / next steps

- `cartridge.cppm` is currently an empty stub (`class Cartridge { // MBC
  mbc; };`) with no `export`, no ROM-loading logic, and no header parsing
  -- this needs to be rebuilt before any real ROM can run.
- `Bus` has `PPU`, `Timer`, `Joypad`, and `Interrupts` wired in as members,
  but `APU` and `Cartridge` are currently commented out; both `apu.cppm`
  and `cartridge.cppm` also declare their classes without `export`, so
  they'd fail to compile as written the moment something outside their own
  translation unit tries to use them.
- `accumulatorRegisterArithmetic` doesn't yet special-case
  `regIndex == 6` (the `(HL)` memory operand) the way `ldRegToReg` does --
  ALU opcodes that route through `(HL)` will currently read the wrong
  operand.
- The CB-prefixed rotate/shift functions use two different calling
  conventions internally (`rotateLeftCB`/`rotateRightCB` re-fetch the CB
  opcode byte themselves; every other CB function takes the decoded
  register index directly) -- this needs to be unified before CB dispatch
  can be wired into `step()`.
- No M-cycle-accurate timing yet -- opcode functions return a total cycle
  count per instruction (atomic execution), which is sufficient for
  Blargg's `cpu_instrs`/`instr_timing` test ROMs but not yet for
  Mooneye-GB's timing-sensitive acceptance tests; that'll likely require a
  refactor once PPU/timer integration starts.
- `GameBoy::step()` has no body yet, so there's no single entry point
  driving `CPU::step()` against a live `Bus` outside of the unit tests.

## Files

| File | Purpose |
|---|---|
| `core/cpu.cppm` | CPU register model + full opcode function declarations |
| `core/cpu_opcodes.cpp` | CPU opcode function implementations |
| `core/bus.cppm` | Flat 64 KiB address space; owns PPU/Timer/Joypad/Interrupts |
| `core/cartridge.cppm` | Cartridge/MBC model (currently an empty stub) |
| `core/gameboy.cppm` | Top-level frontend tying `Bus` and `CPU` together (unimplemented) |
| `core/interrupts.cppm` | IME/IE/IF interrupt state |
| `core/ppu.cppm`, `core/ppu.cpp` | Graphics (stub, no behavior yet) |
| `core/apu.cppm`, `core/apu.cpp` | Audio (stub, no behavior yet) |
| `core/timer.cppm` | Timer registers (stub, no behavior yet) |
| `core/joypad.cppm` | Input (stub, no behavior yet) |
| `core/memliterals.cppm` | `_KiB`/`_MiB` literal helpers |
| `core/mbc/mbc.cppm` | Memory bank controller (referenced by CMake, not yet added) |
| `core/platform/main.cpp` | Entry point (referenced by CMake, not yet added) |
| `core/tests/cpu_opcode_test.cpp` | CPU opcode unit tests |
