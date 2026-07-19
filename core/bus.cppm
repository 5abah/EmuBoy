// bus
export module bus;
import std;
import memory_literals;
import ppu;
import apu;
import timer;
import joypad;
import cartridge;
using namespace MemLiteral;

export class Bus
{
    std::array<std::uint8_t, 64_KiB> addrSpace{};

  public:
    PPU ppu;
    APU apu;
    Timer timer;
    Joypad joypad;
    Cartridge cartridge;
    Interrupts interrupts;
    const std::uint8_t &read(std::uint16_t pcIndex) const;
    void write(std::uint16_t pcIndex, std::uint8_t byte2Write);
};

const std::uint8_t &Bus::read(std::uint16_t pcIndex) const
{
    return addrSpace[pcIndex];
}

void Bus::write(std::uint16_t pcIndex, std::uint8_t byte2Write)
{
    addrSpace[pcIndex] = byte2Write;
}
