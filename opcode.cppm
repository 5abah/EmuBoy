export module opcode;
import std;
import gameboy;

void decodeOpcode(GameBoy &gb)
{
    for (size_t i =)

        switch (x)
        {
        case 0:
            switch (z)
            {
            case 0:
                switch (y)
                {
                case 0:
                    // nop
                case 1:
                case 2:
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
