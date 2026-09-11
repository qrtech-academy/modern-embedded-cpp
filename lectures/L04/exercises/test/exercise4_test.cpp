/**
 * @file Tests for Exercise Set 4: the factory interface, the two factories and the logic class,
 *       rewritten with std::unique_ptr, in smart_factory/include.
 *
 *       The binary is built once the new factory interface exists. The factories and the logic
 *       class are tested as each file appears, so the set can be checked one exercise at a time.
 */
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#include "driver/factory/interface.hpp"
#include "driver/serial/interface.hpp"
#include "qacademy/test/test.hpp"
#include "support/output.hpp"

using support::captureOutput;

namespace
{
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
 * @brief Exercise 4.1: the factory interface is still abstract, with a virtual noexcept
 *        destructor.
 */
TEST(FactoryInterface, IsAbstractWithAVirtualDestructor)
{
    EXPECT_TRUE(std::is_abstract<driver::factory::Interface>::value);
    EXPECT_TRUE(std::has_virtual_destructor<driver::factory::Interface>::value);
    EXPECT_TRUE(std::is_nothrow_destructible<driver::factory::Interface>::value);
}

/**
 * @brief Exercise 4.1: serial() still takes two pins and is noexcept, and now returns a
 *        std::unique_ptr to the serial interface: the caller owns the driver without having to
 *        delete it.
 */
TEST(FactoryInterface, SerialReturnsAUniquePtr)
{
    using driver::factory::Interface;
    using Owner = std::unique_ptr<driver::serial::Interface>;
    EXPECT_TRUE((std::is_same<decltype(&Interface::serial),
                              Owner (Interface::*)(std::uint8_t, std::uint8_t) noexcept>::value));
}

#if __has_include("driver/factory/esp32s3.hpp")
#include "driver/factory/esp32s3.hpp"
#include "driver/serial/esp32s3.hpp"

/**
 * @brief Exercise 4.2 a): the ESP32-S3 factory is still a final, non-copyable implementation.
 */
TEST(Esp32s3Factory, IsAFinalNonCopyableImplementation)
{
    using driver::factory::Esp32s3;
    EXPECT_TRUE(std::is_final<Esp32s3>::value);
    EXPECT_TRUE((std::is_convertible<Esp32s3*, driver::factory::Interface*>::value));
    EXPECT_TRUE(std::is_nothrow_default_constructible<Esp32s3>::value);
    EXPECT_TRUE(isNonCopyableAndNonMovable<Esp32s3>());
}

/**
 * @brief Exercise 4.2 a): serial() returns a std::unique_ptr holding an ESP32-S3 driver with
 *        the given pins. Nothing here deletes it: the std::unique_ptr does.
 */
TEST(Esp32s3Factory, CreatesAnEsp32s3DriverWithTheGivenPins)
{
    driver::factory::Esp32s3 factory{};
    driver::factory::Interface& interface{factory};
    const std::unique_ptr<driver::serial::Interface> serial{interface.serial(17U, 18U)};
    EXPECT_TRUE(nullptr != dynamic_cast<driver::serial::Esp32s3*>(serial.get()));
    EXPECT_OUTPUT(captureOutput([&] { serial->write(0xFFU); }),
                  "Transmitting byte 0xFF via TX pin 17!\n");
}

#endif

#if __has_include("driver/factory/stub.hpp")
#include "driver/factory/stub.hpp"
#include "driver/serial/stub.hpp"

/**
 * @brief Exercise 4.2 b): the stub factory is still a final, non-copyable implementation.
 */
TEST(StubFactory, IsAFinalNonCopyableImplementation)
{
    using driver::factory::Stub;
    EXPECT_TRUE(std::is_final<Stub>::value);
    EXPECT_TRUE((std::is_convertible<Stub*, driver::factory::Interface*>::value));
    EXPECT_TRUE(std::is_nothrow_default_constructible<Stub>::value);
    EXPECT_TRUE(isNonCopyableAndNonMovable<Stub>());
}

/**
 * @brief Exercise 4.2 b): serial() ignores the pins and returns a std::unique_ptr holding a new,
 *        working stub driver each time.
 */
TEST(StubFactory, CreatesANewStubDriverEachTime)
{
    driver::factory::Stub factory{};
    driver::factory::Interface& interface{factory};
    const std::unique_ptr<driver::serial::Interface> first{interface.serial(17U, 18U)};
    const std::unique_ptr<driver::serial::Interface> second{interface.serial(0U, 0U)};
    EXPECT_TRUE(nullptr != dynamic_cast<driver::serial::Stub*>(first.get()));
    EXPECT_TRUE(nullptr != dynamic_cast<driver::serial::Stub*>(second.get()));
    EXPECT_TRUE(first.get() != second.get());

    std::uint8_t byte{};
    first->write(0x7EU);
    EXPECT_TRUE(first->read(byte));
    EXPECT_EQ(static_cast<unsigned>(byte), 0x7EU);
    EXPECT_FALSE(second->read(byte));
}

#endif

#if __has_include("app/logic/logic.hpp")
#include "app/logic/logic.hpp"
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
    [[nodiscard]] std::unique_ptr<driver::serial::Interface>
    serial(const std::uint8_t txPin, const std::uint8_t rxPin) noexcept override
    {
        ++calls;
        lastTxPin = txPin;
        lastRxPin = rxPin;
        return std::make_unique<CountingSerial>();
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

    [[nodiscard]] std::unique_ptr<driver::serial::Interface>
    serial(const std::uint8_t, const std::uint8_t) noexcept override
    {
        return std::make_unique<probe::ProbeSerial>(myFd, myReceives);
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
} // namespace

/**
 * @brief Exercise 4.3 b): the constructor asks the factory for one serial driver, with the
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
 * @brief Exercise 4.3 a) and c): the std::unique_ptr member destroys the driver exactly once
 *        when Logic is destroyed, with no destructor written to do it. A destructor left in that
 *        still deletes the pointer makes that twice.
 */
TEST(Logic, TheUniquePtrDestroysTheDriverExactlyOnce)
{
    const Lifetime lifetime{measureLifetime()};
    EXPECT_EQ(lifetime.createdWhileAlive, 1);
    EXPECT_EQ(lifetime.destroyedWhileAlive, 0);
    EXPECT_EQ(lifetime.destroyedInAll, 1);
}

/**
 * @brief Exercise 4.3: the constructor is still noexcept, and Logic still cannot be default
 *        constructed, copied or moved.
 */
TEST(Logic, IsNoexceptAndCannotBeCopiedOrMoved)
{
    EXPECT_TRUE((std::is_nothrow_constructible<app::logic::Logic, driver::factory::Interface&,
                                               std::uint8_t, std::uint8_t>::value));
    EXPECT_FALSE(std::is_default_constructible<app::logic::Logic>::value);
    EXPECT_TRUE(isNonCopyableAndNonMovable<app::logic::Logic>());
    EXPECT_TRUE(noexcept(std::declval<app::logic::Logic&>().run()));
}

/**
 * @brief Exercise 4.3 d): run() behaves as before: 0, 1, 2, ..., a read after every write, about
 *        100 ms apart.
 */
TEST(Run, SendsAnIncrementingByteEvery100Milliseconds) { EXPECT_THE_LOOP(watchRun(false)); }

/**
 * @brief Exercise 4.3 d): run() prints a received byte, and nothing when nothing was received.
 */
TEST(Run, PrintsOnlyWhatWasReceived)
{
    EXPECT_PRINTS_WHAT_IS_RECEIVED(watchRun(false), false);
    EXPECT_PRINTS_WHAT_IS_RECEIVED(watchRun(true), true);
}

#endif

#if __has_include("main.cpp")

// Your program, with its main() renamed. It is compiled, to check that it builds against your
// headers, but not run: run() never returns. A main() may leave out its return statement and
// any other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise4Main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

/**
 * @brief Exercise 4.3: main.cpp compiles against your smart-pointer headers.
 */
TEST(Program, Compiles) { EXPECT_TRUE((std::is_same<decltype(exercise4Main()), int>::value)); }

#endif
