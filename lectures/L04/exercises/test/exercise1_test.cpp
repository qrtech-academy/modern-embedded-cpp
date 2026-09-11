/**
 * @file Tests for Exercise Set 1: driver::serial::Interface, driver::serial::Stub and
 *       driver::serial::Esp32s3, in raw_factory/include/driver/serial.
 *
 *       A method's exact signature, noexcept and const included, is part of its type, so the
 *       signatures the exercise specifies are checked by comparing member function pointer
 *       types. A mismatch then fails one named test instead of stopping the whole file from
 *       compiling.
 */
#include <cstdint>
#include <type_traits>

#include "driver/serial/esp32s3.hpp"
#include "driver/serial/interface.hpp"
#include "driver/serial/stub.hpp"
#include "qacademy/test/test.hpp"
#include "support/output.hpp"

using support::captureOutput;

namespace
{
using driver::serial::Interface;

/** Overload resolution stand-in: accepts a T, however it can be made. */
template<typename T>
void takes(const T&);

/**
 * @brief True if T can be made implicitly from two bytes, as in T t = {17U, 18U};. An explicit
 *        constructor makes that ill-formed, which is what this detects.
 */
template<typename T, typename = void>
struct ImplicitFromTwoBytes : std::false_type
{
};

template<typename T>
struct ImplicitFromTwoBytes<T, std::void_t<decltype(takes<T>({std::uint8_t{}, std::uint8_t{}}))>>
    : std::true_type
{
};

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
 * @brief Exercise 1.1: the interface is abstract, and its destructor is virtual and noexcept.
 */
TEST(Interface, IsAbstractWithAVirtualDestructor)
{
    EXPECT_TRUE(std::is_abstract<Interface>::value);
    EXPECT_TRUE(std::has_virtual_destructor<Interface>::value);
    EXPECT_TRUE(std::is_nothrow_destructible<Interface>::value);
}

/**
 * @brief Exercise 1.1 b): bool isInitialized() const noexcept.
 */
TEST(Interface, IsInitializedSignature)
{
    EXPECT_TRUE((std::is_same<decltype(&Interface::isInitialized),
                              bool (Interface::*)() const noexcept>::value));
}

/**
 * @brief Exercise 1.1 c): void write(std::uint8_t) noexcept.
 */
TEST(Interface, WriteSignature)
{
    EXPECT_TRUE((std::is_same<decltype(&Interface::write),
                              void (Interface::*)(std::uint8_t) noexcept>::value));
}

/**
 * @brief Exercise 1.1 d): bool read(std::uint8_t&) noexcept.
 *
 *        That read() is not [[nodiscard]] cannot be tested here: GCC does not warn about a
 *        discarded result of a virtual call made through the interface, attribute or not.
 */
TEST(Interface, ReadSignature)
{
    EXPECT_TRUE((std::is_same<decltype(&Interface::read),
                              bool (Interface::*)(std::uint8_t&) noexcept>::value));
}

/**
 * @brief Exercise 1.2: the stub is a final implementation of the interface, created without
 *        arguments by a noexcept default constructor.
 */
TEST(Stub, IsAFinalImplementation)
{
    EXPECT_TRUE(std::is_final<driver::serial::Stub>::value);
    EXPECT_TRUE((std::is_base_of<Interface, driver::serial::Stub>::value));
    EXPECT_TRUE((std::is_convertible<driver::serial::Stub*, Interface*>::value));
    EXPECT_TRUE(std::is_nothrow_default_constructible<driver::serial::Stub>::value);
    EXPECT_TRUE(std::is_nothrow_destructible<driver::serial::Stub>::value);
}

/**
 * @brief Exercise 1.2 e): the stub can be neither copied nor moved.
 */
TEST(Stub, CannotBeCopiedOrMoved)
{
    EXPECT_TRUE(isNonCopyableAndNonMovable<driver::serial::Stub>());
}

/**
 * @brief Exercise 1.2 b) and d): a new stub is initialized, and has nothing to read.
 */
TEST(Stub, StartsInitializedAndEmpty)
{
    driver::serial::Stub stub{};
    std::uint8_t byte{};
    EXPECT_TRUE(stub.isInitialized());
    EXPECT_FALSE(stub.read(byte));
}

/**
 * @brief Exercise 1.2 d): a byte written can be read back once, through the interface.
 */
TEST(Stub, ReadsBackTheLastByteWrittenOnce)
{
    driver::serial::Stub stub{};
    Interface& serial{stub};
    std::uint8_t byte{};

    serial.write(0x42U);
    EXPECT_TRUE(serial.read(byte));
    EXPECT_EQ(static_cast<unsigned>(byte), 0x42U);
    EXPECT_FALSE(serial.read(byte));

    serial.write(0x01U);
    serial.write(0xFFU);
    EXPECT_TRUE(serial.read(byte));
    EXPECT_EQ(static_cast<unsigned>(byte), 0xFFU);
    EXPECT_FALSE(serial.read(byte));
}

/**
 * @brief Exercise 1.3: the ESP32-S3 driver is a final implementation of the interface.
 */
TEST(Esp32s3, IsAFinalImplementation)
{
    EXPECT_TRUE(std::is_final<driver::serial::Esp32s3>::value);
    EXPECT_TRUE((std::is_convertible<driver::serial::Esp32s3*, Interface*>::value));
    EXPECT_TRUE(std::is_nothrow_destructible<driver::serial::Esp32s3>::value);
}

/**
 * @brief Exercise 1.3 b): the constructor takes two pins, is noexcept, and is explicit, so
 *        driver::serial::Esp32s3 serial = {17U, 18U}; does not compile.
 */
TEST(Esp32s3, ConstructorIsExplicitAndNoexcept)
{
    EXPECT_TRUE((
        std::is_nothrow_constructible<driver::serial::Esp32s3, std::uint8_t, std::uint8_t>::value));
    EXPECT_FALSE(ImplicitFromTwoBytes<driver::serial::Esp32s3>::value);
}

/**
 * @brief Exercise 1.3 e): no default constructor, and no copy or move.
 */
TEST(Esp32s3, CannotBeDefaultConstructedCopiedOrMoved)
{
    EXPECT_FALSE(std::is_default_constructible<driver::serial::Esp32s3>::value);
    EXPECT_TRUE(isNonCopyableAndNonMovable<driver::serial::Esp32s3>());
}

/**
 * @brief Exercise 1.3 d): write() prints the byte in hexadecimal and the TX pin, as in the
 *        exercise's example; a second byte and pin show that neither is hard-coded, and that it
 *        is the transmit pin that is printed, not the receive pin.
 */
TEST(Esp32s3, WritePrintsTheByteAndTheTransmitPin)
{
    EXPECT_OUTPUT(captureOutput(
                      []
                      {
                          driver::serial::Esp32s3 serial{17U, 18U};
                          serial.write(0xFFU);
                      }),
                  "Transmitting byte 0xFF via TX pin 17!\n");
    EXPECT_OUTPUT(captureOutput(
                      []
                      {
                          driver::serial::Esp32s3 serial{4U, 5U};
                          Interface& interface{serial};
                          interface.write(0xABU);
                      }),
                  "Transmitting byte 0xAB via TX pin 4!\n");
}

/**
 * @brief Exercise 1.3 d): the placeholder driver is always initialized, and never receives.
 */
TEST(Esp32s3, PlaceholderStatusAndRead)
{
    driver::serial::Esp32s3 serial{17U, 18U};
    std::uint8_t byte{};
    EXPECT_TRUE(serial.isInitialized());
    EXPECT_FALSE(serial.read(byte));
}
