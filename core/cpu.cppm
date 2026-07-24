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

    std::uint8_t step();

    std::uint8_t nop();
    std::uint8_t ldRegToReg(std::uint8_t regIndexY, std::uint8_t regIndexZ); // LD r, r'
    std::uint8_t ldRegImmediate(std::uint8_t regIndex);                      // LD r, d8
    std::uint8_t ldRegPairImmediate(std::uint8_t regIndex);                  // LD rr,n16
    std::uint8_t ldIndirectHLIncrementA();
    std::uint8_t ldAIndirectHLIncrement();
    std::uint8_t ldIndirectHLDecrementA();
    std::uint8_t ldAIndirectHLDecrement();
    std::uint8_t ldSPToHL();
    std::uint8_t ldHLStackPointerPlusOffset();
    std::uint8_t ldAddressStackPointer();
    std::uint8_t accumulatorRegisterArithmetic(std::uint8_t aluIndex, std::uint8_t regIndex); // ADD A, r
    std::uint8_t addHLRegPair(std::uint8_t regIndexP);                                        // ADD HL, rr
    std::uint8_t incRegOrMemory(std::uint8_t regIndexY);                                      // INC
    std::uint8_t incRegPair(std::uint8_t regIndexP);                                          // INC rr
    std::uint8_t decRegOrMemory(std::uint8_t regIndexY);                                      // DEC
    std::uint8_t decRegPair(std::uint8_t regIndexP);                                          // DEC rr
    std::uint8_t pushRegPair(std::uint8_t regIndexP);                                         // PUSH
    std::uint8_t popRegPair(std::uint8_t regIndexP);                                          // POP
    std::uint8_t relativeJump();
    std::uint8_t conditionalJump(std::uint8_t conditionIndexY);         // JP cc
    std::uint8_t relativeConditionalJump(std::uint8_t conditionIndexY); // JR cc
    std::uint8_t callConditional(std::uint8_t conditionYIndex);         // CALL cc
    std::uint8_t returnConditional(std::uint8_t conditionYIndex);       // RET cc
    std::uint8_t restart(std::uint8_t y);                               // RST
    std::uint8_t disableInterrupts();
    std::uint8_t enableInterrupts();
    std::uint8_t decimalAdjustAccumulator();
    std::uint8_t complementAccumulator();
    std::uint8_t complementCarryFlag();
    std::uint8_t setCarryFlag();
    std::uint8_t rotateLeftAccumulator();
    std::uint8_t rotateLeftAccumulatorCarry();
    std::uint8_t rotateRightAccumulator();
    std::uint8_t rotateRightAccumulatorCarry();
    std::uint8_t jmpToHL();
    std::uint8_t retFromInterrupt();
    // CB Prefixed OpCodes
    std::uint8_t rotateLeftCB(std::uint8_t regIndexZ);
    std::uint8_t rotateLeftCarryCB(std::uint8_t regIndexZ);
    std::uint8_t rotateRightCB(std::uint8_t regIndexZ);
    std::uint8_t rotateRightCarryCB(std::uint8_t regIndexZ);
    std::uint8_t shiftLeftArithmeticCB(std::uint8_t regIndexZ);
    std::uint8_t shiftRightArithmeticCB(std::uint8_t regIndexZ);
    std::uint8_t shiftRightLogicalCB(std::uint8_t regIndexZ);
    std::uint8_t swapNibblesCB(std::uint8_t regIndexZ);
    std::uint8_t testBit(std::uint8_t bit, std::uint8_t regIndexZ);  // BIT
    std::uint8_t setBit(std::uint8_t bit, std::uint8_t regIndexZ);   // SET
    std::uint8_t clearBit(std::uint8_t bit, std::uint8_t regIndexZ); // RES
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
                break;
            case 1:
                cycleCount += ldAddressStackPointer();
                break;
            case 2:
                return 1;
                break;
            case 3:
                cycleCount += relativeJump();
                break;
            default:
                cycleCount += relativeConditionalJump(y - 4);
            }
            break;
        case 1:
            switch (q)
            {
            case 0:
                cycleCount += ldRegPairImmediate(p);
                break;
            case 1:
                cycleCount += addHLRegPair(p);
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
                    // TODO: LD (BC), A -- no function yet. Mirror ldIndirectHLIncrementA()
                    // but target Registers::RP::BC and drop the increment.
                    break;
                case 1:
                    // TODO: LD (DE), A -- same gap, target RP::DE.
                    break;
                case 2:
                    cycleCount += ldIndirectHLIncrementA();
                    break;
                case 3:
                    cycleCount += ldIndirectHLDecrementA();
                    break;
                }
                break;
            case 1:
                switch (p)
                {
                case 0:
                    // TODO: LD A, (BC) -- no function yet.
                    break;
                case 1:
                    // TODO: LD A, (DE) -- no function yet.
                    break;
                case 2:
                    cycleCount += ldAIndirectHLIncrement();
                    break;
                case 3:
                    cycleCount += ldAIndirectHLDecrement();
                    break;
                }
                break;
            }
            break;
        case 3:
            q == 0 ? cycleCount += incRegPair(p) : cycleCount += decRegPair(p);
            break;
        case 4:
            cycleCount += incRegOrMemory(y);
            break;
        case 5:
            cycleCount += decRegOrMemory(y);
            break;
        case 6:
            cycleCount += ldRegImmediate(y);
            break;
        case 7:
            switch (y)
            {
            case 0:
                cycleCount += rotateLeftAccumulator();
                break;
            case 1:
                cycleCount += rotateRightAccumulator();
                break;
            case 2:
                cycleCount += rotateLeftAccumulatorCarry();
                break;
            case 3:
                cycleCount += rotateRightAccumulatorCarry();
                break;
            case 4:
                cycleCount += decimalAdjustAccumulator();
                break;
            case 5:
                cycleCount += complementAccumulator();
                break;
            case 6:
                cycleCount += setCarryFlag();
                break;
            case 7:
                cycleCount += complementCarryFlag();
                break;
            }
            break;
        }
        break;
    case 1:
        if (z == 6 && y == 6)
            return 10;
        cycleCount += ldRegToReg(y, z);
        break;
    case 2:
        cycleCount += accumulatorRegisterArithmetic(y, z);
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
                break;
            case 4:
                // TODO: LDH (n), A -- write A to 0xFF00+n8. No function yet.
                break;
            case 5:
                // TODO: ADD SP, e -- signed add into SP. No function yet.
                break;
            case 6:
                // TODO: LDH A, (n) -- read 0xFF00+n8 into A. No function yet.
                break;
            case 7:
                cycleCount += ldHLStackPointerPlusOffset();
                break;
            }
            break;
        case 1:
            if (q)
            {
                switch (p)
                {
                case 0:
                    // TODO: RET (unconditional) -- no standalone function yet.
                    break;
                case 1:
                    cycleCount += retFromInterrupt();
                    break;
                case 2:
                    cycleCount += jmpToHL();
                    break;
                case 3:
                    cycleCount += ldSPToHL();
                    break;
                }
            }
            else
                cycleCount += popRegPair(p);
            break;
        case 2:
            switch (y)
            {
            case 0:
            case 1:
            case 2:
            case 3:
                cycleCount += conditionalJump(y);
                break;
            case 4:
                // TODO: LD (0xFF00+C), A -- no function yet.
                break;
            case 5:
                // TODO: LD (nn), A -- no function yet.
                break;
            case 6:
                // TODO: LD A, (0xFF00+C) -- no function yet.
                break;
            case 7:
                // TODO: LD A, (nn) -- no function yet.
                break;
            }
            break;
        case 3:
            switch (y)
            {
            case 0:
                // TODO: JP nn (unconditional) -- no function yet.
                break;
            case 1:
                // TODO: CB-prefixed dispatch. Needs its own x/y/z decode of the
                // *next* byte, routed to rotateLeftCB / rotateRightCB /
                // rotateLeftCarryCB / rotateRightCarryCB / shiftLeftArithmeticCB /
                // shiftRightArithmeticCB / shiftRightLogicalCB / swapNibblesCB /
                // testBit / setBit / clearBit. Resolve the calling-convention
                // mismatch noted above before wiring this up.
                break;
            case 2:
            case 3:
            case 4:
            case 5:
                throw std::runtime_error("Invalid Opcode!"); // removed on GB (OUT/IN/EX)
            case 6:
                cycleCount += disableInterrupts();
                break;
            case 7:
                cycleCount += enableInterrupts();
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
                break;
            case 4:
            case 5:
            case 6:
            case 7:
                throw std::runtime_error("Invalid Opcode!"); // removed on GB
            }
            break;
        case 5:
            if (q == 0)
                cycleCount += pushRegPair(p);
            else if (p == 0)
            {
                // TODO: CALL nn (unconditional) -- no function yet.
            }
            else
                throw std::runtime_error("Invalid Opcode!"); // removed on GB
            break;
        case 6:
            // TODO: alu[y] n -- accumulatorRegisterArithmetic() only reads
            // regs.reg8[regIndex]; it has no immediate-operand mode, so this
            // needs a new function/overload that reads n8 from bus first.
            break;
        case 7:
            cycleCount += restart(y);
            break;
        }
        break;
    default:
        throw std::runtime_error("Invalid Opcode!");
        std::unreachable();
    }
    return cycleCount;
}
