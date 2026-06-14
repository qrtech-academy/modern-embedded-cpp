/**
 * @brief Bit utility function templates.
 */
#include <bitset>
#include <cstdint>
#include <iostream>
#include <type_traits>

namespace
{
/**
 * @brief Clear bit in a register.
 *
 * @tparam T Register type. Must be integral.
 *
 * @param[out] reg Destination register.
 * @param[in] bit Bit to clear.
 */
template<typename T>
constexpr void clear(T& reg, const std::uint8_t bit) noexcept
{
    static_assert(std::is_arithmetic<T>::value, 
        "Cannot perform bit operation with non-integral type!");
    reg &= ~(static_cast<T>(1) << bit);
}

/**
 * @brief Toggle bits in a register.
 *
 * @tparam T Register type. Must be integral.
 * @tparam Bits Parameter pack of bits.
 *
 * @param[out] reg Destination register.
 * @param[in] bits Bits to toggle.
 */
template<typename T, typename... Bits>
constexpr void toggle(T& reg, const Bits... bits) noexcept
{
    static_assert(std::is_arithmetic<T>::value, 
        "Cannot perform bit operation with non-integral type!");
    
    for (const auto bit : {bits...})
    {
        reg ^= (static_cast<T>(1) << bit);
    }
}
} // namespace

/**
 * @brief Demonstrate the bit utility function templates.
 *
 * @return 0 on successful program termination.
 */
int main()
{
    std::uint8_t reg{0xFFU};

    // Clear bit 2 => reg becomes 0b11111011.
    clear(reg, 2U);
    std::cout << "Register content after clearing bit 2: 0b" << std::bitset<8U>(reg) << "\n";

    // Toggle bits 0-3 => reg becomes 0b11110100.
    toggle(reg, 0U, 1U, 2U, 3U);
    std::cout << "Register content after toggling bits 0-3: 0b" << std::bitset<8U>(reg) << "\n";
    return 0;
}