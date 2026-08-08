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
    Timer timer;
    Joypad joypad;
    Cartridge cartridge;
    Interrupts interrupts;

    void loadROM(std::filesystem::path path);
    std::uint8_t read(std::uint16_t pcIndex) const;
    void write(std::uint16_t pcIndex, std::uint8_t byte2Write);
};

void Bus::loadROM(std::filesystem::path path)
{
    cartridge = Cartridge::loadFromFile(path);
}

std::uint8_t Bus::read(std::uint16_t pcIndex) const
{
    if (pcIndex < 0x8000)
        return cartridge.read(pcIndex);
    if (pcIndex >= 0xA000 && pcIndex < 0xC000)
        return cartridge.read(pcIndex);
    return addrSpace[pcIndex];
}

void Bus::write(std::uint16_t pcIndex, std::uint8_t byte2Write)
{
    if (pcIndex < 0x8000)
    {
        cartridge.write(pcIndex, byte2Write);
        return;
    }
    if (pcIndex >= 0xA000 && pcIndex < 0xC000)
    {
        cartridge.write(pcIndex, byte2Write);
        return;
    }
    addrSpace[pcIndex] = byte2Write;
}
