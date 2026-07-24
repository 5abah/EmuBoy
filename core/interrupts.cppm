// interrupts
export module interrupts;
import std;

export class Interrupts
{
    enum class InterruptType
    {
        VBlank = 0,
        LCDStat,
        Timer,
        Serial,
        Joypad
    };

  public:
    bool IME{};
    std::bitset<8> IE{};
    std::bitset<8> IF{};
};
