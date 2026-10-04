/**
 * @file Solution for exercise set 5.
 */
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <optional>
#include <type_traits>

namespace
{
/**
 * @brief Clear bit in the given register.
 *
 * @tparam T The register type. Must be integral.
 *
 * @param[in, out] reg Register to modify.
 * @param[in] bit Bit to clear.
 */
template<typename T>
constexpr void clear(T& reg, std::uint8_t bit) noexcept
{
    static_assert(std::is_integral<T>::value,
                  "Cannot perform bit operation with non-integral type!");
    reg &= ~(static_cast<T>(1U) << bit);
}

/**
 * @brief Find the lowest set bit in the given register.
 *
 * @tparam T The register type. Must be integral.
 *
 * @param[in] reg Register to read.
 *
 * @return The lowest set bit, or std::nullopt if not bit is set.
 */
template<typename T>
[[nodiscard]] constexpr std::optional<std::uint8_t> lowestSetBit(const T reg) noexcept
{
    static_assert(std::is_integral<T>::value,
                  "Cannot perform bit operation with non-integral type!");
    constexpr std::size_t bitsPerByte{8U};
    constexpr std::size_t bits{sizeof(T) * bitsPerByte};
    constexpr T one{static_cast<T>(1U)};

    for (std::size_t i{}; i < bits; ++i)
    {
        const auto bitMask = static_cast<T>(one << i);
        const auto bit     = static_cast<bool>(reg & bitMask);
        if (bit) { return i; }
    }
    return std::nullopt;
}

/**
 * @brief Find the lowest set bit in the given register.
 *
 * @tparam T The register type. Must be integral.
 *
 * @param[in] reg Register to read.
 */
template<typename T>
void printLowestSetBit(const T reg) noexcept
{
    static_assert(std::is_integral<T>::value,
                  "Cannot perform bit operation with non-integral type!");
    const auto bit    = lowestSetBit(reg);
    const auto regVal = static_cast<int>(reg);

    if (std::nullopt != bit)
    {
        const auto bitVal = static_cast<int>(*bit);
        std::cout << "Lowest set bit in value 0x" << std::hex << regVal << ": " << bitVal << "\n";
    }
    else { std::cout << "No set bit in value 0x" << std::hex << regVal << "!\n"; }
}
} // namespace

/**
 * @brief Perform x
 *
 * @return Exit status (0 = success).
 */
int main()
{
    const std::uint8_t reg1{0x28U};
    const std::uint8_t reg2{0x00U};

    printLowestSetBit(reg1);
    printLowestSetBit(reg2);
    return 0;
}
