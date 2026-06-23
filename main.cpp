import std;
import cpu;
import read_rom;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::println("WRONG USAGE. MUST PROVIDE FILEPATH");
        return 1;
    }

    auto rom = readRomFile(argv[1]);

    // purely for testing
    std::println("Destination Code: {}", rom->header.destCode);
    std::println("Old Licensee Code: {}", rom->header.oldLicenseCode);
    std::println("New Licensee Code: {}", rom->header.newLicenseCode);
    std::println("Cartridge type: {}", rom->header.cartType);
}
