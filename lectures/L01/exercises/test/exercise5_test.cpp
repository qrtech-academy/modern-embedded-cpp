/**
 * @file Tests for Exercise Set 5: the lowestSetBit template, in an anonymous namespace in
 *       exercise5/main.cpp.
 *
 *       The template is tested with the 8-bit register the exercise uses and with 32- and
 *       64-bit ones, for the same reason as in Exercise Set 4: 1U << 40 is undefined for a 32-bit
 *       unsigned. That a non-integral register is rejected at compile time is checked by
 *       exercise5_rejects_float.cpp, which must fail to compile.
 */
#include <cstdint>
#include <optional>
#include <type_traits>

#include "qacademy/test/test.hpp"
#include "support/output.hpp"
#include "support/program.hpp"

// Your program, with its main() renamed. A main() may leave out its return statement and any
// other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise5Main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

namespace
{
/** What bitOf() gives for an empty optional. No register has this many bits. */
constexpr unsigned noBit{0xFFU};

/**
 * @brief Unwrap the result of lowestSetBit() into something the tests can compare and print.
 *
 * @param[in] bit The result of lowestSetBit().
 *
 * @return The bit number, or noBit if the optional is empty.
 */
constexpr unsigned bitOf(const std::optional<std::uint8_t> bit) noexcept
{
    return bit.has_value() ? static_cast<unsigned>(*bit) : noBit;
}
} // namespace

/**
 * @brief Exercise 5.1: the lowest set bit of 0x28 is bit 3, as in the expected output.
 */
TEST(LowestSetBit, FindsTheLowestSetBit)
{
    EXPECT_EQ(bitOf(lowestSetBit(std::uint8_t{0x28U})), 3U);
    EXPECT_EQ(bitOf(lowestSetBit(std::uint8_t{0x80U})), 7U);
    EXPECT_EQ(bitOf(lowestSetBit(std::uint8_t{0xFFU})), 0U);
}

/**
 * @brief Exercise 5.1: a register with no bit set gives std::nullopt.
 */
TEST(LowestSetBit, ReturnsNothingForAnEmptyRegister)
{
    EXPECT_FALSE(lowestSetBit(std::uint8_t{0x00U}).has_value());
    EXPECT_FALSE(lowestSetBit(std::uint32_t{0U}).has_value());
    EXPECT_FALSE(lowestSetBit(std::uint64_t{0U}).has_value());
}

/**
 * @brief Exercise 5.1: bit 0 is a bit that was found, not "nothing". The optional holds 0, and an
 *        optional holding 0 is not an empty optional.
 */
TEST(LowestSetBit, BitZeroIsAValue)
{
    const std::optional<std::uint8_t> bit{lowestSetBit(std::uint8_t{0x01U})};
    EXPECT_TRUE(bit.has_value());
    EXPECT_EQ(bitOf(bit), 0U);
}

/**
 * @brief Exercise 5.1: the template works for any integral register width, up to bit 63.
 */
TEST(LowestSetBit, WorksForWideRegisters)
{
    EXPECT_EQ(bitOf(lowestSetBit(std::uint32_t{0x80000000U})), 31U);
    EXPECT_EQ(bitOf(lowestSetBit(std::uint64_t{0x0000010000000000ULL})), 40U);
    EXPECT_EQ(bitOf(lowestSetBit(std::uint64_t{0x8000000000000000ULL})), 63U);
    EXPECT_EQ(bitOf(lowestSetBit(std::uint64_t{0x8000010000000000ULL})), 40U);
}

/**
 * @brief Exercise 5.1: lowestSetBit() returns a std::optional<std::uint8_t> whatever the register
 *        type, and is noexcept.
 */
TEST(LowestSetBit, ReturnsAnOptionalByteAndIsNoexcept)
{
    EXPECT_TRUE(
        (std::is_same<decltype(lowestSetBit(std::uint8_t{})), std::optional<std::uint8_t>>::value));
    EXPECT_TRUE((
        std::is_same<decltype(lowestSetBit(std::uint64_t{})), std::optional<std::uint8_t>>::value));
    EXPECT_TRUE(noexcept(lowestSetBit(std::uint8_t{})));
}

/**
 * @brief Exercise 5.1: lowestSetBit() is constexpr, so the compiler can find the bit itself. A
 *        lowestSetBit() that is merely correct fails to compile here.
 */
TEST(LowestSetBit, IsUsableAtCompileTime)
{
    static_assert(bitOf(lowestSetBit(std::uint8_t{0x28U})) == 3U,
                  "lowestSetBit() must be usable in a constant expression");
    static_assert(!lowestSetBit(std::uint8_t{0x00U}).has_value(),
                  "lowestSetBit() must be usable in a constant expression");
    EXPECT_TRUE(true);
}

#ifdef PROGRAM
/**
 * @brief Exercise 5.1: the program prints the expected output, and ends normally.
 */
TEST(Program, PrintsTheExpectedOutput)
{
    EXPECT_PROGRAM_OUTPUT("Lowest set bit in value 0x28: 3\n"
                          "No set bit in value 0x0!\n");
}
#endif
