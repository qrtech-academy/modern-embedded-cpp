/**
 * @file Tests for Exercise Set 2: incrementCounter() on an atomic counter, in exercise2.cpp.
 *
 *       The file evolves through the set, and the tests check where it ends up: Exercise 2.3's
 *       atomic counter, whose final value is guaranteed. Exercise 2.1's unsynchronized version is
 *       deliberately not tested: a data race is undefined behaviour, and the one thing a test
 *       could assert about it, that updates are lost, is exactly what a race does not promise.
 */
#include <atomic>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <vector>

#include "qacademy/test/test.hpp"
#include "support/output.hpp"

// Your program, with its main() renamed. A main() may leave out its return statement and any
// other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise2Main
#include "exercise2.cpp"
#undef main
#pragma GCC diagnostic pop

using support::captureOutput;

/**
 * @brief Exercise 2.3: one call increments the counter once per iteration, and is noexcept.
 */
TEST(Counter, CountsEveryIteration)
{
    std::atomic<std::uint32_t> counter{};
    incrementCounter(counter, 1000U);
    EXPECT_EQ(counter.load(), 1000U);
    incrementCounter(counter, 0U);
    EXPECT_EQ(counter.load(), 1000U);
    EXPECT_TRUE(noexcept(incrementCounter(counter, 1U)));
}

/**
 * @brief Exercise 2.3: two threads of 100000 increments each end at exactly 200000, every time.
 */
TEST(Counter, TwoThreadsCountExactly)
{
    std::atomic<std::uint32_t> counter{};
    std::thread t1{incrementCounter, std::ref(counter), 100000U};
    std::thread t2{incrementCounter, std::ref(counter), 100000U};
    t1.join();
    t2.join();
    EXPECT_EQ(counter.load(), 200000U);
}

/**
 * @brief Exercise 2.3: eight threads lose no increment either.
 */
TEST(Counter, ManyThreadsCountExactly)
{
    std::atomic<std::uint32_t> counter{};
    std::vector<std::thread> threads{};
    for (int i{}; i < 8; ++i)
    {
        threads.emplace_back(incrementCounter, std::ref(counter), 25000U);
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
    EXPECT_EQ(counter.load(), 200000U);
}

/**
 * @brief Exercise 2.3: the program prints the final value, which is 200000.
 */
TEST(Program, PrintsTheFinalValue)
{
    const std::string output{captureOutput([] { exercise2Main(); })};
    const bool printedTheFinalValue{std::string::npos != output.find("200000")};
    EXPECT_TRUE(printedTheFinalValue);
}
