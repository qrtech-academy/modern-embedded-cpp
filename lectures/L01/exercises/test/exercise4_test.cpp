/**
 * @file Tests for Exercise Set 4: the clear and toggle templates, in an anonymous namespace in
 *       exercise4/main.cpp.
 *
 *       The templates are tested with the 8-bit register the exercise uses and with 32- and
 *       64-bit ones, because a template is only as good as its widest instantiation: 1U << 40
 *       is undefined for a 32-bit unsigned, and static_cast<T>(1U) << 40 is what gets it right.
 *       That a non-integral register is rejected at compile time is checked by
 *       exercise4_rejects_float.cpp, which must fail to compile.
 */
#include <cstdint>
#include <string>

#include "qacademy/test/test.hpp"
#include "support/output.hpp"
#include "support/program.hpp"

// Your program, with its main() renamed. A main() may leave out its return statement and any
// other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise4Main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

/**
 * @brief Exercise 4.1: clearing bit 2 of 0xFF gives 0b11111011, as in the expected output.
 */
TEST(Clear, ClearsTheBit)
{
    std::uint8_t reg{0xFFU};
    clear(reg, 2U);
    EXPECT_EQ(static_cast<unsigned>(reg), 0xFBU);
}

/**
 * @brief Exercise 4.1: clearing a bit that is already clear, or any bit, touches no other bit.
 */
TEST(Clear, LeavesTheOtherBitsAlone)
{
    std::uint8_t reg{0x0FU};
    clear(reg, 7U);
    EXPECT_EQ(static_cast<unsigned>(reg), 0x0FU);
    clear(reg, 0U);
    EXPECT_EQ(static_cast<unsigned>(reg), 0x0EU);
}

/**
 * @brief Exercise 4.1: the template works for any integral register width, up to bit 63.
 */
TEST(Clear, WorksForWideRegisters)
{
    std::uint32_t word{0xFFFFFFFFU};
    clear(word, 31U);
    EXPECT_EQ(word, 0x7FFFFFFFU);

    std::uint64_t wide{0xFFFFFFFFFFFFFFFFULL};
    clear(wide, 40U);
    EXPECT_EQ(wide, 0xFFFFFEFFFFFFFFFFULL);
    clear(wide, 63U);
    EXPECT_EQ(wide, 0x7FFFFEFFFFFFFFFFULL);
}

/**
 * @brief Exercise 4.1: clear() is noexcept.
 */
TEST(Clear, IsNoexcept)
{
    std::uint8_t reg{};
    EXPECT_TRUE(noexcept(clear(reg, 0U)));
}

/**
 * @brief Exercise 4.2: toggling bits 0, 2, 4 and 6 of 0b11111011 gives 0b10101110.
 */
TEST(Toggle, TogglesEveryBitInThePack)
{
    std::uint8_t reg{0xFBU};
    toggle(reg, 0U, 2U, 4U, 6U);
    EXPECT_EQ(static_cast<unsigned>(reg), 0xAEU);
}

/**
 * @brief Exercise 4.2: toggling the same bits twice restores the register, and a single bit
 *        is a pack of one.
 */
TEST(Toggle, TogglingTwiceIsNoChange)
{
    std::uint8_t reg{0x5AU};
    toggle(reg, 1U, 3U, 7U);
    toggle(reg, 1U, 3U, 7U);
    EXPECT_EQ(static_cast<unsigned>(reg), 0x5AU);
    toggle(reg, 0U);
    EXPECT_EQ(static_cast<unsigned>(reg), 0x5BU);
}

/**
 * @brief Exercise 4.2: the template works for wide registers, and is noexcept.
 */
TEST(Toggle, WorksForWideRegisters)
{
    std::uint64_t wide{};
    toggle(wide, 0U, 32U, 63U);
    EXPECT_EQ(wide, 0x8000000100000001ULL);
    EXPECT_TRUE(noexcept(toggle(wide, 1U, 2U)));
}

#ifdef PROGRAM
/**
 * @brief Exercise 4.2: the program prints the expected output, and ends normally.
 */
TEST(Program, PrintsTheExpectedOutput) { EXPECT_PROGRAM_OUTPUT("Register content: 10101110\n"); }
#endif
