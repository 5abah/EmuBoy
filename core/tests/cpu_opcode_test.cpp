import cpu;
import bus;
import std;
#include <cassert>

// ---------- nop ----------
// No PC reset here on purpose -- this one specifically confirms the CPU's
// real default starting PC (0x0100), since it goes through step() with
// nothing set up beforehand.
void test_nop()
{
    Bus bus{};
    CPU cpu{bus};

    bus.write(0x0000, 0x00); // NOP opcode -- irrelevant here since PC starts at 0x0100, not 0x0000

    std::uint8_t cycles = cpu.step();

    assert(cpu.regs.PC == 0x0101);
    assert(cycles == 1);
    std::cout << "test_nop passed\n";
}

// ---------- ldRegImmediate ----------
void test_ldRegImmediate()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;

    // LD B,n -- opcode 0x06, then the immediate byte
    bus.write(0x0000, 0x06);
    bus.write(0x0001, 0x42);

    std::uint8_t cycles = cpu.step();

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] == 0x42);
    assert(cpu.regs.PC == 0x0002);
    assert(cycles == 2);
    std::cout << "test_ldRegImmediate passed\n";
}

// ---------- ldRegPairImmediate ----------
void test_ldRegPairImmediate()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;

    // LD BC,nn -- opcode 0x01, then low byte, then high byte
    bus.write(0x0000, 0x01);
    bus.write(0x0001, 0x34);
    bus.write(0x0002, 0x12);

    std::uint8_t cycles = cpu.step();

    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::BC)) == 0x1234);
    assert(cpu.regs.PC == 0x0003);
    assert(cycles == 3);
    std::cout << "test_ldRegPairImmediate passed\n";
}

// ---------- ldAddressStackPointer ----------
void test_ldAddressStackPointer()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::SP), 0xFFF0);

    // LD (nn),SP -- opcode 0x08, then a 2-byte address
    bus.write(0x0000, 0x08);
    bus.write(0x0001, 0x00);
    bus.write(0x0002, 0xC0); // target address 0xC000

    std::uint8_t cycles = cpu.step();

    assert(bus.read(0xC000) == 0xF0);                                                   // SP low byte
    assert(bus.read(0xC001) == 0xFF);                                                   // SP high byte
    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::SP)) == 0xFFF0); // SP unchanged
    assert(cycles == 5);
    std::cout << "test_ldAddressStackPointer passed\n";
}

// ---------- accumulatorRegisterArithmetic: ADD ----------
// No PC involved at all -- these call the opcode function directly and
// only touch register values, so no PC reset needed.
void test_add_halfCarry()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x0F;
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x01;

    std::uint8_t cycles =
        cpu.accumulatorRegisterArithmetic(CPU::Registers::ALU::ADD, static_cast<std::size_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x10);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == false);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == false);
    assert(cycles == 1);
    std::cout << "test_add_halfCarry passed\n";
}

void test_add_fullCarry()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0xFF;
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x01;

    std::uint8_t cycles =
        cpu.accumulatorRegisterArithmetic(CPU::Registers::ALU::ADD, static_cast<std::size_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x00);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == true);
    assert(cycles == 1);
    std::cout << "test_add_fullCarry passed\n";
}

// ---------- accumulatorRegisterArithmetic: ADC with carry-in ----------
void test_adc_withCarryIn()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x0E;
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x01;
    cpu.regs.flag.set(CPU::Registers::Flags::Carry); // carry-in = 1

    std::uint8_t cycles =
        cpu.accumulatorRegisterArithmetic(CPU::Registers::ALU::ADC, static_cast<std::size_t>(CPU::Registers::REG8::B));

    // 0x0E + 0x01 + 1(carry-in) = 0x10 -> half-carry should be set because of the carry-in
    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x10);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == true);
    assert(cycles == 1);
    std::cout << "test_adc_withCarryIn passed\n";
}

// ---------- accumulatorRegisterArithmetic: SUB ----------
void test_sub_equalOperands()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x05;
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x05;

    std::uint8_t cycles =
        cpu.accumulatorRegisterArithmetic(CPU::Registers::ALU::SUB, static_cast<std::size_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x00);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == false);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == false);
    assert(cycles == 1);
    std::cout << "test_sub_equalOperands passed\n";
}

// ---------- addHLRegPair ----------
void test_addHLRegPair_halfCarry()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0x0FFF);
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::BC), 0x0001);
    cpu.regs.flag.set(CPU::Registers::Flags::Zero); // confirm this is left alone

    std::uint8_t cycles = cpu.addHLRegPair(static_cast<std::size_t>(CPU::Registers::RP::BC));

    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::HL)) == 0x1000);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == false);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == true); // untouched by this instruction
    assert(cycles == 2);
    std::cout << "test_addHLRegPair_halfCarry passed\n";
}

// ---------- incRegOrMemory / decRegOrMemory ----------
void test_incReg_halfCarry()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::C)] = 0x0F;
    cpu.regs.flag.set(CPU::Registers::Flags::Carry); // confirm untouched

    std::uint8_t cycles = cpu.incRegOrMemory(static_cast<std::size_t>(CPU::Registers::REG8::C));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::C)] == 0x10);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true); // untouched
    assert(cycles == 1);
    std::cout << "test_incReg_halfCarry passed\n";
}

void test_decReg_wrapsToFF()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::C)] = 0x00;

    std::uint8_t cycles = cpu.decRegOrMemory(static_cast<std::size_t>(CPU::Registers::REG8::C));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::C)] == 0xFF);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == true);
    assert(cycles == 1);
    std::cout << "test_decReg_wrapsToFF passed\n";
}

// ---------- accumulatorRegisterArithmetic: AND/XOR/OR/CP ----------
void test_and_setsHalfCarry()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0xF0;
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x0F;

    std::uint8_t cycles =
        cpu.accumulatorRegisterArithmetic(CPU::Registers::ALU::AND, static_cast<std::size_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x00);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == true); // AND always sets this
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == false);
    assert(cycles == 1);
    std::cout << "test_and_setsHalfCarry passed\n";
}

void test_xor_clearsHalfCarry()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0xFF;
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x0F;

    std::uint8_t cycles =
        cpu.accumulatorRegisterArithmetic(CPU::Registers::ALU::XOR, static_cast<std::size_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0xF0);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == false); // this is the bug that was fixed
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == false);
    assert(cycles == 1);
    std::cout << "test_xor_clearsHalfCarry passed\n";
}

void test_or_clearsHalfCarry()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x00;
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x01;

    std::uint8_t cycles =
        cpu.accumulatorRegisterArithmetic(CPU::Registers::ALU::OR, static_cast<std::size_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x01);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == false); // same fix as XOR
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == false);
    assert(cycles == 1);
    std::cout << "test_or_clearsHalfCarry passed\n";
}

void test_cp_doesNotModifyA()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x05;
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x05;

    std::uint8_t cycles =
        cpu.accumulatorRegisterArithmetic(CPU::Registers::ALU::CP, static_cast<std::size_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x05); // CP must NOT write back to A
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == true); // A == B, so this behaves like a zero result
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Subtraction) == true);
    assert(cycles == 1);
    std::cout << "test_cp_doesNotModifyA passed\n";
}

// ---------- incRegPair / decRegPair ----------
void test_incRegPair_noFlagsTouched()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::DE), 0x00FF);
    cpu.regs.flag.set(CPU::Registers::Flags::Zero);
    cpu.regs.flag.set(CPU::Registers::Flags::Carry);

    std::uint8_t cycles = cpu.incRegPair(static_cast<std::size_t>(CPU::Registers::RP::DE));

    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::DE)) == 0x0100);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == true);  // untouched
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true); // untouched
    assert(cycles == 2);
    std::cout << "test_incRegPair_noFlagsTouched passed\n";
}

void test_decRegPair_actuallyDecrements()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::DE), 0x0100);

    std::uint8_t cycles = cpu.decRegPair(static_cast<std::size_t>(CPU::Registers::RP::DE));

    // this assert fails against the current code -- decRegPair currently
    // writes the value back unchanged instead of subtracting 1
    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::DE)) == 0x00FF);
    assert(cycles == 2);
    std::cout << "test_decRegPair_actuallyDecrements passed\n";
}

// ---------- ldRegToReg: HL-memory operand branch ----------
void test_ldRegToReg_fromHLMemory()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0xC000);
    bus.write(0xC000, 0x99);

    std::uint8_t cycles = cpu.ldRegToReg(static_cast<std::uint8_t>(CPU::Registers::REG8::B),
                                         static_cast<std::uint8_t>(CPU::Registers::REG8::HL));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] == 0x99);
    assert(cycles == 2);
    std::cout << "test_ldRegToReg_fromHLMemory passed\n";
}

// ---------- HL increment/decrement loads ----------
void test_ld_hlIncrement_a()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0xC000);
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x42;

    std::uint8_t cycles = cpu.ldIndirectHLIncrementA();

    assert(bus.read(0xC000) == 0x42);
    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::HL)) == 0xC001);
    assert(cycles == 2);
    std::cout << "test_ld_hlIncrement_a passed\n";
}

void test_ld_a_hlIncrement()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0xC000);
    bus.write(0xC000, 0x77);

    std::uint8_t cycles = cpu.ldAIndirectHLIncrement();

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x77);
    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::HL)) == 0xC001);
    assert(cycles == 2);
    std::cout << "test_ld_a_hlIncrement passed\n";
}

void test_ld_hlDecrement_a()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0xC000);
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x33;

    std::uint8_t cycles = cpu.ldIndirectHLDecrementA();

    assert(bus.read(0xC000) == 0x33);
    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::HL)) == 0xBFFF);
    assert(cycles == 2);
    std::cout << "test_ld_hlDecrement_a passed\n";
}

void test_ld_a_hlDecrement()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0xC000);
    bus.write(0xC000, 0x11);

    std::uint8_t cycles = cpu.ldAIndirectHLDecrement();

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x11);
    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::HL)) == 0xBFFF);
    assert(cycles == 2);
    std::cout << "test_ld_a_hlDecrement passed\n";
}

// ---------- push/pop retest, now that getHi/getLo wrap get16() correctly ----------
void test_push_pop_nonAF_stillCorrect()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::SP), 0xFFFE);
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::DE), 0xABCD);

    cpu.pushRegPair(static_cast<std::size_t>(CPU::Registers::RP::DE));

    assert(bus.read(0xFFFD) == 0xAB);
    assert(bus.read(0xFFFC) == 0xCD);

    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::DE), 0x0000);
    cpu.popRegPair(static_cast<std::size_t>(CPU::Registers::RP::DE));

    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::DE)) == 0xABCD);
    std::cout << "test_push_pop_nonAF_stillCorrect passed\n";
}

// ---------- callConditional / returnConditional round trip ----------
void test_call_then_ret_returnsToCallSite()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::SP), 0xFFFE);

    bus.write(0x0000, 0x34);
    bus.write(0x0001, 0x12); // call target 0x1234
    cpu.regs.flag.set(CPU::Registers::Flags::Zero);

    cpu.callConditional(static_cast<std::uint8_t>(CPU::Registers::Conditions::Z));
    assert(cpu.regs.PC == 0x1234);

    std::uint8_t retCycles = cpu.returnConditional(static_cast<std::uint8_t>(CPU::Registers::Conditions::Z));

    assert(cpu.regs.PC == 0x0002); // back at the instruction after the original CALL
    assert(retCycles == 5);
    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::SP)) == 0xFFFE); // SP restored
    std::cout << "test_call_then_ret_returnsToCallSite passed\n";
}

void test_returnConditional_notTaken()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::SP), 0xFFFE);
    cpu.regs.PC = 0x0050;
    cpu.regs.flag.reset(CPU::Registers::Flags::Zero);

    std::uint8_t cycles = cpu.returnConditional(static_cast<std::uint8_t>(CPU::Registers::Conditions::Z));

    assert(cpu.regs.PC == 0x0050);                                                      // untouched
    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::SP)) == 0xFFFE); // untouched
    assert(cycles == 2);
    std::cout << "test_returnConditional_notTaken passed\n";
}

// ---------- restart ----------
void test_restart_pushesPCAndJumpsToVector()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0150;
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::SP), 0xFFFE);

    std::uint8_t cycles = cpu.restart(4); // RST 0x20 (y=4 -> 4*8=0x20)

    assert(cpu.regs.PC == 0x0020);
    assert(bus.read(0xFFFD) == 0x01); // pushed PC high byte
    assert(bus.read(0xFFFC) == 0x50); // pushed PC low byte
    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::SP)) == 0xFFFC);
    assert(cycles == 4);
    std::cout << "test_restart_pushesPCAndJumpsToVector passed\n";
}

// ---------- interrupt enable/disable, jmpToHL, retFromInterrupt ----------
void test_disableInterrupts()
{
    Bus bus{};
    CPU cpu{bus};
    bus.interrupts.IME = true;

    cpu.disableInterrupts();

    assert(bus.interrupts.IME == false);
    std::cout << "test_disableInterrupts passed\n";
}

void test_enableInterrupts()
{
    Bus bus{};
    CPU cpu{bus};
    bus.interrupts.IME = false;

    cpu.enableInterrupts();

    assert(bus.interrupts.IME == true);
    std::cout << "test_enableInterrupts passed\n";
    // NOTE: this test does not check the real-hardware one-instruction
    // delay on EI -- go back and confirm your implementation actually
    // has that delay once step() is driving this instead of a direct call.
}

void test_jmpToHL()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0x3000);

    std::uint8_t cycles = cpu.jmpToHL();

    assert(cpu.regs.PC == 0x3000);
    assert(cycles == 1);
    std::cout << "test_jmpToHL passed\n";
}

void test_retFromInterrupt()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::SP), 0xFFFC);
    bus.write(0xFFFC, 0x34);
    bus.write(0xFFFD, 0x12);
    bus.interrupts.IME = false;

    std::uint8_t cycles = cpu.retFromInterrupt();

    assert(cpu.regs.PC == 0x1234);
    assert(bus.interrupts.IME == true);
    assert(cycles == 4);
    std::cout << "test_retFromInterrupt passed\n";
}

// ---------- conditionalJump: all four conditions, taken and not-taken ----------
void test_jp_z_taken()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0x34);
    bus.write(0x0001, 0x12); // target 0x1234
    cpu.regs.flag.set(CPU::Registers::Flags::Zero);

    std::uint8_t cycles = cpu.conditionalJump(static_cast<std::uint8_t>(CPU::Registers::Conditions::Z));

    assert(cpu.regs.PC == 0x1234);
    assert(cycles == 4);
    std::cout << "test_jp_z_taken passed\n";
}

void test_jp_z_notTaken()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0x34);
    bus.write(0x0001, 0x12);
    cpu.regs.flag.reset(CPU::Registers::Flags::Zero);

    std::uint8_t cycles = cpu.conditionalJump(static_cast<std::uint8_t>(CPU::Registers::Conditions::Z));

    assert(cpu.regs.PC == 0x0002); // fetched both bytes, did NOT jump
    assert(cycles == 3);
    std::cout << "test_jp_z_notTaken passed\n";
}

void test_jp_nc_taken()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0x00);
    bus.write(0x0001, 0x20); // target 0x2000
    cpu.regs.flag.reset(CPU::Registers::Flags::Carry);

    std::uint8_t cycles = cpu.conditionalJump(static_cast<std::uint8_t>(CPU::Registers::Conditions::NC));

    assert(cpu.regs.PC == 0x2000);
    assert(cycles == 4);
    std::cout << "test_jp_nc_taken passed\n";
}

// ---------- relativeConditionalJump: taken forward, taken backward, not-taken ----------
void test_jr_c_forward()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0x05); // +5 offset
    cpu.regs.flag.set(CPU::Registers::Flags::Carry);

    std::uint8_t cycles = cpu.relativeConditionalJump(static_cast<std::uint8_t>(CPU::Registers::Conditions::C));

    assert(cpu.regs.PC == 0x0006); // 0x0001 (after fetching the offset byte) + 5
    assert(cycles == 3);
    std::cout << "test_jr_c_forward passed\n";
}

void test_jr_c_backward()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0xFE); // -2 as signed 8-bit
    cpu.regs.flag.set(CPU::Registers::Flags::Carry);

    std::uint8_t cycles = cpu.relativeConditionalJump(static_cast<std::uint8_t>(CPU::Registers::Conditions::C));

    assert(cpu.regs.PC == 0xFFFF); // 0x0001 - 2, wraps -- confirm this is the behavior you actually want
    assert(cycles == 3);
    std::cout << "test_jr_c_backward passed\n";
}

void test_jr_nz_notTaken()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0x05);
    cpu.regs.flag.set(CPU::Registers::Flags::Zero); // NZ condition fails when Zero IS set

    std::uint8_t cycles = cpu.relativeConditionalJump(static_cast<std::uint8_t>(CPU::Registers::Conditions::NZ));

    assert(cpu.regs.PC == 0x0001); // did not jump
    assert(cycles == 2);
    std::cout << "test_jr_nz_notTaken passed\n";
}

int main()
{
    test_nop();
    test_ldRegImmediate();
    test_ldRegPairImmediate();
    test_ldAddressStackPointer();

    test_add_halfCarry();
    test_add_fullCarry();
    test_adc_withCarryIn();
    test_sub_equalOperands();

    test_addHLRegPair_halfCarry();

    test_and_setsHalfCarry();
    test_xor_clearsHalfCarry();
    test_or_clearsHalfCarry();
    test_cp_doesNotModifyA();

    test_ldRegToReg_fromHLMemory();
    test_ld_hlIncrement_a();
    test_ld_a_hlIncrement();
    test_ld_hlDecrement_a();
    test_ld_a_hlDecrement();

    test_push_pop_nonAF_stillCorrect();
    test_call_then_ret_returnsToCallSite();
    test_returnConditional_notTaken();
    test_restart_pushesPCAndJumpsToVector();

    test_disableInterrupts();
    test_enableInterrupts();
    test_jmpToHL();
    test_retFromInterrupt();

    test_incReg_halfCarry();
    test_decReg_wrapsToFF();
    test_incRegPair_noFlagsTouched();
    test_decRegPair_actuallyDecrements();

    test_jp_z_taken();
    test_jp_z_notTaken();
    test_jp_nc_taken();

    test_jr_c_forward();
    test_jr_c_backward();
    test_jr_nz_notTaken();

    std::cout << "all tests passed\n";
    return 0;
}
