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

// ---------- relativeJump: unconditional JR e (new -- covers the y=3 case that
// was wrongly wired to relativeConditionalJump() with no condition) ----------
void test_relativeJump_forward()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0x05); // +5 offset

    std::uint8_t cycles = cpu.relativeJump();

    assert(cpu.regs.PC == 0x0006); // 0x0001 (after fetching offset) + 5
    assert(cycles == 3);
    std::cout << "test_relativeJump_forward passed\n";
}

void test_relativeJump_backward_wraps()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0xFE); // -2 as signed 8-bit

    std::uint8_t cycles = cpu.relativeJump();

    assert(cpu.regs.PC == 0xFFFF); // 0x0001 - 2, wraps
    assert(cycles == 3);
    std::cout << "test_relativeJump_backward_wraps passed\n";
}

void test_step_jr_unconditional_viaStep()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0x18); // JR e -- opcode 0x18, x=0 y=3 z=0
    bus.write(0x0001, 0x0A); // +10 offset

    std::uint8_t cycles = cpu.step();

    // step()'s own PC++ after the opcode fetch lands PC at 0x0001 before
    // relativeJump() reads the offset and adds it
    assert(cpu.regs.PC == 0x000C);
    assert(cycles == 3);
    std::cout << "test_step_jr_unconditional_viaStep passed\n";
}

// ---------- RLCA / RLA / RRCA / RRA ----------
void test_rlca()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x85; // 0b10000101

    std::uint8_t cycles = cpu.rotateLeftAccumulator();

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x0B); // 0b00001011
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);                 // old bit 7 was 1
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == false);
    assert(cycles == 1);
    std::cout << "test_rlca passed\n";
}

void test_rla()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x80; // 0b10000000
    cpu.regs.flag.reset(CPU::Registers::Flags::Carry);                       // old carry-in = 0

    std::uint8_t cycles = cpu.rotateLeftAccumulatorCarry();

    // bit 7 (1) becomes the new carry; old carry (0) shifts into bit 0
    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x00);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == false); // RLA always clears Z, even though result is 0
    assert(cycles == 1);
    std::cout << "test_rla passed\n";
}

void test_rrca()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x81; // 0b10000001

    std::uint8_t cycles = cpu.rotateRightAccumulator();

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0xC0); // 0b11000000
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);                 // old bit 0 was 1
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == false);
    assert(cycles == 1);
    std::cout << "test_rrca passed\n";
}

void test_rra()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x01; // 0b00000001
    cpu.regs.flag.set(CPU::Registers::Flags::Carry);                         // old carry-in = 1

    std::uint8_t cycles = cpu.rotateRightAccumulatorCarry();

    // bit 0 (1) becomes the new carry; old carry (1) shifts into bit 7
    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x80);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == false);
    assert(cycles == 1);
    std::cout << "test_rra passed\n";
}

// ---------- CPL / SCF / CCF ----------
void test_cpl()
{
    Bus bus{};
    CPU cpu{bus};

    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x35; // 0b00110101

    std::uint8_t cycles = cpu.complementAccumulator();

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0xCA); // 0b11001010
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Subtraction) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == true);
    assert(cycles == 1);
    std::cout << "test_cpl passed\n";
}

void test_scf()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.flag.reset(CPU::Registers::Flags::Carry);

    std::uint8_t cycles = cpu.setCarryFlag();

    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Subtraction) == false);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == false);
    assert(cycles == 1);
    std::cout << "test_scf passed\n";
}

void test_ccf_flipsFromSetToClear()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.flag.set(CPU::Registers::Flags::Carry);

    cpu.complementCarryFlag();

    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == false);
    std::cout << "test_ccf_flipsFromSetToClear passed\n";
}

void test_ccf_flipsFromClearToSet()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.flag.reset(CPU::Registers::Flags::Carry);

    cpu.complementCarryFlag();

    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);
    std::cout << "test_ccf_flipsFromClearToSet passed\n";
}

// ---------- DAA ----------
void test_daa_afterAddWithHalfCarry()
{
    Bus bus{};
    CPU cpu{bus};

    // Simulates the state right after ADD A: 0x08 + 0x08 -> raw result 0x10, H set (nibble overflow)
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x10;
    cpu.regs.flag.set(CPU::Registers::Flags::HalfCarry);
    cpu.regs.flag.reset(CPU::Registers::Flags::Carry);
    cpu.regs.flag.reset(CPU::Registers::Flags::Subtraction);

    std::uint8_t cycles = cpu.decimalAdjustAccumulator();

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x16); // correct BCD result for 8+8=16
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == false);            // always forced to 0
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == false);                // unaffected, wasn't set going in
    assert(cycles == 1);
    std::cout << "test_daa_afterAddWithHalfCarry passed\n";
}

void test_daa_afterAddCausingByteOverflow()
{
    Bus bus{};
    CPU cpu{bus};

    // Simulates raw ADD A: 0x99 + 0x01 -> 0x9A, no half-carry (9+1=0xA, not >0xF)
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x9A;
    cpu.regs.flag.reset(CPU::Registers::Flags::HalfCarry);
    cpu.regs.flag.reset(CPU::Registers::Flags::Carry);
    cpu.regs.flag.reset(CPU::Registers::Flags::Subtraction);

    std::uint8_t cycles = cpu.decimalAdjustAccumulator();

    // 99 + 1 = 100 decimal -- doesn't fit in one BCD byte, wraps to 00 with carry out
    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x00);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true); // DAA itself sets this here
    assert(cycles == 1);
    std::cout << "test_daa_afterAddCausingByteOverflow passed\n";
}

// ---------- RLC (through carry) ----------
void test_rlCarryCB_registerOperand()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x80; // 0b10000000
    cpu.regs.flag.reset(CPU::Registers::Flags::Carry);

    std::uint8_t cycles = cpu.rotateLeftCarryCB(static_cast<std::uint8_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] == 0x00); // old carry(0) -> bit0
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);                 // old bit7(1) -> carry
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == true);
    assert(cycles == 2);
    std::cout << "test_rlCarryCB_registerOperand passed\n";
}

void test_rlCarryCB_hlMemoryOperand()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0xC000);
    bus.write(0xC000, 0x01);
    cpu.regs.flag.set(CPU::Registers::Flags::Carry);

    std::uint8_t cycles = cpu.rotateLeftCarryCB(6);

    assert(bus.read(0xC000) == 0x03); // old carry(1) -> bit0, bit1 shifted from old bit0
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == false); // old bit7 was 0
    assert(cycles == 4);
    std::cout << "test_rlCarryCB_hlMemoryOperand passed\n";
}

// ---------- RRC (through carry) ----------
void test_rrCarryCB_registerOperand()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x01;
    cpu.regs.flag.set(CPU::Registers::Flags::Carry);

    std::uint8_t cycles = cpu.rotateRightCarryCB(static_cast<std::uint8_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] == 0x80); // old carry(1) -> bit7
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);                 // old bit0 was 1
    assert(cycles == 2);
    std::cout << "test_rrCarryCB_registerOperand passed\n";
}

void test_rrCarryCB_hlMemoryOperand()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0xC000);
    bus.write(0xC000, 0x00);
    cpu.regs.flag.reset(CPU::Registers::Flags::Carry);

    std::uint8_t cycles = cpu.rotateRightCarryCB(6);

    assert(bus.read(0xC000) == 0x00);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == false);
    assert(cycles == 4);
    std::cout << "test_rrCarryCB_hlMemoryOperand passed\n";
}

// ---------- SLA / SRA / SRL ----------
void test_sla()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x85; // 0b10000101

    std::uint8_t cycles = cpu.shiftLeftArithmeticCB(static_cast<std::uint8_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] == 0x0A); // 0b00001010
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);                 // old bit7 was 1
    assert(cycles == 2);
    std::cout << "test_sla passed\n";
}

void test_sra_preservesSignBit()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x85; // 0b10000101

    std::uint8_t cycles = cpu.shiftRightArithmeticCB(static_cast<std::uint8_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] == 0xC2); // 0b11000010, bit7 preserved
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);                 // old bit0 was 1
    assert(cycles == 2);
    std::cout << "test_sra_preservesSignBit passed\n";
}

void test_srl_zeroesTopBit()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x85; // 0b10000101

    std::uint8_t cycles = cpu.shiftRightLogicalCB(static_cast<std::uint8_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] == 0x42); // 0b01000010, bit7 forced 0
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);
    assert(cycles == 2);
    std::cout << "test_srl_zeroesTopBit passed\n";
}

// ---------- SWAP ----------
void test_swap()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0xA5; // 0b10100101

    std::uint8_t cycles = cpu.swapNibblesCB(static_cast<std::uint8_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] == 0x5A);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == false); // always cleared
    assert(cycles == 2);
    std::cout << "test_swap passed\n";
}

// ---------- BIT / SET / RES ----------
void test_bit_setWhenBitClear()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x00;
    cpu.regs.flag.set(CPU::Registers::Flags::Carry); // confirm untouched

    std::uint8_t cycles = cpu.testBit(3, static_cast<std::uint8_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == true); // bit 3 was 0
    assert(cpu.regs.flag.test(CPU::Registers::Flags::HalfCarry) == true);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);                 // untouched
    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] == 0x00); // unmodified
    assert(cycles == 2);
    std::cout << "test_bit_setWhenBitClear passed\n";
}

void test_bit_hlMemory_costsThreeCycles()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0xC000);
    bus.write(0xC000, 0x08); // bit 3 set

    std::uint8_t cycles = cpu.testBit(3, 6);

    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == false);
    assert(cycles == 3); // NOT 4 -- this is the one CB (HL) op that's cheaper
    std::cout << "test_bit_hlMemory_costsThreeCycles passed\n";
}

void test_setBit_registerOperand()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] = 0x00;

    std::uint8_t cycles = cpu.setBit(5, static_cast<std::uint8_t>(CPU::Registers::REG8::B));

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::B)] == 0x20);
    assert(cycles == 2);
    std::cout << "test_setBit_registerOperand passed\n";
}

void test_clearBit_hlMemoryOperand()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0xC000);
    bus.write(0xC000, 0xFF);

    std::uint8_t cycles = cpu.clearBit(0, 6);

    assert(bus.read(0xC000) == 0xFE);
    assert(cycles == 4);
    std::cout << "test_clearBit_hlMemoryOperand passed\n";
}

// ---------- LD SP,HL / LD HL,SP+e ----------
void test_ldSPToHL()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::HL), 0xBEEF);

    std::uint8_t cycles = cpu.ldSPToHL();

    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::SP)) == 0xBEEF);
    assert(cycles == 2);
    std::cout << "test_ldSPToHL passed\n";
}

void test_ldHLStackPointerPlusOffset_positive()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0x05); // +5
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::SP), 0xFFF8);

    std::uint8_t cycles = cpu.ldHLStackPointerPlusOffset();

    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::HL)) == 0xFFFD);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Zero) == false);
    assert(cycles == 3);
    std::cout << "test_ldHLStackPointerPlusOffset_positive passed\n";
}

// ---------- step(): x=3 paths ----------
void test_step_retNZ_taken()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::SP), 0xFFFC);
    bus.write(0xFFFC, 0x34);
    bus.write(0xFFFD, 0x12);                          // return address 0x1234
    cpu.regs.flag.reset(CPU::Registers::Flags::Zero); // NZ condition true

    bus.write(0x0000, 0xC0); // RET NZ

    std::uint8_t cycles = cpu.step();

    assert(cpu.regs.PC == 0x1234);
    assert(cpu.regs.get16(static_cast<std::size_t>(CPU::Registers::RP::SP)) == 0xFFFE);
    assert(cycles == 5);
    std::cout << "test_step_retNZ_taken passed\n";
}

void test_step_jpNZ_taken()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0xC2); // JP NZ, nn
    bus.write(0x0001, 0x34);
    bus.write(0x0002, 0x12);
    cpu.regs.flag.reset(CPU::Registers::Flags::Zero);

    std::uint8_t cycles = cpu.step();

    assert(cpu.regs.PC == 0x1234);
    assert(cycles == 4);
    std::cout << "test_step_jpNZ_taken passed\n";
}

void test_step_callZ_taken()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::SP), 0xFFFE);
    bus.write(0x0000, 0xCC); // CALL Z, nn
    bus.write(0x0001, 0x34);
    bus.write(0x0002, 0x12);
    cpu.regs.flag.set(CPU::Registers::Flags::Zero);

    std::uint8_t cycles = cpu.step();

    // return address pushed is 0x0003 -- step()'s opcode-fetch increment
    // plus callConditional's own two reads land PC at 0x0003 before the jump
    assert(bus.read(0xFFFD) == 0x00);
    assert(bus.read(0xFFFC) == 0x03);
    assert(cpu.regs.PC == 0x1234);
    assert(cycles == 6);
    std::cout << "test_step_callZ_taken passed\n";
}

void test_step_rst_viaStep()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0150;
    cpu.regs.set16(static_cast<std::size_t>(CPU::Registers::RP::SP), 0xFFFE);
    bus.write(0x0150, 0xC7); // RST 00h

    std::uint8_t cycles = cpu.step();

    // step()'s fetch increment moves PC to 0x0151 before restart() pushes it
    assert(cpu.regs.PC == 0x0000);
    assert(bus.read(0xFFFD) == 0x01);
    assert(bus.read(0xFFFC) == 0x51);
    assert(cycles == 4);
    std::cout << "test_step_rst_viaStep passed\n";
}

void test_step_rlca_viaStep()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] = 0x85;
    bus.write(0x0000, 0x07); // RLCA

    std::uint8_t cycles = cpu.step();

    assert(cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] == 0x0B);
    assert(cpu.regs.flag.test(CPU::Registers::Flags::Carry) == true);
    assert(cpu.regs.PC == 0x0001);
    assert(cycles == 1);
    std::cout << "test_step_rlca_viaStep passed\n";
}

void test_step_di_ei_viaStep()
{
    Bus bus{};
    CPU cpu{bus};
    cpu.regs.PC = 0x0000;
    bus.interrupts.IME = true;
    bus.write(0x0000, 0xF3); // DI
    std::uint8_t cyclesDi = cpu.step();
    assert(bus.interrupts.IME == false);
    assert(cyclesDi == 1);

    cpu.regs.PC = 0x0000;
    bus.write(0x0000, 0xFB); // EI
    std::uint8_t cyclesEi = cpu.step();
    assert(bus.interrupts.IME == true);
    assert(cyclesEi == 1);
    std::cout << "test_step_di_ei_viaStep passed\n";
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

    test_relativeJump_forward();
    test_relativeJump_backward_wraps();
    test_step_jr_unconditional_viaStep();

    test_rlca();
    test_rla();
    test_rrca();
    test_rra();

    test_cpl();
    test_scf();
    test_ccf_flipsFromSetToClear();
    test_ccf_flipsFromClearToSet();
    test_daa_afterAddWithHalfCarry();
    test_daa_afterAddCausingByteOverflow();

    test_rlCarryCB_registerOperand();
    test_rlCarryCB_hlMemoryOperand();
    test_rrCarryCB_registerOperand();
    test_rrCarryCB_hlMemoryOperand();
    test_sla();
    test_sra_preservesSignBit();
    test_srl_zeroesTopBit();
    test_swap();
    test_bit_setWhenBitClear();
    test_bit_hlMemory_costsThreeCycles();
    test_setBit_registerOperand();
    test_clearBit_hlMemoryOperand();
    test_ldSPToHL();
    test_ldHLStackPointerPlusOffset_positive();

    test_step_retNZ_taken();
    test_step_jpNZ_taken();
    test_step_callZ_taken();
    test_step_rst_viaStep();
    test_step_rlca_viaStep();
    test_step_di_ei_viaStep();

    std::cout << "all tests passed\n";
    return 0;
}
