// bus
export module bus;
import std;
import memliterals;
import ppu;
import apu;
import timer;
import joypad;
import cartridge;
import interrupts;
using namespace MemLiteral;

export class Bus
{
    std::array<std::uint8_t, 64_KiB> addrSpace{};

  public:
    PPU ppu;
    // APU apu;
    Timer timer;
    Joypad joypad;
    // Cartridge cartridge;
    Interrupts interrupts;
    const std::uint8_t &read(std::uint16_t pcIndex) const;
    void write(std::uint16_t pcIndex, std::uint8_t byte2Write);
    std::uint8_t handleInterrupts();
};

const std::uint8_t &Bus::read(std::uint16_t pcIndex) const
{
    return addrSpace[pcIndex];
}

void Bus::write(std::uint16_t pcIndex, std::uint8_t byte2Write)
{
    addrSpace[pcIndex] = byte2Write;
}

std::uint8_t Bus::handleInterrupts()
{

    if (!interrupts.IME)
    {
        return 5;
    }

    interrupts.pending = interrupts.IE.to_ulong() & interrupts.IF.to_ulong();

    if (interrupts.pending == 0)
        return 5;

    if (interrupts.pending & 0x1)
    {
        interrupts.IF.reset(interrupts.InterruptType::VBlank);
        interrupts.IME = !interrupts.IME;
    }
    else if (interrupts.pending & 0x2)
    {
        interrupts.IF.reset(interrupts.InterruptType::LCDStat);
        interrupts.IME = !interrupts.IME;
    }
    else if (interrupts.pending & 0x4)
    {
        interrupts.IF.reset(interrupts.InterruptType::Timer);
        interrupts.IME = !interrupts.IME;
    }
    else if (interrupts.pending & 0x8)
    {
        interrupts.IF.reset(interrupts.InterruptType::Serial);
        interrupts.IME = !interrupts.IME;
    }
    else if (interrupts.pending & 0x16)
    {
        interrupts.IF.reset(interrupts.InterruptType::Joypad);
        interrupts.IME = !interrupts.IME;
    }

    return 5;
}
