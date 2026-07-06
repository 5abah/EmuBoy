export module opcode;
import std;
import gameboy;

void decodeOpcode(GameBoy &gb)
{
    auto &PC = gb.cpu.regs.PC;
    auto &instr = gb.rom.instr;

    std::uint8_t x{}, y{}, z{}, q{}, p{};
    std::uint8_t opcode{};
    while (true)
    {
        opcode = instr[PC++];
        x = (opcode >> 6) & 0x03;
        y = (opcode >> 3) & 0x07;
        z = opcode & 0x07;
        p = y >> 1;
        q = y % 2;

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
                    std::uint8_t nnLo = instr[PC++];
                    std::uint8_t nnHi = instr[PC++];
                    std::uint16_t addr = nnLo | (nnHi << 8);
                    gb.memory[addr] = getLo(gb.cpu.regs.regPsp[+REGSP::SP]);
                    gb.memory[addr + 1] = getHi(gb.cpu.regs.regPsp[+REGSP::SP]);
                    break;
                }
                case 2:
                    break;
                case 3:
                default:
                }
            case 1:
                q ? /*ADD HL, rp[p]*/ : ; // ld rp[p], nn
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
}
