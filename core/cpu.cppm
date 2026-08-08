// cpu
export module cpu;
import std;
import bus;

export class CPU
{
  public:
    struct Registers
    {
        enum Flags
        {
            Carry = 0,   // 4
            HalfCarry,   // 5
            Subtraction, // 6
            Zero         // 7
        };

        enum Conditions
        {
            NZ = 0,
            Z,
            NC,
            C
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

        enum RP
        {
            BC = 0,
            DE,
            HL,
            SP,
            AF
        };

        enum ALU
        {
            ADD = 0,
            ADC,
            SUB,
            SBC,
            AND,
            XOR,
            OR,
            CP
        };
        std::array<std::uint16_t, 5> rp{}; // register pairs + SP, also I put AF in this same place since rp2 is
                                           // basically the same just with AF index with RP enum

        std::uint16_t PC{0x100};
        std::bitset<4> flag{};

        void flagCheck(std::uint16_t result, std::uint8_t a, std::uint8_t b, std::uint8_t carryIn,
                       Registers::ALU operation);
        std::uint8_t getLo(std::uint16_t reg);
        std::uint8_t getHi(std::uint16_t reg);
        std::uint16_t get16(std::uint8_t pair);
        void set16(std::uint8_t pair, std::uint16_t value);
        std::uint8_t getFlag(std::uint16_t afReg, Flags flag2Get);
        std::uint8_t setFlag(std::uint16_t afReg, Flags flag2Set);
        std::uint16_t make16(std::uint8_t lo, std::uint8_t hi);
    };

    Bus &bus;
    Registers regs{};
    CPU(Bus &busGet) : bus{busGet} {};
    bool halted{};

    std::uint8_t step();
    std::uint8_t nop();
    std::uint8_t handleInterrupts();
    std::uint8_t ldRegToReg(std::uint8_t regIndexY, std::uint8_t regIndexZ); // LD r, r'
    std::uint8_t ldRegImmediate(std::uint8_t regIndex);                      // LD r, n8
    std::uint8_t ldRegPairImmediate(std::uint8_t regIndex);                  // LD rr, n16
    std::uint8_t ldIndirectHLIncrementA();                                   // LD (HL+), A
    std::uint8_t ldAIndirectHLIncrement();                                   // LD A, (HL+)
    std::uint8_t ldIndirectHLDecrementA();                                   // LD (HL-), A
    std::uint8_t ldAIndirectHLDecrement();                                   // LD A, (HL-)
    std::uint8_t ldIndirectBCA();                                            // LD (BC), A
    std::uint8_t ldAIndirectBC();                                            // LD A, (BC)
    std::uint8_t ldIndirectDEA();                                            // LD (DE), A
    std::uint8_t ldAIndirectDE();                                            // LD A, (DE)
    std::uint8_t loadHighAddressImmediate();                                 // LDH (n), A
    std::uint8_t loadAHighAddressImmediate();                                // LDH A, (n)
    std::uint8_t loadHighAddressC();                                         // LD (C), A
    std::uint8_t loadAHighAddressC();                                        // LD A, (C)
    std::uint8_t loadAddressA();                                             // LD (nn), A
    std::uint8_t loadAAddress();                                             // LD A, (nn)
    std::uint8_t addSPOffset();                                              // ADD SP, e
    std::uint8_t jmpImmediate();                                             // JP nn
    std::uint8_t callImmediate();                                            // CALL nn
    std::uint8_t returnUnconditional();                                      // RET
    std::uint8_t halt();                                                     // HALT
    std::uint8_t stop();                                                     // STOP
    std::uint8_t ldSPToHL();                                                 // LD SP, HL
    std::uint8_t ldHLStackPointerPlusOffset();                               // LD HL, SP+e
    std::uint8_t ldAddressStackPointer();                                    // LD (nn), SP
    std::uint8_t accumulatorImmediateArithmetic(std::uint8_t aluIndex);
    std::uint8_t accumulatorRegisterArithmetic(std::uint8_t aluIndex,
                                               std::uint8_t regIndex);  // ADD/ADC/SUB/SBC/AND/XOR/OR/CP A, r
    std::uint8_t addHLRegPair(std::uint8_t regIndexP);                  // ADD HL, rr
    std::uint8_t incRegOrMemory(std::uint8_t regIndexY);                // INC r / INC (HL)
    std::uint8_t incRegPair(std::uint8_t regIndexP);                    // INC rr
    std::uint8_t decRegOrMemory(std::uint8_t regIndexY);                // DEC r / DEC (HL)
    std::uint8_t decRegPair(std::uint8_t regIndexP);                    // DEC rr
    std::uint8_t pushRegPair(std::uint8_t regIndexP);                   // PUSH rr
    std::uint8_t popRegPair(std::uint8_t regIndexP);                    // POP rr
    std::uint8_t conditionalJump(std::uint8_t conditionIndexY);         // JP cc, nn
    std::uint8_t relativeJump();                                        // JR e (unconditional)
    std::uint8_t relativeConditionalJump(std::uint8_t conditionIndexY); // JR cc, e
    std::uint8_t callConditional(std::uint8_t conditionYIndex);         // CALL cc, nn
    std::uint8_t returnConditional(std::uint8_t conditionYIndex);       // RET cc
    std::uint8_t restart(std::uint8_t y);                               // RST n
    std::uint8_t disableInterrupts();                                   // DI
    std::uint8_t enableInterrupts();                                    // EI
    std::uint8_t decimalAdjustAccumulator();                            // DAA
    std::uint8_t complementAccumulator();                               // CPL
    std::uint8_t complementCarryFlag();                                 // CCF
    std::uint8_t setCarryFlag();                                        // SCF
    std::uint8_t rotateLeftAccumulator();                               // RLCA
    std::uint8_t rotateLeftAccumulatorCarry();                          // RLA
    std::uint8_t rotateRightAccumulator();                              // RRCA
    std::uint8_t rotateRightAccumulatorCarry();                         // RRA
    std::uint8_t jmpToHL();                                             // JP HL
    std::uint8_t retFromInterrupt();                                    // RETI
    // CB Prefixed OpCodes
    std::uint8_t rotateLeftCB(std::uint8_t regIndexZ);               // RLC r / RLC (HL)
    std::uint8_t rotateLeftCarryCB(std::uint8_t regIndexZ);          // RL r / RL (HL)
    std::uint8_t rotateRightCB(std::uint8_t regIndexZ);              // RRC r / RRC (HL)
    std::uint8_t rotateRightCarryCB(std::uint8_t regIndexZ);         // RR r / RR (HL)
    std::uint8_t shiftLeftArithmeticCB(std::uint8_t regIndexZ);      // SLA r / SLA (HL)
    std::uint8_t shiftRightArithmeticCB(std::uint8_t regIndexZ);     // SRA r / SRA (HL)
    std::uint8_t shiftRightLogicalCB(std::uint8_t regIndexZ);        // SRL r / SRL (HL)
    std::uint8_t swapNibblesCB(std::uint8_t regIndexZ);              // SWAP r / SWAP (HL)
    std::uint8_t testBit(std::uint8_t bit, std::uint8_t regIndexZ);  // BIT b, r / BIT b, (HL)
    std::uint8_t setBit(std::uint8_t bit, std::uint8_t regIndexZ);   // SET b, r / SET b, (HL)
    std::uint8_t clearBit(std::uint8_t bit, std::uint8_t regIndexZ); // RES b, r / RES b, (HL)
};

std::uint8_t CPU::Registers::getLo(std::uint16_t reg)
{
    return static_cast<std::uint8_t>(reg & 0xFF);
}

std::uint8_t CPU::Registers::getHi(std::uint16_t reg)
{
    return static_cast<std::uint8_t>((reg >> 8) & 0xFF);
}

std::uint16_t CPU::Registers::make16(std::uint8_t lo, std::uint8_t hi)
{
    return (static_cast<std::uint16_t>(hi) << 8) | lo;
}

std::uint16_t CPU::Registers::get16(std::uint8_t pair)
{
    switch (static_cast<Registers::RP>(pair))
    {
    case BC:
        return make16(reg8[static_cast<std::size_t>(REG8::C)], reg8[static_cast<std::size_t>(REG8::B)]);
    case DE:
        return make16(reg8[static_cast<std::size_t>(REG8::E)], reg8[static_cast<std::size_t>(REG8::D)]);
    case HL:
        return make16(reg8[static_cast<std::size_t>(REG8::L)], reg8[static_cast<std::size_t>(REG8::H)]);
    case SP:
        return rp[SP];
    case AF:
        std::uint8_t f = 0;
        if (flag.test(Carry))
            f |= (1 << 4);
        if (flag.test(HalfCarry))
            f |= (1 << 5);
        if (flag.test(Subtraction))
            f |= (1 << 6);
        if (flag.test(Zero))
            f |= (1 << 7);
        return make16(f, reg8[static_cast<std::size_t>(REG8::A)]);
    }
    return 0;
}

void CPU::Registers::set16(std::uint8_t pair, std::uint16_t value)
{
    rp[pair] = value;

    std::uint8_t hi = getHi(value);
    std::uint8_t lo = getLo(value);

    switch (static_cast<Registers::RP>(pair))
    {
    case BC:
        reg8[static_cast<std::size_t>(REG8::B)] = hi;
        reg8[static_cast<std::size_t>(REG8::C)] = lo;
        break;
    case DE:
        reg8[static_cast<std::size_t>(REG8::D)] = hi;
        reg8[static_cast<std::size_t>(REG8::E)] = lo;
        break;
    case HL:
        reg8[static_cast<std::size_t>(REG8::H)] = hi;
        reg8[static_cast<std::size_t>(REG8::L)] = lo;
        break;
    case SP:
        break;
    case AF:
        reg8[static_cast<std::size_t>(REG8::A)] = hi;

        flag.set(Carry, (lo & (1 << 4)) != 0);
        flag.set(HalfCarry, (lo & (1 << 5)) != 0);
        flag.set(Subtraction, (lo & (1 << 6)) != 0);
        flag.set(Zero, (lo & (1 << 7)) != 0);

        rp[AF] &= 0xFFF0;
        break;
    }
}

std::uint8_t CPU::step()
{
    std::uint8_t opcode{bus.read(regs.PC)}, cycleCount{};
    std::uint8_t x = (opcode & 0xC0) >> 6;
    std::uint8_t y = (opcode & 0x38) >> 3;
    std::uint8_t z = opcode & 0x7;
    std::uint8_t p = y >> 1;
    std::uint8_t q = y % 2;
    regs.PC++;

    if (bus.interrupts.IMEPendingEnable)
    {
        bus.interrupts.IME = true;
        bus.interrupts.IMEPendingEnable = !bus.interrupts.IMEPendingEnable;
    }

    if (halted)
    {
        if ((bus.interrupts.IE.to_ulong() & bus.interrupts.IF.to_ulong()) != 0)
        {
            halted = false;
        }
        else
        {
            return 1;
        }
    }
    switch (x)
    {
    case 0:
        switch (z)
        {
        case 0:
            switch (y)
            {
            case 0:
                cycleCount += nop();
                cycleCount += handleInterrupts();
                break;
            case 1:
                cycleCount += ldAddressStackPointer();
                cycleCount += handleInterrupts();
                break;
            case 2:
                cycleCount += stop(); // STOP -- not HALT, corrected label
                cycleCount += handleInterrupts();
                break;
            case 3:
                cycleCount += relativeJump();
                cycleCount += handleInterrupts();
                break;
            default:
                cycleCount += relativeConditionalJump(y - 4);
                cycleCount += handleInterrupts();
            }
            break;
        case 1:
            switch (q)
            {
            case 0:
                cycleCount += ldRegPairImmediate(p);
                cycleCount += handleInterrupts();
                break;
            case 1:
                cycleCount += addHLRegPair(p);
                cycleCount += handleInterrupts();
                break;
            }
            break;
        case 2:
            switch (q)
            {
            case 0:
                switch (p)
                {
                case 0:
                    cycleCount += ldIndirectBCA();
                    cycleCount += handleInterrupts();
                    break;
                case 1:
                    cycleCount += ldIndirectDEA();
                    cycleCount += handleInterrupts();
                    break;
                case 2:
                    cycleCount += ldIndirectHLIncrementA();
                    cycleCount += handleInterrupts();
                    break;
                case 3:
                    cycleCount += ldIndirectHLDecrementA();
                    cycleCount += handleInterrupts();
                    break;
                }
                break;
            case 1:
                switch (p)
                {
                case 0:
                    cycleCount += ldAIndirectBC();
                    cycleCount += handleInterrupts();
                    break;
                case 1:
                    cycleCount += ldAIndirectDE();
                    cycleCount += handleInterrupts();
                    break;
                case 2:
                    cycleCount += ldAIndirectHLIncrement();
                    cycleCount += handleInterrupts();
                    break;
                case 3:
                    cycleCount += ldAIndirectHLDecrement();
                    cycleCount += handleInterrupts();
                    break;
                }
                break;
            }
            break;
        case 3:
            q == 0 ? cycleCount += incRegPair(p) : cycleCount += decRegPair(p);
            cycleCount += handleInterrupts();
            break;
        case 4:
            cycleCount += incRegOrMemory(y);
            cycleCount += handleInterrupts();
            break;
        case 5:
            cycleCount += decRegOrMemory(y);
            cycleCount += handleInterrupts();
            break;
        case 6:
            cycleCount += ldRegImmediate(y);
            cycleCount += handleInterrupts();
            break;
        case 7:
            switch (y)
            {
            case 0:
                cycleCount += rotateLeftAccumulator();
                cycleCount += handleInterrupts();
                break;
            case 1:
                cycleCount += rotateRightAccumulator();
                cycleCount += handleInterrupts();
                break;
            case 2:
                cycleCount += rotateLeftAccumulatorCarry();
                cycleCount += handleInterrupts();
                break;
            case 3:
                cycleCount += rotateRightAccumulatorCarry();
                cycleCount += handleInterrupts();
                break;
            case 4:
                cycleCount += decimalAdjustAccumulator();
                cycleCount += handleInterrupts();
                break;
            case 5:
                cycleCount += complementAccumulator();
                cycleCount += handleInterrupts();
                break;
            case 6:
                cycleCount += setCarryFlag();
                cycleCount += handleInterrupts();
                break;
            case 7:
                cycleCount += complementCarryFlag();
                cycleCount += handleInterrupts();
                break;
            }
            break;
        }
        break;
    case 1:
        if (z == 6 && y == 6)
        {
            // HALT -- correct instruction location, still only a flat return.
            // Needs halt() to set a real `halted` state, and step() needs a
            // check at the very top (before fetch) to skip normal execution
            // while halted, per step 6 above. Not done yet.
            cycleCount += halt();
            cycleCount += handleInterrupts();
            break;
        }
        cycleCount += ldRegToReg(y, z);
        cycleCount += handleInterrupts();
        break;
    case 2:
        cycleCount += accumulatorRegisterArithmetic(y, z);
        cycleCount += handleInterrupts();
        break;

    case 3:
        switch (z)
        {
        case 0:
            switch (y)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                cycleCount += returnConditional(y);
                cycleCount += handleInterrupts();
                break;
            case 4:
                cycleCount += loadHighAddressImmediate();
                cycleCount += handleInterrupts();
                break;
            case 5:
                cycleCount += addSPOffset();
                cycleCount += handleInterrupts();
                break;
            case 6:
                cycleCount += loadAHighAddressImmediate();
                cycleCount += handleInterrupts();
                break;
            case 7:
                cycleCount += ldHLStackPointerPlusOffset();
                cycleCount += handleInterrupts();
                break;
            }
            break;
        case 1:
            if (q)
            {
                switch (p)
                {
                case 0:
                    cycleCount += returnUnconditional();
                    cycleCount += handleInterrupts();
                    break;
                case 1:
                    cycleCount += retFromInterrupt();
                    cycleCount += handleInterrupts();
                    break;
                case 2:
                    cycleCount += jmpToHL();
                    cycleCount += handleInterrupts();
                    break;
                case 3:
                    cycleCount += ldSPToHL();
                    cycleCount += handleInterrupts();
                    break;
                }
            }
            else
            {
                cycleCount += popRegPair(p);
                cycleCount += handleInterrupts();
            }
            break;
        case 2:
            switch (y)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                cycleCount += conditionalJump(y);
                cycleCount += handleInterrupts();
                break;
            case 4:
                cycleCount += loadHighAddressC();
                cycleCount += handleInterrupts();
                break;
            case 5:
                cycleCount += loadAddressA();
                cycleCount += handleInterrupts();
                break;
            case 6:
                cycleCount += loadAHighAddressC();
                cycleCount += handleInterrupts();
                break;
            case 7:
                cycleCount += loadAAddress();
                cycleCount += handleInterrupts();
                break;
            }
            break;
        case 3:
            switch (y)
            {
            case 0:
                cycleCount += jmpImmediate();
                cycleCount += handleInterrupts();
                break;
            case 1: {
                std::uint8_t cbOpcode = bus.read(regs.PC++);
                std::uint8_t x2 = (cbOpcode & 0xC0) >> 6;
                std::uint8_t y2 = (cbOpcode & 0x38) >> 3;
                std::uint8_t z2 = cbOpcode & 0x7;

                switch (x2)
                {
                case 0:
                    switch (y2)
                    {
                    case 0:
                        cycleCount += rotateLeftCB(z2);
                        break;
                    case 1:
                        cycleCount += rotateRightCB(z2);
                        break;
                    case 2:
                        cycleCount += rotateLeftCarryCB(z2);
                        break;
                    case 3:
                        cycleCount += rotateRightCarryCB(z2);
                        break;
                    case 4:
                        cycleCount += shiftLeftArithmeticCB(z2);
                        break;
                    case 5:
                        cycleCount += shiftRightArithmeticCB(z2);
                        break;
                    case 6:
                        cycleCount += swapNibblesCB(z2);
                        break;
                    case 7:
                        cycleCount += shiftRightLogicalCB(z2);
                        break;
                    }
                    break;
                case 1:
                    cycleCount += testBit(y2, z2);
                    break;
                case 2:
                    cycleCount += clearBit(y2, z2);
                    break;
                case 3:
                    cycleCount += setBit(y2, z2);
                    break;
                }
                cycleCount += handleInterrupts();
                break;
            }
            case 2:
            case 3:
            case 4:
            case 5:
                cycleCount += handleInterrupts();
                throw std::runtime_error(
                    std::format("Invalid Opcode 0x{:02X} at PC 0x{:04X}", opcode, regs.PC)); // removed on GB
            case 6:
                cycleCount += disableInterrupts();
                cycleCount += handleInterrupts();
                break;
            case 7:
                cycleCount += enableInterrupts();
                cycleCount += handleInterrupts();
                break;
            }
            break;
        case 4:
            switch (y)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                cycleCount += callConditional(y);
                cycleCount += handleInterrupts();
                break;
            case 4:
            case 5:
            case 6:
            case 7:
                cycleCount += handleInterrupts();
                throw std::runtime_error(
                    std::format("Invalid Opcode 0x{:02X} at PC 0x{:04X}", opcode, regs.PC)); // removed on GB
            }
            break;
        case 5:
            if (q == 0)
            {
                cycleCount += pushRegPair(p);
                cycleCount += handleInterrupts();
            }
            else if (p == 0)
            {
                cycleCount += callImmediate();
                cycleCount += handleInterrupts();
            }
            else
            {
                cycleCount += handleInterrupts();
                throw std::runtime_error(
                    std::format("Invalid Opcode 0x{:02X} at PC 0x{:04X}", opcode, regs.PC)); // removed on GB
            }
            break;
        case 6:
            cycleCount += accumulatorImmediateArithmetic(y);
            cycleCount += handleInterrupts();
            break;
        case 7:
            cycleCount += restart(y);
            cycleCount += handleInterrupts();
            break;
        }
        break;
    default:
        cycleCount += handleInterrupts();
        throw std::runtime_error("Invalid Opcode!");
        std::unreachable();
    }
    return cycleCount;
}
