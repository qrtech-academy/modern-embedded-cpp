/**
 * @file Tests for Exercise Set 2: driver::factory::Interface, driver::factory::Esp32s3 and
 *       driver::factory::Stub, returning raw pointers, in raw_factory/include/driver/factory.
 *
 *       A factory returning a raw pointer hands ownership to the caller, so every test here
 *       deletes what it was given, through the interface, exactly as the logic class will.
 */
#include <cstdint>
#include <type_traits>

#include "driver/factory/esp32s3.hpp"
#include "driver/factory/interface.hpp"
#include "driver/factory/stub.hpp"
#include "driver/serial/esp32s3.hpp"
#include "driver/serial/interface.hpp"
#include "driver/serial/stub.hpp"
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
 * @brief Exercise 2.1 a): the factory interface is abstract, and its destructor is virtual and
 *        noexcept.
 */
TEST(FactoryInterface, IsAbstractWithAVirtualDestructor)
{
    EXPECT_TRUE(std::is_abstract<driver::factory::Interface>::value);
    EXPECT_TRUE(std::has_virtual_destructor<driver::factory::Interface>::value);
    EXPECT_TRUE(std::is_nothrow_destructible<driver::factory::Interface>::value);
}

/**
 * @brief Exercise 2.1 b): serial() takes two pins and returns a raw pointer to the serial
 *        interface, noexcept.
 */
TEST(FactoryInterface, SerialSignature)
{
    using driver::factory::Interface;
    EXPECT_TRUE((std::is_same<decltype(&Interface::serial),
                              driver::serial::Interface* (
                                  Interface::*)(std::uint8_t, std::uint8_t) noexcept>::value));
}

/**
 * @brief Exercise 2.2: the ESP32-S3 factory is a final implementation of the factory interface,
 *        default constructible, noexcept, and neither copyable nor movable.
 */
TEST(Esp32s3Factory, IsAFinalNonCopyableImplementation)
{
    using driver::factory::Esp32s3;
    EXPECT_TRUE(std::is_final<Esp32s3>::value);
    EXPECT_TRUE((std::is_convertible<Esp32s3*, driver::factory::Interface*>::value));
    EXPECT_TRUE(std::is_nothrow_default_constructible<Esp32s3>::value);
    EXPECT_TRUE(std::is_nothrow_destructible<Esp32s3>::value);
    EXPECT_TRUE(isNonCopyableAndNonMovable<Esp32s3>());
}

/**
 * @brief Exercise 2.2 b): serial() creates an ESP32-S3 serial driver with the pins it was given,
 *        and the caller owns it: it is deleted here, through the interface.
 */
TEST(Esp32s3Factory, CreatesAnEsp32s3DriverWithTheGivenPins)
{
    driver::factory::Esp32s3 factory{};
    driver::factory::Interface& interface{factory};
    driver::serial::Interface* const serial{interface.serial(17U, 18U)};
    EXPECT_TRUE(nullptr != dynamic_cast<driver::serial::Esp32s3*>(serial));
    EXPECT_OUTPUT(captureOutput([&] { serial->write(0x2AU); }),
                  "Transmitting byte 0x2A via TX pin 17!\n");
    delete serial;
}

/**
 * @brief Exercise 2.2 b): every call creates a new driver.
 */
TEST(Esp32s3Factory, CreatesANewDriverEachTime)
{
    driver::factory::Esp32s3 factory{};
    driver::serial::Interface* const first{factory.serial(1U, 2U)};
    driver::serial::Interface* const second{factory.serial(3U, 4U)};
    EXPECT_TRUE(first != second);
    EXPECT_OUTPUT(captureOutput([&] { second->write(0x01U); }),
                  "Transmitting byte 0x01 via TX pin 3!\n");
    delete first;
    delete second;
}

/**
 * @brief Exercise 2.3: the stub factory is a final implementation of the factory interface,
 *        default constructible, noexcept, and neither copyable nor movable.
 */
TEST(StubFactory, IsAFinalNonCopyableImplementation)
{
    using driver::factory::Stub;
    EXPECT_TRUE(std::is_final<Stub>::value);
    EXPECT_TRUE((std::is_convertible<Stub*, driver::factory::Interface*>::value));
    EXPECT_TRUE(std::is_nothrow_default_constructible<Stub>::value);
    EXPECT_TRUE(std::is_nothrow_destructible<Stub>::value);
    EXPECT_TRUE(isNonCopyableAndNonMovable<Stub>());
}

/**
 * @brief Exercise 2.3 b): serial() ignores the pins and creates a working stub serial driver,
 *        a new one each time, which the caller owns and deletes.
 */
TEST(StubFactory, CreatesANewStubDriverEachTime)
{
    driver::factory::Stub factory{};
    driver::factory::Interface& interface{factory};
    driver::serial::Interface* const first{interface.serial(17U, 18U)};
    driver::serial::Interface* const second{interface.serial(0U, 0U)};
    EXPECT_TRUE(nullptr != dynamic_cast<driver::serial::Stub*>(first));
    EXPECT_TRUE(nullptr != dynamic_cast<driver::serial::Stub*>(second));
    EXPECT_TRUE(first != second);

    std::uint8_t byte{};
    first->write(0x7EU);
    EXPECT_TRUE(first->read(byte));
    EXPECT_EQ(static_cast<unsigned>(byte), 0x7EU);
    EXPECT_FALSE(second->read(byte));
    delete first;
    delete second;
}
