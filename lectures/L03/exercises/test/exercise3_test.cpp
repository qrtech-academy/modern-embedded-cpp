/**
 * @file Tests for Exercise Set 3: the console driver, a singleton, declared in
 *       include/driver/serial/console.hpp and defined in source/driver/serial/console.cpp.
 *
 *       What makes a singleton is what cannot be done with it: it cannot be created, destroyed,
 *       copied or moved by anyone else, and these tests check each of those with a type trait.
 *       The console prints what it is given, so every write is captured.
 */
#include <cstdint>
#include <type_traits>

#include "driver/serial/console.hpp"
#include "qacademy/test/test.hpp"
#include "support/output.hpp"

using support::captureOutput;

namespace
{
using driver::serial::Console;
using driver::serial::Interface;
} // namespace

/**
 * @brief Exercise 3.1: the console inherits the interface publicly and is marked final.
 */
TEST(Console, ImplementsTheInterface)
{
    EXPECT_TRUE((std::is_base_of<Interface, Console>::value));
    EXPECT_TRUE((std::is_convertible<Console*, Interface*>::value));
    EXPECT_TRUE(std::is_final<Console>::value);
}

/**
 * @brief Exercise 3.1 a) and b): nobody else can create or destroy a console, because its
 *        constructor and destructor are private.
 */
TEST(Console, CannotBeCreatedOrDestroyedOutsideTheClass)
{
    EXPECT_FALSE(std::is_default_constructible<Console>::value);
    EXPECT_FALSE(std::is_destructible<Console>::value);
}

/**
 * @brief Exercise 3.1 c): the one instance cannot be copied or moved, neither constructed nor
 *        assigned.
 */
TEST(Console, CannotBeCopiedOrMoved)
{
    EXPECT_FALSE(std::is_copy_constructible<Console>::value);
    EXPECT_FALSE(std::is_move_constructible<Console>::value);
    EXPECT_FALSE(std::is_copy_assignable<Console>::value);
    EXPECT_FALSE(std::is_move_assignable<Console>::value);
}

/**
 * @brief Exercise 3.1 d): instance() is static and noexcept, returns a reference to the interface,
 *        and returns the same console every time.
 */
TEST(Console, InstanceIsTheOneConsole)
{
    EXPECT_TRUE((std::is_same<decltype(Console::instance()), Interface&>::value));
    EXPECT_TRUE(noexcept(Console::instance()));
    Interface& first{Console::instance()};
    Interface& second{Console::instance()};
    EXPECT_TRUE(&first == &second);
    EXPECT_TRUE(nullptr != dynamic_cast<Console*>(&first));
}

/**
 * @brief Exercise 3.1 e): the console is always initialized, and cannot be read from.
 */
TEST(Console, IsAlwaysInitializedAndWriteOnly)
{
    Interface& console{Console::instance()};
    std::uint8_t byte{};
    EXPECT_TRUE(console.isInitialized());
    EXPECT_FALSE(console.read(byte));
    captureOutput([&] { console.write('A'); });
    EXPECT_FALSE(console.read(byte));
}

/**
 * @brief Exercise 3.1 e): write() prints each byte as the character it is, and nothing else.
 */
TEST(Console, WritePrintsTheCharacter)
{
    Interface& console{Console::instance()};
    EXPECT_OUTPUT(captureOutput([&] { console.write('A'); }), "A");
    EXPECT_OUTPUT(captureOutput(
                      [&]
                      {
                          console.write('H');
                          console.write('i');
                          console.write('!');
                          console.write('\n');
                      }),
                  "Hi!\n");
}
