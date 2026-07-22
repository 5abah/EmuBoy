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

    test_incReg_halfCarry();
    test_decReg_wrapsToFF();
    test_incRegPair_noFlagsTouched();

    test_jp_z_taken();
    test_jp_z_notTaken();
    test_jp_nc_taken();

    test_jr_c_forward();
    test_jr_c_backward();
    test_jr_nz_notTaken();

    std::cout << "all tests passed\n";
    return 0;
}
