export module gameboy;
import std;
import read_rom;

class Registers
{
  public:
    std::array<std::uint8_t, 8> regs;    // B, C, D, E, H, L, (HL), A
    std::array<std::uint16_t, 4> regPsp; // BC,DE,HL,SP
    std::array<std::uint16_t, 4> regPaf; // BC, DE, HL, AF
    std::array<std::uint8_t, 4> flags;   // NZ, Z, NC, C
    std::bitset<8> F{};

    std::uint16_t get16Bit(std::uint8_t &high, std::uint8_t &low)
    {
        return (high << 8) | low;
    };
};

class CPU
{
  public:
    Registers regs{};

    std::array<std::uint8_t, 8> alu; // refer to paper by Scott Mansel
    std::array<std::uint8_t, 8> rot; // refer to paper by Scott Mansel
};

export class GameBoy
{
    CPU cpu;
    Cartridge rom;
};
