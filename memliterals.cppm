export module memory_literals;
import std;

export namespace MemLiteral
{
constexpr std::size_t operator""_KiB(unsigned long long size)
{
    return 1024ULL * size;
}

constexpr std::size_t operator""_MiB(unsigned long long int x)
{
    return 1024_KiB * x;
}

} // namespace MemLiteral
