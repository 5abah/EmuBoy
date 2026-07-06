export module gameboy;
import std;
import read_rom;
import memory_literals;

export {
    enum class REG
    {
        B,
        C,
        D,
        E,
        H,
        L,
        HL,
        A,
        Count
    };
    enum class REGSP
    {
        BC,
        DE,
        HL,
        SP,
        Count
    };
    enum class REGAF
    {
        BC,
        DE,
        HL,
        AF,
        Count
    };

    // Overload the + operator to convert enum classes to size_t implicitly
    template <typename T> constexpr std::size_t operator+(T e)
    {
        return static_cast<std::size_t>(e);
    }
    enum CC // flag conditions
    {
        NZ,
        Z,
        NC,
        C
    };
    enum ALU
    {
        ADD,
        ADC,
        SUB,
        SBC,
        AND,
        XOR,
        OR,
        CP
    };
    enum ROT
    {
        RLC,
        RRC,
        RL,
        RR,
        SLA,
        SRA,
        SWAP,
        SRL
    };
}
class Registers
{
  public:
    std::array<std::uint8_t, 8> regs;    // B, C, D, E, H, L, (HL), A
    std::array<std::uint16_t, 4> regPsp; // BC,DE,HL,SP
    std::array<std::uint16_t, 4> regPaf; // BC, DE, HL, AF
    std::array<std::uint8_t, 4> flags;   // NZ, Z, NC, C
    std::bitset<8> F{};
    std::uint16_t PC{0x100};
};

class CPU
{
  public:
    Registers regs{};

    std::array<std::uint8_t, 8> alu; // refer to paper by Scott Mansel
    std::array<std::uint8_t, 8> rot; // refer to paper by Scott Mansel
};

using namespace MemLiteral;

export class GameBoy
{
  public:
    CPU cpu;
    Cartridge rom;
    std::array<std::uint_least32_t, 1_MiB> memory;
};

export {
    std::uint8_t getHi(std::uint16_t data)
    {
        return data >> 8;
    }
    std::uint8_t getLo(std::uint16_t data)
    {
        return data & 0xFF;
    }
}
