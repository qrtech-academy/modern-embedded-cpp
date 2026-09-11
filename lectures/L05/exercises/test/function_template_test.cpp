/**
 * @file Tests for Exercise Set 1: the clear and toggle function templates, in
 *       function_template/main.cpp.
 *
 *       The templates are tested with the 8-bit register the exercise uses and with 16-, 32- and
 *       64-bit ones, because a template is only as good as its widest instantiation: 1 << 40 is
 *       undefined for an int, and static_cast<T>(1) << 40 is what gets it right. That a
 *       non-integral register is rejected at compile time is checked by the two
 *       function_template_rejects_float_*.cpp files, which must fail to compile.
 */
#include <cstdint>
#include <string>

#include "qacademy/test/test.hpp"
#include "support/output.hpp"
#include "support/program.hpp"

// Your program, with its main() renamed, for the templates in it; the program itself is built by
// the suite and run as a separate process. A main() may leave out its return statement and any
// other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main functionTemplateMain
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

using support::captureOutput;

namespace
{
/**
 * @brief Clear and toggle a register at compile time, which only constexpr templates can do.
 *
 * @return The register after clearing bit 2 of 0xFF and toggling bits 0 to 3.
 */
constexpr std::uint8_t atCompileTime()
{
    std::uint8_t reg{0xFFU};
    clear(reg, 2U);
    toggle(reg, 0U, 1U, 2U, 3U);
    return reg;
}
} // namespace

/**
 * @brief Exercise 1.1 b): clearing bit 2 of 0xFF gives 0b11111011, as in the expected output.
 */
TEST(Clear, ClearsTheBit)
{
    std::uint8_t reg{0xFFU};
    clear(reg, 2U);
    EXPECT_EQ(static_cast<unsigned>(reg), 0xFBU);
}

/**
 * @brief Exercise 1.1 b): clearing a bit touches no other bit, and a clear bit stays clear.
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
 * @brief Exercise 1.1: the template works for every integral register width, up to bit 63.
 */
TEST(Clear, WorksForEveryWidth)
{
    std::uint16_t half{0xFFFFU};
    clear(half, 15U);
    EXPECT_EQ(static_cast<unsigned>(half), 0x7FFFU);

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
 * @brief Exercise 1.1: clear() is noexcept, as its declaration says.
 */
TEST(Clear, IsNoexcept)
{
    std::uint8_t reg{};
    EXPECT_TRUE(noexcept(clear(reg, 0U)));
}

/**
 * @brief Exercise 1.2 d): toggling bits 0 to 3 of 0b11111011 gives 0b11110100, as in the expected
 *        output.
 */
TEST(Toggle, TogglesEveryBitInThePack)
{
    std::uint8_t reg{0xFBU};
    toggle(reg, 0U, 1U, 2U, 3U);
    EXPECT_EQ(static_cast<unsigned>(reg), 0xF4U);
}

/**
 * @brief Exercise 1.2 d): toggling the same bits twice restores the register, and one bit is a
 *        pack of one.
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
 * @brief Exercise 1.2: the template works for every integral register width, up to bit 63.
 */
TEST(Toggle, WorksForEveryWidth)
{
    std::uint16_t half{};
    toggle(half, 0U, 15U);
    EXPECT_EQ(static_cast<unsigned>(half), 0x8001U);

    std::uint32_t word{};
    toggle(word, 31U);
    EXPECT_EQ(word, 0x80000000U);

    std::uint64_t wide{};
    toggle(wide, 0U, 32U, 63U);
    EXPECT_EQ(wide, 0x8000000100000001ULL);
}

/**
 * @brief Exercise 1.2: toggle() is noexcept, as its declaration says.
 */
TEST(Toggle, IsNoexcept)
{
    std::uint64_t wide{};
    EXPECT_TRUE(noexcept(toggle(wide, 1U, 2U)));
}

/**
 * @brief Exercises 1.1 and 1.2: both templates are constexpr, so they can run at compile time.
 */
TEST(Templates, AreConstexpr)
{
    static_assert(atCompileTime() == 0xF4U, "clear() and toggle() must be usable at compile time");
    EXPECT_EQ(static_cast<unsigned>(atCompileTime()), 0xF4U);
}

#ifdef PROGRAM
/**
 * @brief Exercise 1.2 e): the program prints the expected output.
 */
TEST(Program, PrintsTheExpectedOutput)
{
    EXPECT_PROGRAM_OUTPUT("Register content after clearing bit 2: 0b11111011\n"
                          "Register content after toggling bits 0-3: 0b11110100\n");
}
#endif
