import std;
import cpu;
import read_rom;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::println("WRONG USAGE. MUST PROVIDE FILEPATH");
    return 1;
  }

  auto rom = readRomFile(argv[1]);
  std::println("{}", rom->header.newLicenseCode);
}
