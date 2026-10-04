/**
 * @file Tests for Exercise Set 2: the serial stub, declared in include/driver/serial/stub.hpp
 *       and defined in source/driver/serial/stub.cpp.
 *
 *       The stub is tested both directly and through a reference to the interface, because the
 *       point of a stub is to stand in for real hardware wherever code expects the interface.
 */
#include <cstdint>
#include <optional>
#include <type_traits>

#include "driver/serial/stub.hpp"
#include "qacademy/test/test.hpp"

namespace
{
using driver::serial::Interface;
using driver::serial::Stub;
} // namespace

/**
 * @brief Exercise 2.1: the stub inherits the interface publicly and is marked final.
 */
TEST(Stub, ImplementsTheInterface)
{
    EXPECT_TRUE((std::is_base_of<Interface, Stub>::value));
    EXPECT_TRUE((std::is_convertible<Stub*, Interface*>::value));
    EXPECT_TRUE(std::is_final<Stub>::value);
    EXPECT_FALSE(std::is_abstract<Stub>::value);
}

/**
 * @brief Exercise 2.1 c): the stub cannot be copied or moved, neither constructed nor assigned.
 */
TEST(Stub, CannotBeCopiedOrMoved)
{
    EXPECT_FALSE(std::is_copy_constructible<Stub>::value);
    EXPECT_FALSE(std::is_move_constructible<Stub>::value);
    EXPECT_FALSE(std::is_copy_assignable<Stub>::value);
    EXPECT_FALSE(std::is_move_assignable<Stub>::value);
}

/**
 * @brief Exercise 2.1 b): a new stub is initialized and has nothing to read.
 */
TEST(Stub, StartsInitializedAndEmpty)
{
    Stub stub{};
    EXPECT_TRUE(stub.isInitialized());
    EXPECT_FALSE(stub.read().has_value());
}

/**
 * @brief Exercise 2.1 d): read() returns a std::optional<std::uint8_t>, and takes no parameters.
 */
TEST(Stub, ReadReturnsAnOptionalByte)
{
    Stub stub{};
    EXPECT_TRUE((std::is_same<decltype(stub.read()), std::optional<std::uint8_t>>::value));
}

/**
 * @brief Exercise 2.1 d): a written byte can be read back once, and then there is nothing more
 *        to read.
 */
TEST(Stub, ReadsBackTheByteWrittenOnce)
{
    Stub stub{};
    stub.write(0x41U);
    const std::optional<std::uint8_t> byte{stub.read()};
    EXPECT_TRUE(byte.has_value());
    EXPECT_EQ(static_cast<unsigned>(byte.value_or(0U)), 0x41U);
    EXPECT_FALSE(stub.read().has_value());
}

/**
 * @brief Exercise 2.1 d): the stub keeps the most recently transmitted byte, not the first.
 */
TEST(Stub, KeepsTheLastByte)
{
    Stub stub{};
    stub.write(0x01U);
    stub.write(0x02U);
    stub.write(0xFFU);
    EXPECT_EQ(static_cast<unsigned>(stub.read().value_or(0U)), 0xFFU);
}

/**
 * @brief Exercise 2.1 d): a zero byte is a byte. An optional holding 0 is not an empty optional,
 *        which is the case a value and a flag kept apart tend to get wrong.
 */
TEST(Stub, ReadsBackAZeroByte)
{
    Stub stub{};
    stub.write(0x00U);
    const std::optional<std::uint8_t> byte{stub.read()};
    EXPECT_TRUE(byte.has_value());
    EXPECT_EQ(static_cast<unsigned>(byte.value_or(0xFFU)), 0x00U);
    EXPECT_FALSE(stub.read().has_value());
}

/**
 * @brief Exercise 2.1 d) and e): an uninitialized stub reports so, ignores writes, and has nothing
 *        to read; initializing it again does not bring back the ignored byte.
 */
TEST(Stub, UninitializedStubDoesNothing)
{
    Stub stub{};
    stub.setInitialized(false);
    EXPECT_FALSE(stub.isInitialized());
    stub.write(0x41U);
    EXPECT_FALSE(stub.read().has_value());
    stub.setInitialized(true);
    EXPECT_TRUE(stub.isInitialized());
    EXPECT_FALSE(stub.read().has_value());
}

/**
 * @brief Exercise 2.1 e): setting the stub to uninitialized discards the byte it holds, so there
 *        is nothing to read, and initializing it again does not bring the byte back.
 */
TEST(Stub, UninitializingDiscardsTheStoredByte)
{
    Stub stub{};
    stub.write(0x41U);
    stub.setInitialized(false);
    EXPECT_FALSE(stub.read().has_value());
    stub.setInitialized(true);
    EXPECT_FALSE(stub.read().has_value());
}

/**
 * @brief Exercise 2.1 d): through a reference to the interface, the stub behaves the same, and
 *        its methods are noexcept as the interface requires.
 */
TEST(Stub, WorksThroughTheInterface)
{
    Stub stub{};
    Interface& serial{stub};
    EXPECT_TRUE(noexcept(serial.write(0U)));
    EXPECT_TRUE(noexcept(serial.read()));
    EXPECT_TRUE(noexcept(stub.isInitialized()));
    serial.write('!');
    EXPECT_TRUE(serial.isInitialized());
    const std::optional<std::uint8_t> byte{serial.read()};
    EXPECT_TRUE(byte.has_value());
    EXPECT_EQ(static_cast<char>(byte.value_or(0U)), '!');
}
