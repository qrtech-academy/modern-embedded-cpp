/**
 * @file Tests for Exercise Set 1: debug::log and app::delay_ms, in exercise1/main.cpp.
 *
 *       The functions live in your main.cpp, so this file includes it, with main() renamed so it
 *       does not collide with the test runner's. The program itself is not run: its 100 ms
 *       delay is a billion loop iterations, and the two functions can be tested directly.
 */
#include <string>

#include "qacademy/test/test.hpp"
#include "support/output.hpp"

// Your program, with its main() renamed. A main() may leave out its return statement and any
// other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise1Main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

using support::captureOutput;

/**
 * @brief Exercise 1.1: the message is printed with its log level, as in the example output.
 */
TEST(Logger, PrintsTheMessageAndItsLevel)
{
    EXPECT_OUTPUT(captureOutput([] { debug::log("Sensor failure", 2U); }),
                  "Sensor failure, log level = 2\n");
}

/**
 * @brief Exercise 1.1: leaving the level out logs at level 0.
 */
TEST(Logger, LevelDefaultsToZero)
{
    EXPECT_OUTPUT(captureOutput([] { debug::log("System started"); }),
                  "System started, log level = 0\n");
}

/**
 * @brief Exercise 1.1: debug::log is marked noexcept.
 */
TEST(Logger, IsNoexcept) { EXPECT_TRUE(noexcept(debug::log("message"))); }

/**
 * @brief Exercise 1.2: app::delay_ms is marked noexcept, defaults to 1 ms, and returns.
 */
TEST(Delay, DefaultsToOneMillisecondAndReturns)
{
    EXPECT_TRUE(noexcept(app::delay_ms()));
    app::delay_ms();
    app::delay_ms(2U);
    EXPECT_TRUE(true);
}
