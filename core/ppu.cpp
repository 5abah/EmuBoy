// ppu
module ppu;

struct LCDC
{
};

struct BGP
{
};

bool STAT::check()
{
    switch (stat.to_ulong())
    {
    case 0:
    case 1:
        return stat[0];

    case 2:
        if (PPU::LcdRegs::ly == PPU::LcdRegs::lyc)
            stat.set(2);
    }
};

std::uint8_t PPU::readVRAM(std::uint16_t addr)
{
    return static_cast<std::uint8_t>(mode) >= 3 ? 0 : vram[addr];
}

std::uint8_t PPU::readOAM(std::uint16_t addr)
{
    return static_cast<std::uint8_t>(mode) >= 2 ? 0 : oam[addr];
}

void PPU::writeVRAM(std::uint16_t addr, std::uint8_t val)
{
    static_cast<std::uint8_t>(mode) >= 3 ? 0 : vram[addr] = val;
}

void PPU::writeOAM(std::uint16_t addr, std::uint8_t val)
{
    static_cast<std::uint8_t>(mode) >= 2 ? 0 : oam[addr] = val;
}
