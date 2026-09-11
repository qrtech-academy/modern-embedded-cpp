/**
 * @file Tests for Exercise Set 1: the serial interface in include/driver/serial/interface.hpp.
 *
 *       An interface cannot be instantiated, so it is tested through a test double: a minimal
 *       class implementing it, which records what is done to it. The double overrides every
 *       method with the exact signature Exercise 1.1 specifies, so it only compiles if your
 *       interface declares those methods, and it only overrides them if they are virtual.
 */
#include <cstdint>
#include <memory>
#include <type_traits>

#include "driver/serial/interface.hpp"
#include "qacademy/test/test.hpp"

namespace
{
/** How many Double objects have been destroyed. */
int destroyed{};

/**
 * @brief A test double: the smallest possible implementation of the serial interface.
 */
class Double final : public driver::serial::Interface
{
public:
    Double() noexcept = default;

    // Not marked override, so that an interface whose destructor is not virtual is reported by
    // the tests below rather than by a compile error in this test double.
    ~Double() noexcept { ++destroyed; }

    [[nodiscard]] bool isInitialized() const noexcept override { return true; }

    void write(const std::uint8_t byte) noexcept override { myLastByte = byte; }

    bool read(std::uint8_t& byte) noexcept override
    {
        byte = myLastByte;
        return true;
    }

    /** The last byte written through the interface. */
    std::uint8_t myLastByte{};
};

/** The interface as the rest of the program sees it. */
using Interface = driver::serial::Interface;
} // namespace

/**
 * @brief Exercise 1.1: every method except the destructor is pure virtual, so the interface is
 *        abstract and cannot be instantiated.
 */
TEST(Interface, IsAbstract)
{
    EXPECT_TRUE(std::is_abstract<Interface>::value);
    EXPECT_TRUE(std::is_polymorphic<Interface>::value);
    EXPECT_FALSE(std::is_abstract<Double>::value);
}

/**
 * @brief Exercise 1.1 a): the destructor is virtual, so deleting an implementation through a
 *        pointer to the interface destroys the implementation too.
 */
TEST(Interface, DestructorIsVirtual)
{
    EXPECT_TRUE(std::has_virtual_destructor<Interface>::value);
    EXPECT_TRUE(std::is_nothrow_destructible<Interface>::value);
    destroyed = 0;
    std::unique_ptr<Interface> serial{new Double{}};
    serial.reset();
    EXPECT_EQ(destroyed, 1);
}

/**
 * @brief Exercise 1.1 b): isInitialized() returns a bool, is const and noexcept, and is
 *        dispatched to the implementation.
 */
TEST(Interface, IsInitializedIsAConstQuery)
{
    const Double serial{};
    const Interface& interface{serial};
    EXPECT_TRUE((std::is_same<decltype(interface.isInitialized()), bool>::value));
    EXPECT_TRUE(noexcept(interface.isInitialized()));
    EXPECT_TRUE(interface.isInitialized());
}

/**
 * @brief Exercise 1.1 c): write() takes one std::uint8_t, returns nothing, is noexcept, and is
 *        dispatched to the implementation.
 */
TEST(Interface, WriteTransmitsOneByte)
{
    Double serial{};
    Interface& interface{serial};
    EXPECT_TRUE((std::is_same<decltype(interface.write(std::uint8_t{})), void>::value));
    EXPECT_TRUE(noexcept(interface.write(std::uint8_t{})));
    interface.write(0x41U);
    EXPECT_EQ(static_cast<unsigned>(serial.myLastByte), 0x41U);
}

/**
 * @brief Exercise 1.1 d): read() takes a reference to the byte it stores into, returns a bool,
 *        is noexcept, and its result may be discarded.
 *
 *        Whether read() is [[nodiscard]], and isInitialized() is, cannot be tested here: as
 *        Appendix B notes, GCC does not warn when the result of a virtual call is discarded, so
 *        discarding it compiles either way.
 */
TEST(Interface, ReadStoresIntoAReference)
{
    Double serial{};
    Interface& interface{serial};
    std::uint8_t byte{};
    EXPECT_TRUE((std::is_same<decltype(interface.read(byte)), bool>::value));
    EXPECT_TRUE(noexcept(interface.read(byte)));
    serial.myLastByte = 0x5AU;
    interface.read(byte);
    EXPECT_EQ(static_cast<unsigned>(byte), 0x5AU);
}
