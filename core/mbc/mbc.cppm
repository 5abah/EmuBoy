// mbc.cppm
export module mbc;
import std;

export class MBC
{
  public:
    virtual ~MBC() = default;
    virtual std::uint8_t read(std::uint16_t addr) const = 0;
    virtual void write(std::uint16_t addr, std::uint8_t val) = 0;
};

export std::unique_ptr<MBC> createMBC(std::uint8_t cartridgeTypeByte, std::vector<std::uint8_t> rom,
                                      std::size_t ramSize);

export class MBC0 : public MBC
{
  public:
    explicit MBC0(std::vector<std::uint8_t> rom);
    std::uint8_t read(std::uint16_t addr) const override;
    void write(std::uint16_t addr, std::uint8_t val) override;

  private:
    std::vector<std::uint8_t> romData{};
};

export class MBC1 : public MBC
{
  public:
    explicit MBC1(std::vector<std::uint8_t> rom, std::size_t ramSize);
    std::uint8_t read(std::uint16_t addr) const override;
    void write(std::uint16_t addr, std::uint8_t val) override;

  private:
    std::vector<std::uint8_t> romData{};
    std::vector<std::uint8_t> ramData{};
    bool ramEnabled{};
    std::uint8_t romBankLow5{1};
    std::uint8_t bank2{};
    bool advancedBankingMode{};

    std::size_t effectiveRomBank() const;
};
