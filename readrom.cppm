export module read_rom;
import std;

export class Rom {
public:
  struct Header {
    std::string_view newLicenseCode{};
  };
  Header header;
};

std::string_view getNewCode(std::uint16_t licenseeByte);
void parseRom(Rom &rom);

export std::expected<Rom, std::string_view>
readRomFile(std::string_view filePath) {
  Rom rom{};

  std::ifstream romFile(filePath.data(), std::ios::binary);
  if (!romFile.is_open()) {
    return std::unexpected<std::string_view>("ERROR OPENING FILE");
  }

  std::filesystem::path romFilePath{filePath};
  const auto romFileLen = std::filesystem::file_size(romFilePath);
  if (romFileLen == 0) {
    return std::unexpected<std::string_view>("FILE IS EMPTY");
  }

  std::vector<std::byte> buffer(romFileLen);

  romFile.read(reinterpret_cast<char *>(buffer.data()), romFileLen);

  romFile.close();

  rom.header.newLicenseCode = getNewCode(static_cast<std::uint16_t>(
      (static_cast<std::uint16_t>(buffer.at(0x144)) << 8) |
      static_cast<std::uint16_t>(buffer.at(0x145))));

  return rom;
}

std::string_view getNewCode(std::uint16_t licenseeByte) {

  switch (licenseeByte) {
  case static_cast<std::uint16_t>(('0' << 8) | '0'):
    return "NONE";

  case static_cast<std::uint16_t>(('0' << 8) | '1'):
    return "Nintendo Research & Development 1";
  case static_cast<std::uint16_t>(('0' << 8) | '8'):
    return "Capcom";
  case static_cast<std::uint16_t>(('1' << 8) | '3'):
    return "EA (Electronic Arts)";
  case static_cast<std::uint16_t>(('1' << 8) | '8'):
    return "Hudson Soft";
  case static_cast<std::uint16_t>(('1' << 8) | '9'):
    return "B-AI";
  case static_cast<std::uint16_t>(('2' << 8) | '0'):
    return "KSS";
  case static_cast<std::uint16_t>(('2' << 8) | '2'):
    return "Planning Office WADA";
  case static_cast<std::uint16_t>(('2' << 8) | '4'):
    return "PCM Complete";
  case static_cast<std::uint16_t>(('2' << 8) | '5'):
    return "San-X";
  case static_cast<std::uint16_t>(('2' << 8) | '8'):
    return "Kemco";
  case static_cast<std::uint16_t>(('2' << 8) | '9'):
    return "SETA Corporation";
  case static_cast<std::uint16_t>(('3' << 8) | '0'):
    return "Viacom";
  case static_cast<std::uint16_t>(('3' << 8) | '1'):
    return "Nintendo";
  case static_cast<std::uint16_t>(('3' << 8) | '2'):
    return "Bandai";
  case static_cast<std::uint16_t>(('3' << 8) | '3'):
    return "Ocean Software/Acclaim Entertainment";
  case static_cast<std::uint16_t>(('3' << 8) | '4'):
    return "Konami";
  case static_cast<std::uint16_t>(('3' << 8) | '5'):
    return "HectorSoft";
  case static_cast<std::uint16_t>(('3' << 8) | '7'):
    return "Taito";
  case static_cast<std::uint16_t>(('3' << 8) | '8'):
    return "Hudson Soft";
  case static_cast<std::uint16_t>(('3' << 8) | '9'):
    return "Banpresto";
  case static_cast<std::uint16_t>(('4' << 8) | '1'):
    return "Ubi Soft";
  case static_cast<std::uint16_t>(('4' << 8) | '2'):
    return "Atlus";
  case static_cast<std::uint16_t>(('4' << 8) | '4'):
    return "Malibu Interactive";
  case static_cast<std::uint16_t>(('4' << 8) | '6'):
    return "Angel";
  case static_cast<std::uint16_t>(('4' << 8) | '7'):
    return "Bullet-Proof Software";
  case static_cast<std::uint16_t>(('4' << 8) | '9'):
    return "Irem";

  case static_cast<std::uint16_t>(('5' << 8) | '0'):
    return "Absolute";
  case static_cast<std::uint16_t>(('5' << 8) | '1'):
    return "Acclaim Entertainment";
  case static_cast<std::uint16_t>(('5' << 8) | '2'):
    return "Activision";
  case static_cast<std::uint16_t>(('5' << 8) | '3'):
    return "Sammy USA Corporation";
  case static_cast<std::uint16_t>(('5' << 8) | '4'):
    return "Konami";
  case static_cast<std::uint16_t>(('5' << 8) | '5'):
    return "Hi Tech Expressions";
  case static_cast<std::uint16_t>(('5' << 8) | '6'):
    return "LJN";
  case static_cast<std::uint16_t>(('5' << 8) | '7'):
    return "Matchbox";
  case static_cast<std::uint16_t>(('5' << 8) | '8'):
    return "Mattel";
  case static_cast<std::uint16_t>(('5' << 8) | '9'):
    return "Milton Bradley Company";

  case static_cast<std::uint16_t>(('6' << 8) | '0'):
    return "Titus Interactive";
  case static_cast<std::uint16_t>(('6' << 8) | '1'):
    return "Virgin Games Ltd.";
  case static_cast<std::uint16_t>(('6' << 8) | '4'):
    return "Lucasfilm Games";
  case static_cast<std::uint16_t>(('6' << 8) | '7'):
    return "Ocean Software";
  case static_cast<std::uint16_t>(('6' << 8) | '9'):
    return "EA (Electronic Arts)";

  case static_cast<std::uint16_t>(('7' << 8) | '0'):
    return "Infogrames";
  case static_cast<std::uint16_t>(('7' << 8) | '1'):
    return "Interplay Entertainment";
  case static_cast<std::uint16_t>(('7' << 8) | '2'):
    return "Broderbund";
  case static_cast<std::uint16_t>(('7' << 8) | '3'):
    return "Sculptured Software";
  case static_cast<std::uint16_t>(('7' << 8) | '5'):
    return "The Sales Curve Limited";
  case static_cast<std::uint16_t>(('7' << 8) | '8'):
    return "THQ";
  case static_cast<std::uint16_t>(('7' << 8) | '9'):
    return "Accolade";

  case static_cast<std::uint16_t>(('8' << 8) | '0'):
    return "Misawa Entertainment";
  case static_cast<std::uint16_t>(('8' << 8) | '3'):
    return "LOZC G.";
  case static_cast<std::uint16_t>(('8' << 8) | '6'):
    return "Tokuma Shoten";
  case static_cast<std::uint16_t>(('8' << 8) | '7'):
    return "Tsukuda Original";

  case static_cast<std::uint16_t>(('9' << 8) | '1'):
    return "Chunsoft Co.";
  case static_cast<std::uint16_t>(('9' << 8) | '2'):
    return "Video System";
  case static_cast<std::uint16_t>(('9' << 8) | '3'):
    return "Ocean Software/Acclaim Entertainment";
  case static_cast<std::uint16_t>(('9' << 8) | '5'):
    return "Varie";
  case static_cast<std::uint16_t>(('9' << 8) | '6'):
    return "Yonezawa/S'Pal";
  case static_cast<std::uint16_t>(('9' << 8) | '7'):
    return "Kaneko";
  case static_cast<std::uint16_t>(('9' << 8) | '9'):
    return "Pack-In-Video";
  case static_cast<std::uint16_t>(('9' << 8) | 'H'):
    return "Bottom Up";

  case static_cast<std::uint16_t>(('A' << 8) | '4'):
    return "Konami (Yu-Gi-Oh!)";

  case static_cast<std::uint16_t>(('B' << 8) | 'L'):
    return "MTO";

  case static_cast<std::uint16_t>(('D' << 8) | 'K'):
    return "Kodansha";
  }
  return "NOTHING FOUND!";
}
