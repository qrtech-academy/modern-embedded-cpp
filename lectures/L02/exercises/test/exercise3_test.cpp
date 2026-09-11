/**
 * @file Tests for Exercise Set 3: the Buzzer class, declared in
 *       exercise3/include/driver/buzzer.hpp and defined in exercise3/source/driver/buzzer.cpp,
 *       and the program in exercise3/source/main.cpp once it exists.
 *
 *       The buzzer prints when it is destroyed, and what it prints is part of the specification,
 *       so every test that destroys one captures the output.
 */
#include <cstdint>
#include <type_traits>

#include "driver/buzzer.hpp"
#include "qacademy/test/test.hpp"
#include "support/output.hpp"
#include "support/program.hpp"

using driver::Buzzer;
using support::captureOutput;

/**
 * @brief Exercise 3.1 b): the constructor takes a pin and an optional state, and is explicit and
 *        noexcept.
 */
TEST(Buzzer, ConstructorIsExplicitAndNoexcept)
{
    EXPECT_TRUE((std::is_nothrow_constructible<Buzzer, std::uint8_t>::value));
    EXPECT_TRUE((std::is_nothrow_constructible<Buzzer, std::uint8_t, bool>::value));
    EXPECT_FALSE((std::is_convertible<std::uint8_t, Buzzer>::value));
    EXPECT_TRUE(std::is_nothrow_destructible<Buzzer>::value);
}

/**
 * @brief Exercise 3.1 e): the default constructor and every copy and move operation are deleted,
 *        and the class is final, as declared.
 */
TEST(Buzzer, CannotBeDefaultedCopiedOrMoved)
{
    EXPECT_FALSE(std::is_default_constructible<Buzzer>::value);
    EXPECT_FALSE(std::is_copy_constructible<Buzzer>::value);
    EXPECT_FALSE(std::is_move_constructible<Buzzer>::value);
    EXPECT_FALSE(std::is_copy_assignable<Buzzer>::value);
    EXPECT_FALSE(std::is_move_assignable<Buzzer>::value);
    EXPECT_TRUE(std::is_final<Buzzer>::value);
}

/**
 * @brief Exercise 3.1 c): destroying the buzzer prints the release message with its own pin.
 */
TEST(Buzzer, AnnouncesItsReleaseWithItsPin)
{
    EXPECT_OUTPUT(captureOutput([] { const Buzzer buzzer{8U}; }),
                  "Releasing resources allocated for buzzer at pin 8!\n");
    EXPECT_OUTPUT(captureOutput([] { const Buzzer buzzer{5U, true}; }),
                  "Releasing resources allocated for buzzer at pin 5!\n");
}

/**
 * @brief Exercise 3.1 b) and d): the buzzer reports its pin, and starts disabled unless told
 *        otherwise.
 */
TEST(Buzzer, ReportsItsPinAndInitialState)
{
    captureOutput(
        []
        {
            const Buzzer quiet{8U};
            const Buzzer loud{9U, true};
            EXPECT_EQ(static_cast<unsigned>(quiet.pin()), 8U);
            EXPECT_EQ(static_cast<unsigned>(loud.pin()), 9U);
            EXPECT_FALSE(quiet.isEnabled());
            EXPECT_TRUE(loud.isEnabled());
            EXPECT_TRUE(noexcept(quiet.pin()));
            EXPECT_TRUE(noexcept(quiet.isEnabled()));
        });
}

/**
 * @brief Exercise 3.1 d): enable(), disable() and toggle() set, clear and flip the state, and are
 *        noexcept.
 */
TEST(Buzzer, EnableDisableAndToggle)
{
    captureOutput(
        []
        {
            Buzzer buzzer{8U};
            buzzer.enable();
            EXPECT_TRUE(buzzer.isEnabled());
            buzzer.disable();
            EXPECT_FALSE(buzzer.isEnabled());
            buzzer.toggle();
            EXPECT_TRUE(buzzer.isEnabled());
            buzzer.toggle();
            EXPECT_FALSE(buzzer.isEnabled());
            EXPECT_TRUE(noexcept(buzzer.enable()));
            EXPECT_TRUE(noexcept(buzzer.disable()));
            EXPECT_TRUE(noexcept(buzzer.toggle()));
        });
}

#ifdef PROGRAM

/**
 * @brief Exercise 3.1 f): the program prints the example output, ending with the destructor's
 *        message as the buzzer goes out of scope.
 */
TEST(Program, PrintsTheExampleOutput)
{
    EXPECT_PROGRAM_OUTPUT("Initial state: Disabled\n"
                          "After enable(): Enabled\n"
                          "After toggle(): Disabled\n"
                          "Releasing resources allocated for buzzer at pin 8!\n");
}

#endif
