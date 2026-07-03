export module gameboy;
import std;
import read_rom;

export {
    enum class REG // 8 bit registers
    {
        B,
        C,
        D,
        E,
        H,
        L,
        HL,
        A
    };
    enum class REGSP // register pair along with SP
    {
        BC,
        DE,
        HL,
        SP
    };
    enum class REGAF // register pair along with AF
    {
        BC,
        DE,
        HL,
        AF
    };
    enum class CC // flag conditions
    {
        NZ,
        Z,
        NC,
        C
    };
    enum class ALU
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
    enum class ROT
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
