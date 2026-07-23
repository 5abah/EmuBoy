// interrupts
export module interrupts;

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
};
