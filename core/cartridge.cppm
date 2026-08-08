// cartridge
export module cartridge;
import std;
import mbc;

export class Cartridge
{
  public:
    static Cartridge loadFromFile(std::filesystem::path path)
    {
        auto length = std::filesystem::file_size(path);
        if (length == 0)
        {
            return {};
        }

        Cartridge cart;
        cart.romData.resize(length);

        std::ifstream file(path, std::ios_base::binary);
        file.read(reinterpret_cast<char *>(cart.romData.data()), length);
        file.close();

        for (std::size_t i = 0x134; i <= 0x143; ++i)
        {
            cart.cartTitle += cart.romData[i];
        }

        cart.mbc = createMBC(cart.romData[0x147], cart.romData, 0);

        return cart;
    };

    std::uint8_t read(std::uint16_t addr) const
    {
        return mbc->read(addr);
    };

    void write(std::uint16_t addr, std::uint8_t val)
    {
        mbc->write(addr, val);
    };

    std::string title() const
    {
        return cartTitle;
    };

  private:
    std::vector<std::uint8_t> romData;
    std::unique_ptr<MBC> mbc;
    std::string cartTitle;
};
