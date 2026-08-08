// mbc1.cpp
module mbc;
import std;

MBC1::MBC1(std::vector<std::uint8_t> rom, std::size_t ramSize) : romData{std::move(rom)}
{
    ramData.resize(ramSize);
}
std::uint8_t MBC1::read(std::uint16_t addr) const
{
    if (addr < 0x4000)
        return romData[addr];
    if (addr < 0x8000)
    {
        std::size_t offsetInBank = addr - 0x4000;
        std::size_t bankStart = effectiveRomBank() * 0x4000;
        return romData[bankStart + offsetInBank];
    }
    if (!ramEnabled || ramData.empty())
        return 0xFF;
    std::size_t ramOffset = addr - 0xA000;
    return ramOffset < ramData.size() ? ramData[ramOffset] : 0xFF;
}
void MBC1::write(std::uint16_t addr, std::uint8_t val)
{
    if (addr < 0x2000)
        ramEnabled = ((val & 0x0F) == 0x0A);
    else if (addr < 0x4000)
    {
        std::uint8_t bankBits = val & 0x1F;
        romBankLow5 = bankBits == 0 ? 1 : bankBits;
    }
    else if (addr < 0x6000)
        bank2 = val & 0x03;
    else if (addr < 0x8000)
        advancedBankingMode = (val & 0x01) != 0;
    else if (addr >= 0xA000 && addr < 0xC000 && ramEnabled)
    {
        std::size_t ramOffset = addr - 0xA000;
        if (ramOffset < ramData.size())
            ramData[ramOffset] = val;
    }
}

std::size_t MBC1::effectiveRomBank() const
{
    if (!advancedBankingMode)
        return romBankLow5;
    return (bank2 << 5) | romBankLow5;
}
