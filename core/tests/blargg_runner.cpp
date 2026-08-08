import cpu;
import bus;
import std;

int main()
{
    Bus bus{};
    bus.loadROM("01-special.gb");
    CPU cpu(bus);

    std::string serialOutput;
    constexpr std::size_t maxSteps = 50'000'000;
    std::size_t traceCount = 0;
    constexpr std::size_t maxTraceLines = 500;

    for (std::size_t step = 0; step < maxSteps; ++step)
    {
        // Scoped trace: only print while PC is in the region we're investigating,
        // and only for the first maxTraceLines hits so it doesn't flood forever.
        if (cpu.regs.PC >= 0xC000 && cpu.regs.PC <= 0xC020 && traceCount < maxTraceLines)
        {
            std::uint8_t opcodeAtPC = bus.read(cpu.regs.PC);
            std::cout << std::hex << "PC=" << cpu.regs.PC << " opcode=" << +opcodeAtPC
                      << " A=" << +cpu.regs.reg8[static_cast<std::size_t>(CPU::Registers::REG8::A)] << std::dec << '\n';
            ++traceCount;
        }

        cpu.step();

        // Blargg test ROMs output a character by writing:
        //   SB ($FF01) = character
        //   SC ($FF02) = 0x81
        if (bus.read(0xFF02) == 0x81)
        {
            std::uint8_t rawByte = bus.read(0xFF01);
            serialOutput += static_cast<char>(rawByte);

            // Print both the raw hex value and the character, so a garbled
            // byte shows up as a real number instead of unreadable terminal
            // junk
            std::cout << "[0x" << std::hex << static_cast<int>(rawByte) << std::dec << " '" << serialOutput.back()
                      << "'] " << std::flush;

            // Acknowledge the transfer so we don't read it again.
            bus.write(0xFF02, 0x00);
        }

        if (serialOutput.contains("Passed"))
        {
            std::cout << "\n\n=== TEST PASSED ===\n";
            std::cout << "Instructions executed: " << step << '\n';
            return 0;
        }
        if (serialOutput.contains("Failed"))
        {
            std::cout << "\n\n=== TEST FAILED ===\n";
            std::cout << "Instructions executed: " << step << '\n';
            std::cout << "Output:\n" << serialOutput << '\n';
            return 1;
        }
    }

    std::cout << "\n\n=== TEST TIMED OUT ===\n";
    std::cout << "Instructions executed: " << maxSteps << '\n';
    std::cout << "PC: 0x" << std::hex << std::uppercase << cpu.regs.PC << std::dec << '\n';
    std::cout << "Output so far:\n" << serialOutput << '\n';
    return 2;
}
