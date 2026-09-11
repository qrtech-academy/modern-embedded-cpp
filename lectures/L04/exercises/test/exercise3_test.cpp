/**
 * @file Tests for Exercise Set 3: app::logic::Logic with a raw-pointer factory, in
 *       raw_factory/include/app/logic/logic.hpp, and the program in raw_factory/source/main.cpp.
 *
 *       Logic owns the driver its factory creates, so the tests give it a factory of their own:
 *       one that records what it was asked for and creates a driver that counts its own
 *       destruction. That shows whether Logic asks once, with the right pins, and deletes what it
 *       owns exactly once. run() never returns, so it is watched from outside, in a child
 *       process: see run_probe.hpp.
 */
#include <chrono>
#include <cstdint>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#include "app/logic/logic.hpp"
#include "driver/factory/interface.hpp"
#include "driver/serial/interface.hpp"
#include "qacademy/test/test.hpp"
#include "run_probe.hpp"

namespace
{
/** How many CountingSerial drivers have been created and destroyed. */
int created{};
int destroyed{};

/**
 * @brief A serial driver that counts how many of its kind exist.
 */
class CountingSerial final : public driver::serial::Interface
{
public:
    CountingSerial() noexcept { ++created; }
    ~CountingSerial() noexcept override { ++destroyed; }
    [[nodiscard]] bool isInitialized() const noexcept override { return true; }
    void write(const std::uint8_t) noexcept override {}
    bool read(std::uint8_t&) noexcept override { return false; }
};

/**
 * @brief A factory that records what it was asked for, and creates counting drivers.
 */
class CountingFactory final : public driver::factory::Interface
{
public:
    [[nodiscard]] driver::serial::Interface* serial(const std::uint8_t txPin,
                                                    const std::uint8_t rxPin) noexcept override
    {
        ++calls;
        lastTxPin = txPin;
        lastRxPin = rxPin;
        return new CountingSerial{};
    }

    int calls{};
    std::uint8_t lastTxPin{};
    std::uint8_t lastRxPin{};
};

/**
 * @brief A factory that creates the probe's serial driver, for watching run().
 */
class ProbeFactory final : public driver::factory::Interface
{
public:
    ProbeFactory(const int fd, const bool receives) noexcept
        : myFd{fd}
        , myReceives{receives}
    {}

    [[nodiscard]] driver::serial::Interface* serial(const std::uint8_t,
                                                    const std::uint8_t) noexcept override
    {
        return new probe::ProbeSerial{myFd, myReceives};
    }

private:
    const int myFd;
    const bool myReceives;
};

/**
 * @brief Watch run() for a little over four loop iterations.
 *
 * @param[in] receives True if the serial driver shall report a received byte each time.
 *
 * @return What run() did.
 */
probe::Report watchRun(const bool receives)
{
    return probe::runProbe(
        [receives](const int fd)
        {
            ProbeFactory factory{fd, receives};
            app::logic::Logic logic{factory, 17U, 18U};
            logic.run();
        },
        std::chrono::milliseconds{450});
}

/**
 * @brief What constructing and destroying one Logic did.
 */
struct Lifetime
{
    /** How often the factory was asked for a driver, and with which pins. */
    int calls{};
    unsigned txPin{};
    unsigned rxPin{};

    /** Drivers created and destroyed while the Logic existed, and destroyed in all. */
    int createdWhileAlive{};
    int destroyedWhileAlive{};
    int destroyedInAll{};
};

/**
 * @brief Construct a Logic with the counting factory, destroy it, and report what happened.
 *
 *        Measured in a child process: a Logic that deletes its driver twice crashes, and that
 *        should fail this test with an explanation, not end every test after it.
 *
 * @return What happened.
 */
Lifetime measureLifetime()
{
    int crashed{};
    const std::string text{probe::inChild(
        []
        {
            CountingFactory factory{};
            int createdWhileAlive{}, destroyedWhileAlive{};
            {
                app::logic::Logic logic{factory, 17U, 18U};
                createdWhileAlive   = created;
                destroyedWhileAlive = destroyed;
            }
            return std::to_string(factory.calls) + " " + std::to_string(factory.lastTxPin) + " " +
                   std::to_string(factory.lastRxPin) + " " + std::to_string(createdWhileAlive) +
                   " " + std::to_string(destroyedWhileAlive) + " " + std::to_string(destroyed);
        },
        crashed)};
    if (0 != crashed)
    {
        probe::fail(std::string{"constructing and destroying a Logic crashed ("} +
                        strsignal(crashed) +
                        "): is its driver deleted twice, or used after it was deleted?",
                    __FILE__, __LINE__);
    }
    Lifetime lifetime{};
    std::istringstream stream{text};
    stream >> lifetime.calls >> lifetime.txPin >> lifetime.rxPin >> lifetime.createdWhileAlive >>
        lifetime.destroyedWhileAlive >> lifetime.destroyedInAll;
    return lifetime;
}

/**
 * @brief True if T is not copyable and not movable, in either direction.
 */
template<typename T>
constexpr bool isNonCopyableAndNonMovable()
{
    return !std::is_copy_constructible<T>::value && !std::is_move_constructible<T>::value &&
           !std::is_copy_assignable<T>::value && !std::is_move_assignable<T>::value;
}
} // namespace

/**
 * @brief Exercise 3.1 b): the constructor asks the factory for one serial driver, with the
 *        pins it was given.
 */
TEST(Logic, AsksTheFactoryForOneDriverWithTheGivenPins)
{
    const Lifetime lifetime{measureLifetime()};
    EXPECT_EQ(lifetime.calls, 1);
    EXPECT_EQ(lifetime.txPin, 17U);
    EXPECT_EQ(lifetime.rxPin, 18U);
}

/**
 * @brief Exercise 3.1 c): Logic owns the driver, and its destructor deletes it exactly once:
 *        not zero times, which leaks it, and not twice.
 */
TEST(Logic, DeletesItsDriverExactlyOnce)
{
    const Lifetime lifetime{measureLifetime()};
    EXPECT_EQ(lifetime.createdWhileAlive, 1);
    EXPECT_EQ(lifetime.destroyedWhileAlive, 0);
    EXPECT_EQ(lifetime.destroyedInAll, 1);
}

/**
 * @brief Exercise 3.1 b): the constructor is noexcept; e) there is no default constructor, and
 *        no copy or move, because a copy would delete the same driver twice.
 */
TEST(Logic, IsNoexceptAndCannotBeCopiedOrMoved)
{
    EXPECT_TRUE((std::is_nothrow_constructible<app::logic::Logic, driver::factory::Interface&,
                                               std::uint8_t, std::uint8_t>::value));
    EXPECT_TRUE(std::is_nothrow_destructible<app::logic::Logic>::value);
    EXPECT_FALSE(std::is_default_constructible<app::logic::Logic>::value);
    EXPECT_TRUE(isNonCopyableAndNonMovable<app::logic::Logic>());
    EXPECT_TRUE(noexcept(std::declval<app::logic::Logic&>().run()));
}

/**
 * @brief Exercise 3.1 d): run() sends 0, 1, 2, ... and tries to read after every write, about
 *        100 ms apart, and keeps going: in 450 ms that is four or five writes, where a loop with
 *        no delay makes thousands.
 */
TEST(Run, SendsAnIncrementingByteEvery100Milliseconds) { EXPECT_THE_LOOP(watchRun(false)); }

/**
 * @brief Exercise 3.1 d): run() prints a received byte, and prints nothing when nothing was
 *        received.
 */
TEST(Run, PrintsOnlyWhatWasReceived)
{
    EXPECT_PRINTS_WHAT_IS_RECEIVED(watchRun(false), false);
    EXPECT_PRINTS_WHAT_IS_RECEIVED(watchRun(true), true);
}

#if __has_include("main.cpp")

// Your program, with its main() renamed. It is compiled, to check that it builds against your
// headers, but not run: run() never returns. A main() may leave out its return statement and
// any other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise3Main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

/**
 * @brief Exercise 3.2: main.cpp compiles against your headers. What it does when it runs, you
 *        check by running it: it never stops.
 */
TEST(Program, Compiles) { EXPECT_TRUE((std::is_same<decltype(exercise3Main()), int>::value)); }

#endif
