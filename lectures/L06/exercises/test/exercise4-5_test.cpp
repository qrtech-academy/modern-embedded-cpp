/**
 * @file Tests for Exercise Sets 4 and 5: driver::counter::Interface, the thread-safe
 *       driver::counter::Stub, and, once source/main.cpp exists, counterThread() and the program.
 *
 *       Set 5 extends the stub from Set 4, so these tests check where it ends up. A stub without
 *       Set 5's setInitialized() yet still compiles here: the Set 5 test detects the method, and
 *       fails with a message saying what is missing rather than stopping the whole build.
 */
#include <atomic>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "driver/counter/interface.hpp"
#include "driver/counter/stub.hpp"
#include "qacademy/test/test.hpp"
#include "support/output.hpp"
#include "support/program.hpp"

using driver::counter::Interface;
using driver::counter::Stub;
namespace
{
/** True if T has a setInitialized(bool) method, which Exercise 5.2 adds. */
template<typename T, typename = void>
struct HasSetInitialized : std::false_type
{
};

template<typename T>
struct HasSetInitialized<T, std::void_t<decltype(std::declval<T&>().setInitialized(true))>>
    : std::true_type
{
};

/**
 * @brief Increment a counter from several threads at once, through the interface.
 *
 * @param[in, out] counter The counter.
 * @param[in] threadCount How many threads.
 * @param[in] iterations How many increments per thread.
 */
void incrementConcurrently(Interface& counter, const unsigned threadCount,
                           const std::uint32_t iterations)
{
    std::vector<std::thread> threads{};
    for (unsigned i{}; i < threadCount; ++i)
    {
        threads.emplace_back(
            [&counter, iterations]
            {
                for (std::uint32_t j{}; j < iterations; ++j)
                {
                    counter.increment();
                }
            });
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
}

/**
 * @brief Exercise Set 5 on a stub that has setInitialized(): an uninitialized stub ignores
 *        increment() and reset() and reports zero, and keeps its value for when it is
 *        initialized again.
 *
 * @tparam T The stub type.
 */
template<typename T>
void checkInitializationGuards()
{
    if constexpr (HasSetInitialized<T>::value)
    {
        T stub{};
        EXPECT_TRUE(stub.isInitialized());
        stub.increment();
        stub.increment();
        stub.increment();

        stub.setInitialized(false);
        EXPECT_FALSE(stub.isInitialized());
        EXPECT_EQ(stub.value(), 0U);
        stub.increment();
        stub.reset();
        EXPECT_EQ(stub.value(), 0U);

        stub.setInitialized(true);
        EXPECT_TRUE(stub.isInitialized());
        EXPECT_EQ(stub.value(), 3U);
        EXPECT_TRUE(noexcept(stub.setInitialized(true)));
    }
    else
    {
        throw std::runtime_error{"Exercise 5.2: driver::counter::Stub has no "
                                 "setInitialized(bool) yet"};
    }
}
} // namespace

/**
 * @brief Exercise 4.1: the interface is abstract, has a virtual destructor, and declares the
 *        four methods with the qualifiers the exercise gives.
 */
TEST(Interface, IsAnAbstractInterface)
{
    EXPECT_TRUE(std::is_abstract<Interface>::value);
    EXPECT_TRUE(std::has_virtual_destructor<Interface>::value);
    EXPECT_TRUE(std::is_nothrow_destructible<Interface>::value);

    const Interface* constCounter{nullptr};
    Interface* counter{nullptr};
    EXPECT_TRUE((std::is_same<decltype(constCounter->isInitialized()), bool>::value));
    EXPECT_TRUE((std::is_same<decltype(constCounter->value()), std::uint32_t>::value));
    EXPECT_TRUE(noexcept(constCounter->isInitialized()));
    EXPECT_TRUE(noexcept(constCounter->value()));
    EXPECT_TRUE(noexcept(counter->increment()));
    EXPECT_TRUE(noexcept(counter->reset()));
}

/**
 * @brief Exercise 4.2: the stub implements the interface, is final, cannot be copied or moved,
 *        and is created with a noexcept default constructor.
 */
TEST(Stub, IsAFinalNonCopyableImplementation)
{
    EXPECT_TRUE((std::is_base_of<Interface, Stub>::value));
    EXPECT_TRUE(std::is_final<Stub>::value);
    EXPECT_TRUE(std::is_nothrow_default_constructible<Stub>::value);
    EXPECT_FALSE(std::is_copy_constructible<Stub>::value);
    EXPECT_FALSE(std::is_move_constructible<Stub>::value);
    EXPECT_FALSE(std::is_copy_assignable<Stub>::value);
    EXPECT_FALSE(std::is_move_assignable<Stub>::value);
}

/**
 * @brief Exercise 4.2: a new stub is initialized and at zero, counts, and resets.
 */
TEST(Stub, CountsAndResets)
{
    Stub stub{};
    Interface& counter{stub};
    EXPECT_TRUE(counter.isInitialized());
    EXPECT_EQ(counter.value(), 0U);
    counter.increment();
    counter.increment();
    EXPECT_EQ(counter.value(), 2U);
    counter.reset();
    EXPECT_EQ(counter.value(), 0U);
}

/**
 * @brief Exercise 4.2: eight threads incrementing at once lose no increment.
 */
TEST(Stub, CountsExactlyFromManyThreads)
{
    Stub stub{};
    incrementConcurrently(stub, 8U, 20000U);
    EXPECT_EQ(stub.value(), 160000U);
}

/**
 * @brief Exercise 4.2: a reader calling value() while others increment sees the count only ever
 *        rise, never a value above the final count, and the count ends exact.
 */
TEST(Stub, ValueIsConsistentWhileOthersIncrement)
{
    Stub stub{};
    std::atomic<bool> done{false};
    bool valueNeverWentDown{true};
    bool valueNeverExceededTheTotal{true};
    constexpr std::uint32_t total{4U * 20000U};

    std::thread reader{[&]
                       {
                           std::uint32_t previous{};
                           while (!done.load())
                           {
                               const std::uint32_t current{stub.value()};
                               if (current < previous) { valueNeverWentDown = false; }
                               if (current > total) { valueNeverExceededTheTotal = false; }
                               previous = current;
                           }
                       }};
    incrementConcurrently(stub, 4U, 20000U);
    done.store(true);
    reader.join();

    EXPECT_TRUE(valueNeverWentDown);
    EXPECT_TRUE(valueNeverExceededTheTotal);
    EXPECT_EQ(stub.value(), total);
}

/**
 * @brief Exercise 4.2: reset() from one thread while others increment leaves a count no larger
 *        than the increments made after it, and a final reset leaves zero.
 */
TEST(Stub, ResetsWhileOthersIncrement)
{
    Stub stub{};
    std::thread resetter{[&stub]
                         {
                             for (int i{}; i < 1000; ++i)
                             {
                                 stub.reset();
                             }
                         }};
    incrementConcurrently(stub, 4U, 10000U);
    resetter.join();
    const bool noMoreThanWasIncremented{stub.value() <= 40000U};
    EXPECT_TRUE(noMoreThanWasIncremented);
    stub.reset();
    EXPECT_EQ(stub.value(), 0U);
}

/**
 * @brief Exercise Set 5: the initialization flag guards increment(), value() and reset().
 */
TEST(Stub, GuardsItsOperationsWithTheInitializationFlag) { checkInitializationGuards<Stub>(); }

#if __has_include("main.cpp")

// Your program, with its main() renamed. A main() may leave out its return statement and any
// other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise45Main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

/**
 * @brief Exercise 4.3: counterThread() calls increment() once per iteration, and is noexcept.
 */
TEST(CounterThread, IncrementsOncePerIteration)
{
    Stub stub{};
    counterThread(stub, 250U);
    EXPECT_EQ(stub.value(), 250U);
    EXPECT_TRUE(noexcept(counterThread(stub, 1U)));
}

#ifdef PROGRAM
/**
 * @brief Exercise 4.3: the program's two threads, of 100 and 200 iterations, end at 300.
 */
TEST(Program, PrintsTheFinalValue)
{
    const support::ProgramResult result{support::runProgram(PROGRAM)};
    EXPECT_CLEAN_EXIT(result);
    const std::string& output{result.output};
    const bool printedTheFinalValue{std::string::npos != output.find("300")};
    EXPECT_TRUE(printedTheFinalValue);
}
#endif

#endif
