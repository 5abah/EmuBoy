// cpu
export module cpu;
import std;
import bus;

export class CPU
{
    struct Registers
    {
        enum class Flags
        {
            Carry = 4,
            HalfCarry,
            Subtraction,
            Zero
        };

        enum class REG8
        {
            B = 0,
            C,
            D,
            E,
            H,
            L,
            HL,
            A
        };
        std::array<std::uint8_t, 8> reg8{}; // index through this with REG8 enum

        enum class RP
        {
            BC = 0,
            DE,
            HL,
            SP,
            AF
        };
        std::array<std::uin16_t, 5> rp{}; // register pairs + SP, also I put AF in this same place since rp2 is
                                          // basically the same just with AF index with RP enum

        std::uint8_t getLo(std::uint16_t reg);
        std::uint8_t getHi(std::uint16_t reg);
        std::uint8_t getFlag(std::uint16_t afReg, Flags flag2Get);
        std::uint8_t setFlag(std::uint16_t afReg, Flags flag2Set);
    };

    CPU();
    Bus &bus;
    Registers regs{};
    std::uint8_t IME{0}; // Interrupt Master Enable flag
    void step();
};

std::uint8_t CPU::Registers::getHi(std::uint16_t reg)
{
    return reg >> 8;
}

std::uint8_t CPU::Registers::getLo(std::uint16_t reg)
{
    return reg;
}

void step()
{
    std::uint8_t x{}, y{}, z{}, q{}, p{};
    std::uint8_t opcode{};
    switch (x)
    {
    case 0:
        switch (z)
        {
        case 0:
            switch (y)
            {
            case 0:
                break;
            case 1: {
                // LD(nn), SP
                break;
            }
            case 2:
                break;
            case 3:
            default:
            }
        case 1:
            // q ? /*ADD HL, rp[p] : ; // ld rp[p], nn*/
            break;
        case 2:
            switch (q)
            {
            case 0:
                switch (p)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                }
            case 1:
                switch (p)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                }
            }
        case 3:
            q ? /*DEC rp[p]*/ : ; // INC rp[p]
        case 4:
            // inc r[y]
        case 5:
            // Dec r[y]
        case 6:
            // Ld r[y], n
        case 7:
            switch (y)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            }
        }
        break;
    case 1:
        if (z == 6)
        y == 6 ? /*HALT*/ : // Ld r[y], r[z]
            case 2:
            // alu[y] r[z]
        case 3:
            switch (z)
            {
            case 0:
                switch (y)
                {
                case 4:
                case 5:
                case 6:
                case 7:
                default:
                }
            case 1:
                if (q)
                {
                    switch (p)
                    {
                    case 0:
                        // RET
                    case 1:
                        // RETI
                    case 2:
                        // JP HL
                    case 3:
                        // LD SP, HL
                    }
                }
                else
                    // pop rp2[p]
                case 2:
                    switch (y)
                    {
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    default:
                    }
            case 3:
                switch (y)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                }
            case 4:
                if (y <= 3)
                // CALL cc[y], nn
                else
                    // removed
                    case 5:
                    if (q)
                    {
                        if (p == 0)
                            // call nn
                            else
                        // removed
                    }
                case 6:
                    // alu[y] n
                case 7:
                // RST y*8
            }
    }
}
