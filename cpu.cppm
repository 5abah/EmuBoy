export module cpu;
import std;

export {
    class Registers
    {
      public:
        std::uint8_t A, B, C, D, E, H, L;
        std::bitset<8> F{};
        std::uint16_t SP{}, PC{};

        std::uint16_t get16Bit(std::uint8_t &high, std::uint8_t &low)
        {
            return (high << 8) | low;
        };
    };

    class CPU
    {
      public:
        Registers regs{};
        void step();
    };
}
