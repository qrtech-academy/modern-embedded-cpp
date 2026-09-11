/**
 * @file Solution for exercise set 4.
 */
#include <bitset>
#include <cstdint>
#include <iostream>
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
 * @brief Toggle one or more bits in the given register.
 *
 * @tparam T The register type. Must be integral.
 * @tparam Bits Parameter pack of bits.
 *
 * @param[in, out] reg Register to modify.
 * @param[in] bits Bits to toggle.
 */
template<typename T, typename... Bits>
constexpr void toggle(T& reg, const Bits... bits) noexcept
{
    static_assert(std::is_integral<T>::value,
                  "Cannot perform bit operation with non-integral type!");
    for (const auto bit : {bits...})
    {
        reg ^= (static_cast<T>(1U) << bit);
    }
}
} // namespace

/**
 * @brief Perform bit manipulations with an 8-bit register.
 *
 * @return Exit status (0 = success).
 */
int main()
{
    std::uint8_t reg{0xFFU};
    clear(reg, 2U);
    toggle(reg, 0U, 2U, 4U, 6U);
    std::cout << "Register content: " << std::bitset<8U>(reg) << "\n";
    return 0;
}
