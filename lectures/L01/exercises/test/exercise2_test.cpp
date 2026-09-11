/**
 * @file Tests for Exercise Set 2: the software timer in exercise2/driver/timer.hpp, and, once
 *       exercise2/main.cpp exists too, the program that uses it, which the suite builds and this
 *       file runs.
 *
 *       The timer prints as it goes, so every test that creates one captures the output: both to
 *       keep the report readable and because what it prints is part of the specification.
 */
#include <cstdint>
#include <string>

#include "driver/timer.hpp"
#include "qacademy/test/test.hpp"
#include "support/output.hpp"
#include "support/program.hpp"

using support::captureOutput;

namespace
{
/**
 * @brief Tick a timer a number of times, and count how many of those ticks timed out.
 *
 * @param[in, out] timer The timer to tick.
 * @param[in] ticks How many times to tick it.
 *
 * @return The number of timeouts.
 */
unsigned timeoutsAfter(driver::Timer& timer, const unsigned ticks)
{
    unsigned timeouts{};
    for (unsigned i{}; i < ticks; ++i)
    {
        timer.tick();
        if (timer.hasTimedOut()) { ++timeouts; }
    }
    return timeouts;
}
} // namespace

/**
 * @brief Exercise 2.1 b) and c): a stopped timer prints its creation, then stops and is destroyed.
 */
TEST(Timer, PrintsItsLifetime)
{
    EXPECT_OUTPUT(captureOutput([] { driver::Timer timer{1000U}; }),
                  "Creating timer!\nStopping timer!\nDestroying timer!\n");
}

/**
 * @brief Exercise 2.1 b): a timer created running calls start() from its constructor.
 */
TEST(Timer, StartsWhenCreatedRunning)
{
    EXPECT_OUTPUT(captureOutput([] { driver::Timer timer{1000U, true}; }),
                  "Creating timer!\nStarting timer!\nStopping timer!\nDestroying timer!\n");
}

/**
 * @brief Exercise 2.1 d): the timeout is reported back, from a const timer too.
 */
TEST(Timer, ReportsItsTimeout)
{
    captureOutput(
        []
        {
            const driver::Timer timer{250U};
            EXPECT_EQ(static_cast<unsigned>(timer.timeout_ms()), 250U);
            EXPECT_FALSE(timer.isRunning());
            EXPECT_TRUE(noexcept(timer.timeout_ms()));
            EXPECT_TRUE(noexcept(timer.isRunning()));
        });
}

/**
 * @brief Exercise 2.1 d): start(), stop() and toggle() change the running state, and say so.
 */
TEST(Timer, StartStopAndToggle)
{
    captureOutput(
        []
        {
            driver::Timer timer{1000U};
            EXPECT_OUTPUT(captureOutput([&] { timer.start(); }), "Starting timer!\n");
            EXPECT_TRUE(timer.isRunning());
            EXPECT_OUTPUT(captureOutput([&] { timer.toggle(); }), "Toggling timer: stopped!\n");
            EXPECT_FALSE(timer.isRunning());
            EXPECT_OUTPUT(captureOutput([&] { timer.toggle(); }), "Toggling timer: running!\n");
            EXPECT_TRUE(timer.isRunning());
            EXPECT_OUTPUT(captureOutput([&] { timer.stop(); }), "Stopping timer!\n");
            EXPECT_FALSE(timer.isRunning());
        });
}

/**
 * @brief Exercise 2.1 d): a running timer times out on exactly its timeout-th tick, and not
 *        before.
 */
TEST(Timer, TimesOutAfterExactlyItsTimeout)
{
    captureOutput(
        []
        {
            driver::Timer timer{1000U, true};
            EXPECT_EQ(timeoutsAfter(timer, 999U), 0U);
            timer.tick();
            EXPECT_TRUE(timer.hasTimedOut());
            EXPECT_TRUE(noexcept(timer.hasTimedOut()));
        });
}

/**
 * @brief Exercise 2.1 d): hasTimedOut() resets the counter, so the next timeout is a whole
 *        timeout later rather than on every tick after the first.
 */
TEST(Timer, ResetsTheCounterOnTimeout)
{
    captureOutput(
        []
        {
            driver::Timer timer{100U, true};
            EXPECT_EQ(timeoutsAfter(timer, 500U), 5U);
            EXPECT_EQ(timeoutsAfter(timer, 99U), 0U);
        });
}

/**
 * @brief Exercise 2.1 d): a stopped timer does not count.
 */
TEST(Timer, DoesNotCountWhileStopped)
{
    captureOutput(
        []
        {
            driver::Timer timer{100U};
            EXPECT_EQ(timeoutsAfter(timer, 1000U), 0U);
            timer.start();
            EXPECT_EQ(timeoutsAfter(timer, 100U), 1U);
        });
}

#ifdef PROGRAM

/**
 * @brief Exercise 2.1 e): the program prints exactly the example output, five timeouts in 5000
 *        ticks of a 1000 ms timer, and ends normally.
 */
TEST(Program, PrintsTheExampleOutput)
{
    EXPECT_PROGRAM_OUTPUT("Creating timer!\n"
                          "Starting timer!\n"
                          "Timeout after 1000 ms!\n"
                          "Timeout after 1000 ms!\n"
                          "Timeout after 1000 ms!\n"
                          "Timeout after 1000 ms!\n"
                          "Timeout after 1000 ms!\n"
                          "Stopping timer!\n"
                          "Destroying timer!\n");
}

#endif
