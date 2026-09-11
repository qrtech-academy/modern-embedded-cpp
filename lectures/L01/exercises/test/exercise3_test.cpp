/**
 * @file Tests for Exercise Set 3: swap, in an anonymous namespace in exercise3/main.cpp.
 *
 *       An anonymous namespace is visible only inside its own file, so this file includes yours,
 *       with main() renamed, which makes swap() callable here. The program itself is built by the
 *       suite and run as a separate process.
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
#define main exercise3Main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

namespace
{
/**
 * @brief Swap two values at compile time, which only a constexpr swap() can do.
 *
 * @return The first value after the swap.
 */
constexpr std::uint32_t swappedAtCompileTime()
{
    std::uint32_t a{3U}, b{10U};
    swap(a, b);
    return a;
}
} // namespace

/**
 * @brief Exercise 3.1: the two variables exchange values.
 */
TEST(Swap, ExchangesTheValues)
{
    std::uint32_t a{3U}, b{10U};
    swap(a, b);
    EXPECT_EQ(a, 10U);
    EXPECT_EQ(b, 3U);
}

/**
 * @brief Exercise 3.1: swapping twice restores the originals, and equal values stay equal.
 */
TEST(Swap, SwappingTwiceIsNoChange)
{
    std::uint32_t a{0xFFFFFFFFU}, b{0U};
    swap(a, b);
    swap(a, b);
    EXPECT_EQ(a, 0xFFFFFFFFU);
    EXPECT_EQ(b, 0U);

    std::uint32_t c{7U}, d{7U};
    swap(c, d);
    EXPECT_EQ(c, 7U);
    EXPECT_EQ(d, 7U);
}

/**
 * @brief Exercise 3.1: swap() is constexpr and noexcept, as its declaration says.
 */
TEST(Swap, IsConstexprAndNoexcept)
{
    static_assert(swappedAtCompileTime() == 10U, "swap() must be usable at compile time");
    std::uint32_t a{}, b{};
    EXPECT_TRUE(noexcept(swap(a, b)));
}

#ifdef PROGRAM
/**
 * @brief Exercise 3.1: the program prints the example output, and ends normally.
 */
TEST(Program, PrintsTheExampleOutput)
{
    EXPECT_PROGRAM_OUTPUT("Before swap: a = 3, b = 10\nAfter swap: a = 10, b = 3\n");
}
#endif
