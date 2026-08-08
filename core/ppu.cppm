// ppu
export module ppu;

import memliterals;
import std;

using namespace MemLiteral;

enum class PPUMode
{
    HBlank,
    VBLANK,
    OAMScan,
    PixelTrasnfer
};

export {
    struct LCDC
    {
    };

    struct STAT
    {
        std::bitset<8> stat{};
        bool check();
    };
    struct BGP
    {
    };

    class PPU
    {
        void renderScanLine();

        static constexpr std::uint8_t width{160}, height{144};

      public:
        std::array<std::uint8_t, 8_KiB> vram{};
        std::array<std::uint8_t, 160> oam{};
        std::array<std::array<std::uint8_t, height>, width> frameBuffer;

        PPUMode mode{PPUMode::OAMScan};
        std::uint8_t modeCycles{};

        struct LcdRegs
        {
            LCDC lcdc{};
            STAT stat{};
            std::uint8_t scy{}, scx{}, ly{}, lyc{};
            BGP bgp{};
        };

        bool vblankPending{}, statPending{};

        std::uint8_t readVRAM(std::uint16_t addr);
        void writeVRAM(std::uint16_t addr, std::uint8_t val);
        std::uint8_t readOAM(std::uint16_t addr);
        void writeOAM(std::uint16_t addr, std::uint8_t val);

        std::uint8_t step(std::uint8_t cycles);

        std::span<const std::uint8_t> framebufferView();

        std::uint8_t consumeBLankInterrupt();
        std::uint8_t consumeStateInterrupt();
    };
}
