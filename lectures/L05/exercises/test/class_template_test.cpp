/**
 * @file Tests for Exercise Set 3: the Timer class template and its STM32 specialization in
 *       class_template/include/driver/timer/timer.hpp, and, once class_template/source/main.cpp
 *       exists too, the program that uses them.
 *
 *       Every behaviour is tested on both Timer<Type::Stub> and Timer<Type::Stm32>, and the two
 *       are told apart by what they print. That is what proves the specialization is really used:
 *       without it, Timer<Type::Stm32> would be the primary template and print "stub".
 *
 *       Each timer lives in a std::optional, so that its construction and its destruction can each
 *       be captured on their own, and what the timer prints at every step compared exactly.
 */
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <thread>
#include <type_traits>

#include "driver/timer/timer.hpp"
#include "qacademy/test/test.hpp"
#include "support/output.hpp"
#include "support/program.hpp"

using driver::timer::Timer;
using driver::timer::Type;
using support::captureOutput;

namespace
{
/**
 * @brief Tick a timer a number of times, and count how many of those ticks timed out.
 *
 * @tparam T Timer type.
 *
 * @param[in, out] timer The timer to tick.
 * @param[in] ticks How many times to tick it.
 *
 * @return The number of timeouts.
 */
template<Type T>
unsigned timeoutsAfter(Timer<T>& timer, const unsigned ticks)
{
    unsigned timeouts{};
    for (unsigned i{}; i < ticks; ++i)
    {
        timer.tick();
        if (timer.hasTimedOut()) { ++timeouts; }
    }
    return timeouts;
}

/**
 * @brief Check whether a text ends with a given suffix.
 *
 * @param[in] text The text.
 * @param[in] suffix The ending to look for.
 *
 * @return True if text ends with suffix, false otherwise.
 */
bool endsWith(const std::string& text, const std::string& suffix)
{
    return (text.size() >= suffix.size()) &&
           (0 == text.compare(text.size() - suffix.size(), suffix.size(), suffix));
}

/**
 * @brief Check the class-level properties Part I b), c) and f) ask for.
 *
 * @tparam T Timer type.
 */
template<Type T>
void expectDeclaredAsSpecified()
{
    using TimerT = Timer<T>;
    EXPECT_TRUE(std::is_final<TimerT>::value);
    EXPECT_FALSE((std::is_convertible<std::uint16_t, TimerT>::value));
    EXPECT_TRUE((std::is_nothrow_constructible<TimerT, std::uint16_t>::value));
    EXPECT_TRUE(std::is_nothrow_destructible<TimerT>::value);
    EXPECT_FALSE(std::is_default_constructible<TimerT>::value);
    EXPECT_FALSE(std::is_copy_constructible<TimerT>::value);
    EXPECT_FALSE(std::is_move_constructible<TimerT>::value);
    EXPECT_FALSE(std::is_copy_assignable<TimerT>::value);
    EXPECT_FALSE(std::is_move_assignable<TimerT>::value);
}

/**
 * @brief Check that the methods Part I e) lists are noexcept, and the queries const.
 *
 * @tparam T Timer type.
 */
template<Type T>
void expectMethodsAsSpecified()
{
    captureOutput(
        []
        {
            Timer<T> timer{10U};
            const Timer<T>& readOnly{timer};
            EXPECT_TRUE(noexcept(readOnly.timeout_ms()));
            EXPECT_TRUE(noexcept(readOnly.isRunning()));
            EXPECT_TRUE(noexcept(readOnly.isInitialized()));
            EXPECT_TRUE(noexcept(timer.start()));
            EXPECT_TRUE(noexcept(timer.stop()));
            EXPECT_TRUE(noexcept(timer.toggle()));
            EXPECT_TRUE(noexcept(timer.tick()));
            EXPECT_TRUE(noexcept(timer.hasTimedOut()));
        });
}

/**
 * @brief The checks of expectLifetimePrints(), which runs them under an outer capture.
 *
 * @tparam T Timer type.
 *
 * @param[in] name The name the timer prints: "stub" or "STM32".
 */
template<Type T>
void expectLifetimePrintsCaptured(const std::string& name)
{
    std::optional<Timer<T>> timer{};
    EXPECT_OUTPUT(captureOutput([&] { timer.emplace(1000U); }),
                  "Created " + name + " timer with timeout 1000 ms!\n");
    EXPECT_TRUE(timer->isInitialized());
    EXPECT_FALSE(timer->isRunning());
    EXPECT_EQ(static_cast<unsigned>(timer->timeout_ms()), 1000U);

    EXPECT_OUTPUT(captureOutput([&] { timer->start(); }), "Starting " + name + " timer!\n");
    EXPECT_TRUE(timer->isRunning());
    EXPECT_OUTPUT(captureOutput([&] { timer->stop(); }), "Stopping " + name + " timer!\n");
    EXPECT_FALSE(timer->isRunning());
    EXPECT_OUTPUT(captureOutput([&] { timer->toggle(); }), "Toggling " + name + " timer!\n");
    EXPECT_TRUE(timer->isRunning());

    // A running timer is stopped by its destructor, which then says it is being destroyed.
    EXPECT_OUTPUT(captureOutput([&] { timer.reset(); }),
                  "Stopping " + name + " timer!\nDestroying " + name + " timer!\n");
}

/**
 * @brief Check what a timer prints and reports from its creation to its destruction.
 *
 * @tparam T Timer type.
 *
 * @param[in] name The name the timer prints: "stub" or "STM32".
 */
template<Type T>
void expectLifetimePrints(const std::string& name)
{
    // The outer capture keeps a timer destroyed by a failed expectation out of the report.
    captureOutput([&] { expectLifetimePrintsCaptured<T>(name); });
}

/**
 * @brief The checks of expectZeroTimeoutRejected(), which runs them under an outer capture.
 *
 * @tparam T Timer type.
 *
 * @param[in] name The name the timer prints: "stub" or "STM32".
 */
template<Type T>
void expectZeroTimeoutRejectedCaptured(const std::string& name)
{
    std::optional<Timer<T>> timer{};
    EXPECT_OUTPUT(captureOutput([&] { timer.emplace(0U); }),
                  "Failed to initialize " + name + " timer: invalid timeout 0 ms!\n");
    EXPECT_FALSE(timer->isInitialized());
    captureOutput(
        [&]
        {
            timer->start();
            timer->toggle();
        });
    EXPECT_FALSE(timer->isRunning());
    const std::string destroyed{captureOutput([&] { timer.reset(); })};
    EXPECT_TRUE(endsWith(destroyed, "Destroying " + name + " timer!\n"));
}

/**
 * @brief Check that a timeout of 0 is rejected, and that the timer then cannot be started.
 *
 * @tparam T Timer type.
 *
 * @param[in] name The name the timer prints: "stub" or "STM32".
 */
template<Type T>
void expectZeroTimeoutRejected(const std::string& name)
{
    // The outer capture keeps a timer destroyed by a failed expectation out of the report.
    captureOutput([&] { expectZeroTimeoutRejectedCaptured<T>(name); });
}

/**
 * @brief Check the counting: a timeout on exactly the timeout-th tick, a reset counter, no
 *        counting while stopped, and room in the counter for a timeout of 60000 ms.
 *
 * @tparam T Timer type.
 */
template<Type T>
void expectCounting()
{
    captureOutput(
        []
        {
            Timer<T> timer{100U};
            EXPECT_EQ(timeoutsAfter(timer, 1000U), 0U);
            timer.start();
            EXPECT_EQ(timeoutsAfter(timer, 99U), 0U);
            timer.tick();
            EXPECT_TRUE(timer.hasTimedOut());
            EXPECT_EQ(timeoutsAfter(timer, 500U), 5U);
            EXPECT_EQ(timeoutsAfter(timer, 99U), 0U);
            timer.stop();
            EXPECT_EQ(timeoutsAfter(timer, 1000U), 0U);

            Timer<T> slow{60000U};
            EXPECT_EQ(static_cast<unsigned>(slow.timeout_ms()), 60000U);
            slow.start();
            EXPECT_EQ(timeoutsAfter(slow, 59999U), 0U);
            EXPECT_EQ(timeoutsAfter(slow, 1U), 1U);
        });
}
} // namespace

/**
 * @brief Exercise 3.1: Type is an enumeration class with an 8-bit underlying type, and a timer
 *        declared without a type is a stub timer.
 */
TEST(Timer, TypeAndDefaultTemplateArgument)
{
    EXPECT_TRUE(std::is_enum<Type>::value);
    EXPECT_FALSE((std::is_convertible<Type, int>::value));
    EXPECT_TRUE((std::is_same<std::underlying_type_t<Type>, std::uint8_t>::value));
    EXPECT_TRUE((std::is_same<Timer<>, Timer<Type::Stub>>::value));
    EXPECT_FALSE((std::is_same<Timer<Type::Stub>, Timer<Type::Stm32>>::value));
}

/**
 * @brief Part I a), c) and f): the stub timer is final, has an explicit noexcept constructor,
 *        and can be neither default constructed, copied nor moved.
 */
TEST(StubTimer, IsDeclaredAsSpecified) { expectDeclaredAsSpecified<Type::Stub>(); }

/**
 * @brief Part II b): the STM32 timer is declared the same way.
 */
TEST(Stm32Timer, IsDeclaredAsSpecified) { expectDeclaredAsSpecified<Type::Stm32>(); }

/**
 * @brief Part I e): every method is noexcept, and the three queries can be called on a const
 *        timer.
 */
TEST(StubTimer, MethodsAreNoexceptAndQueriesConst) { expectMethodsAsSpecified<Type::Stub>(); }

/**
 * @brief Part II b): the STM32 timer has the same interface.
 */
TEST(Stm32Timer, MethodsAreNoexceptAndQueriesConst) { expectMethodsAsSpecified<Type::Stm32>(); }

/**
 * @brief Part I c) to e): what the stub timer prints when it is created, started, stopped,
 *        toggled and destroyed.
 */
TEST(StubTimer, PrintsItsLifetime) { expectLifetimePrints<Type::Stub>("stub"); }

/**
 * @brief Part II b): the STM32 specialization prints the same messages with its own name, which
 *        it only does if the specialization, not the primary template, is what was instantiated.
 */
TEST(Stm32Timer, PrintsItsLifetime) { expectLifetimePrints<Type::Stm32>("STM32"); }

/**
 * @brief Part I c): a timeout of 0 leaves the stub timer uninitialized, and it will not start.
 */
TEST(StubTimer, RejectsAZeroTimeout) { expectZeroTimeoutRejected<Type::Stub>("stub"); }

/**
 * @brief Part II b): the same for the STM32 timer.
 */
TEST(Stm32Timer, RejectsAZeroTimeout) { expectZeroTimeoutRejected<Type::Stm32>("STM32"); }

/**
 * @brief Part I b) and e): the stub timer times out on its timeout-th tick, resets, does not count
 *        while stopped, and holds a 16-bit timeout.
 */
TEST(StubTimer, CountsTicks) { expectCounting<Type::Stub>(); }

/**
 * @brief Part II b): the same for the STM32 timer.
 */
TEST(Stm32Timer, CountsTicks) { expectCounting<Type::Stm32>(); }

#ifdef PROGRAM

/**
 * @brief Part III: the program prints exactly the expected output, the timeouts interleaved in the
 *        order the two timers reach them, and the timers destroyed in the reverse order of their
 *        creation. It is run as the program it is, 1 ms sleeps and all, so this takes two seconds.
 */
TEST(Program, PrintsTheExpectedOutput)
{
    EXPECT_PROGRAM_OUTPUT("Created stub timer with timeout 500 ms!\n"
                          "Created STM32 timer with timeout 1500 ms!\n"
                          "Starting stub timer!\n"
                          "Starting STM32 timer!\n"
                          "Stub timer has timed out after 500 ms!\n"
                          "Stub timer has timed out after 500 ms!\n"
                          "Stub timer has timed out after 500 ms!\n"
                          "STM32 timer has timed out after 1500 ms!\n"
                          "Stub timer has timed out after 500 ms!\n"
                          "Stopping STM32 timer!\n"
                          "Destroying STM32 timer!\n"
                          "Stopping stub timer!\n"
                          "Destroying stub timer!\n");
}

#endif
