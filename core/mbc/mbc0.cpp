// mbc0.cpp
module mbc;
import std;
MBC0::MBC0(std::vector<std::uint8_t> rom) : romData{std::move(rom)}
{
}
std::uint8_t MBC0::read(std::uint16_t addr) const
{
    return romData[addr];
}
void MBC0::write(std::uint16_t addr, std::uint8_t val)
{
}
std::unique_ptr<MBC> createMBC(std::uint8_t cartridgeTypeByte, std::vector<std::uint8_t> rom, std::size_t ramSize)
{
    if (cartridgeTypeByte == 0x00)
        return std::make_unique<MBC0>(std::move(rom));
    if (cartridgeTypeByte == 0x01 || cartridgeTypeByte == 0x02 || cartridgeTypeByte == 0x03)
        return std::make_unique<MBC1>(std::move(rom), ramSize);
    throw std::runtime_error("Unsupported cartridge type");
}
