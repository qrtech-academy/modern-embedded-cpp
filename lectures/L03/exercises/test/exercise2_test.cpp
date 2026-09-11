/**
 * @file Tests for Exercise Set 2: the serial stub, declared in include/driver/serial/stub.hpp
 *       and defined in source/driver/serial/stub.cpp.
 *
 *       The stub is tested both directly and through a reference to the interface, because the
 *       point of a stub is to stand in for real hardware wherever code expects the interface.
 */
#include <cstdint>
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
    std::uint8_t byte{};
    EXPECT_TRUE(stub.isInitialized());
    EXPECT_FALSE(stub.read(byte));
}

/**
 * @brief Exercise 2.1 d): a written byte can be read back once, and then there is nothing more
 *        to read.
 */
TEST(Stub, ReadsBackTheByteWrittenOnce)
{
    Stub stub{};
    std::uint8_t byte{};
    stub.write(0x41U);
    EXPECT_TRUE(stub.read(byte));
    EXPECT_EQ(static_cast<unsigned>(byte), 0x41U);
    EXPECT_FALSE(stub.read(byte));
}

/**
 * @brief Exercise 2.1 d): the stub keeps the most recently transmitted byte, not the first.
 */
TEST(Stub, KeepsTheLastByte)
{
    Stub stub{};
    std::uint8_t byte{};
    stub.write(0x01U);
    stub.write(0x02U);
    stub.write(0xFFU);
    EXPECT_TRUE(stub.read(byte));
    EXPECT_EQ(static_cast<unsigned>(byte), 0xFFU);
}

/**
 * @brief Exercise 2.1 d): read() copies into its argument only when it returns true.
 */
TEST(Stub, LeavesTheArgumentAloneWhenThereIsNothingToRead)
{
    Stub stub{};
    std::uint8_t byte{0x7EU};
    EXPECT_FALSE(stub.read(byte));
    EXPECT_EQ(static_cast<unsigned>(byte), 0x7EU);
}

/**
 * @brief Exercise 2.1 d) and e): an uninitialized stub reports so, ignores writes, and has nothing
 *        to read; initializing it again does not bring back the ignored byte.
 */
TEST(Stub, UninitializedStubDoesNothing)
{
    Stub stub{};
    std::uint8_t byte{};
    stub.setInitialized(false);
    EXPECT_FALSE(stub.isInitialized());
    stub.write(0x41U);
    EXPECT_FALSE(stub.read(byte));
    stub.setInitialized(true);
    EXPECT_TRUE(stub.isInitialized());
    EXPECT_FALSE(stub.read(byte));
}

/**
 * @brief Exercise 2.1 d): an uninitialized stub refuses to read even a byte it already holds.
 */
TEST(Stub, UninitializedStubDoesNotRead)
{
    Stub stub{};
    std::uint8_t byte{};
    stub.write(0x41U);
    stub.setInitialized(false);
    EXPECT_FALSE(stub.read(byte));
}

/**
 * @brief Exercise 2.1 d): through a reference to the interface, the stub behaves the same, and
 *        its methods are noexcept as the interface requires.
 */
TEST(Stub, WorksThroughTheInterface)
{
    Stub stub{};
    Interface& serial{stub};
    std::uint8_t byte{};
    EXPECT_TRUE(noexcept(serial.write(0U)));
    EXPECT_TRUE(noexcept(serial.read(byte)));
    EXPECT_TRUE(noexcept(stub.isInitialized()));
    serial.write('!');
    EXPECT_TRUE(serial.isInitialized());
    EXPECT_TRUE(serial.read(byte));
    EXPECT_EQ(static_cast<char>(byte), '!');
}
