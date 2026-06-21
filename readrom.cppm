export module read_rom;
import std;

export class Rom {
public:
  struct Header {
    std::string_view newLicenseCode{};
  };
  Header header;
};

std::string_view getNewCode(std::byte licenseeByte);
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

  rom.header.newLicenseCode = getNewCode(buffer.at(0x145));

  return rom;
}

// TO-DO: Actually properly read it. Must read like three bytes or something
// bruh
std::string_view getNewCode(std::byte licenseeByte) {
  std::println("{:x}", std::to_integer<std::uint16_t>(licenseeByte));
  switch (std::to_integer<std::uint8_t>(licenseeByte)) {
  case 0x00:
    return "NONE";
  case 0x01:
    return "Nintendo Research & Development 1";
  case 0x08:
    return "Capcom";
  case 0x13:
    return "EA (Electronic Arts)";
  case 0x18:
    return "Hudson Soft";
  case 0x19:
    return "B-AI";
  case 0x20:
    return "KSS";
  case 0x22:
    return "Planning Office WADA";
  case 0x24:
    return "PCM Complete";
  case 0x25:
    return "San-X";
  case 0x28:
    return "Kemco";
  case 0x29:
    return "SETA Corporation";
  case 0x30:
    return "Viacom";
  case 0x31:
    return "Nintendo";
  case 0x32:
    return "Bandai";
  case 0x33:
    return "Ocean Software/Acclaim Entertainment";
  case 0x34:
    return "Konami";
  case 0x35:
    return "HectorSoft";
  case 0x37:
    return "Taito";
  case 0x38:
    return "Hudson Soft";
  case 0x39:
    return "Banpresto";
  case 0x41:
    return "Ubisoft";
  case 0x42:
    return "Atlus";
  case 0x44:
    return "Malibu Interactive";
  case 0x46:
    return "Angel";
  case 0x47:
    return "Bullet-Proof Software";
  case 0x49:
    return "Irem";
  case 0x50:
    return "Absolute";
  case 0x51:
    return "Acclaim Entertainment";
  case 0x52:
    return "Activision";
  case 0x53:
    return "Sammy USA Corporation";
  case 0x54:
    return "Konami";
  case 0x55:
    return "Hi Tech Expressions";
  case 0x56:
    return "LJN";
  case 0x57:
    return "Matchbox";
  case 0x58:
    return "Mattel";
  case 0x59:
    return "Milton Bradley Company";
  case 0x60:
    return "Titus Interactive";
  case 0x61:
    return "Virgin Games Ltd.";
  case 0x64:
    return "Lucasfilm Games";
  case 0x67:
    return "Ocean Software";
  case 0x69:
    return "EA (Electronic Arts)";
  case 0x70:
    return "Infogrames";
  case 0x71:
    return "Interplay Entertainment";
  case 0x72:
    return "Broderbund";
  case 0x73:
    return "Sculptured Software";
  case 0x75:
    return "The Sales Curve Limited";
  case 0x78:
    return "THQ";
  case 0x79:
    return "Accolade";
  case 0x80:
    return "Misawa Entertainment";
  case 0x83:
    return "LOZC G.";
  case 0x86:
    return "Tokuma Shoten";
  case 0x87:
    return "Tsukuda Original";
  case 0x91:
    return "Chunsoft Co.";
  case 0x92:
    return "Video System";
  case 0x93:
    return "Ocean Software/Acclaim Entertainment";
  case 0x95:
    return "Varie";
  case 0x96:
    return "Yonezawa/S'Pal";
  case 0x97:
    return "Kaneko";
  case 0x99:
    return "Pack-In-Video";
  case 0xA4:
    return "Konami (Yu-Gi-Oh!)";
  }
  return "NOTHING FOUND!";
}
