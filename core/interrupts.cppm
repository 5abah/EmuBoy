// interrupts
export module interrupts;
import std;

export struct Interrupts
{
    enum InterruptType
    {
        VBlank = 0,
        LCDStat,
        Timer,
        Serial,
        Joypad
    };

    struct InterruptEntry
    {
        std::uint8_t mask;
        InterruptType type;
        std::uint16_t vector;
    };

    bool IME{}, IMEPendingEnable{};
    std::bitset<8> IE{};
    std::bitset<8> IF{};
    std::uint16_t pending{};
    void handleInterrupts();
};
