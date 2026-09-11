/**
 * @file Tests for Exercise Set 1: the Led class in exercise1/include/driver/gpio/led.hpp, the
 *       Button class beside it once it exists, and the program in exercise1/main.cpp once both do.
 *
 *       Much of what the exercise asks for is a declaration rather than a behaviour: explicit,
 *       noexcept, final, a defaulted destructor, deleted copy and move operations. Each of those
 *       is visible to the compiler, so the tests ask it with type traits and noexcept(...), and a
 *       class that behaves correctly but is missing one of them fails.
 */
#include <cstdint>
#include <type_traits>

#include "driver/gpio/led.hpp"
#include "qacademy/test/test.hpp"
#include "support/output.hpp"

using driver::gpio::Led;
using support::captureOutput;

/**
 * @brief Exercise 1.1 b): the constructor takes a pin and an optional state, and is explicit, so
 *        a pin number does not turn into an LED on its own.
 */
TEST(Led, ConstructorIsExplicitAndNoexcept)
{
    EXPECT_TRUE((std::is_nothrow_constructible<Led, std::uint8_t>::value));
    EXPECT_TRUE((std::is_nothrow_constructible<Led, std::uint8_t, bool>::value));
    EXPECT_FALSE((std::is_convertible<std::uint8_t, Led>::value));
}

/**
 * @brief Exercise 1.1 c): the destructor is noexcept and defaulted. A defaulted destructor of a
 *        class holding only a pin and a bool is trivial; one written out by hand is not.
 */
TEST(Led, DestructorIsDefaulted)
{
    EXPECT_TRUE(std::is_nothrow_destructible<Led>::value);
    EXPECT_TRUE(std::is_trivially_destructible<Led>::value);
}

/**
 * @brief Exercise 1.1 e): the default constructor and every copy and move operation are deleted,
 *        and the class is final, as declared.
 */
TEST(Led, CannotBeDefaultedCopiedOrMoved)
{
    EXPECT_FALSE(std::is_default_constructible<Led>::value);
    EXPECT_FALSE(std::is_copy_constructible<Led>::value);
    EXPECT_FALSE(std::is_move_constructible<Led>::value);
    EXPECT_FALSE(std::is_copy_assignable<Led>::value);
    EXPECT_FALSE(std::is_move_assignable<Led>::value);
    EXPECT_TRUE(std::is_final<Led>::value);
}

/**
 * @brief Exercise 1.1 b): the LED starts off unless told otherwise.
 */
TEST(Led, StartsInTheGivenState)
{
    const Led off{13U};
    const Led on{13U, true};
    EXPECT_FALSE(off.isOn());
    EXPECT_TRUE(on.isOn());
    EXPECT_TRUE(noexcept(off.isOn()));
}

/**
 * @brief Exercise 1.1 d): on(), off() and toggle() set, clear and flip the state, and are
 *        noexcept.
 */
TEST(Led, OnOffAndToggle)
{
    Led led{13U};
    led.on();
    EXPECT_TRUE(led.isOn());
    led.on();
    EXPECT_TRUE(led.isOn());
    led.off();
    EXPECT_FALSE(led.isOn());
    led.toggle();
    EXPECT_TRUE(led.isOn());
    led.toggle();
    EXPECT_FALSE(led.isOn());
    EXPECT_TRUE(noexcept(led.on()));
    EXPECT_TRUE(noexcept(led.off()));
    EXPECT_TRUE(noexcept(led.toggle()));
}

#if __has_include("driver/gpio/button.hpp")
#include "driver/gpio/button.hpp"

using driver::gpio::Button;

/**
 * @brief Exercise 1.2 b) and c): the constructor takes a pin, is explicit and noexcept, and the
 *        destructor is noexcept and defaulted.
 */
TEST(Button, ConstructorIsExplicitAndDestructorDefaulted)
{
    EXPECT_TRUE((std::is_nothrow_constructible<Button, std::uint8_t>::value));
    EXPECT_FALSE((std::is_convertible<std::uint8_t, Button>::value));
    EXPECT_TRUE(std::is_nothrow_destructible<Button>::value);
    EXPECT_TRUE(std::is_trivially_destructible<Button>::value);
}

/**
 * @brief Exercise 1.2 e): the default constructor and every copy and move operation are deleted,
 *        and the class is final, as declared.
 */
TEST(Button, CannotBeDefaultedCopiedOrMoved)
{
    EXPECT_FALSE(std::is_default_constructible<Button>::value);
    EXPECT_FALSE(std::is_copy_constructible<Button>::value);
    EXPECT_FALSE(std::is_move_constructible<Button>::value);
    EXPECT_FALSE(std::is_copy_assignable<Button>::value);
    EXPECT_FALSE(std::is_move_assignable<Button>::value);
    EXPECT_TRUE(std::is_final<Button>::value);
}

/**
 * @brief Exercise 1.2 b) and d): a new button reports its pin and starts released.
 */
TEST(Button, ReportsItsPinAndStartsReleased)
{
    const Button button{2U};
    EXPECT_EQ(static_cast<unsigned>(button.pin()), 2U);
    EXPECT_FALSE(button.isPressed());
    EXPECT_TRUE(noexcept(button.pin()));
    EXPECT_TRUE(noexcept(button.isPressed()));
}

/**
 * @brief Exercise 1.2 d): setPressed() updates the state, both ways, and is noexcept.
 */
TEST(Button, SetPressedUpdatesTheState)
{
    Button button{7U};
    button.setPressed(true);
    EXPECT_TRUE(button.isPressed());
    button.setPressed(false);
    EXPECT_FALSE(button.isPressed());
    EXPECT_TRUE(noexcept(button.setPressed(true)));
}

#if __has_include("main.cpp")

// Your program, with its main() renamed so this file can run it. A main() may leave out its
// return statement and any other function may not, so that one warning is silenced here.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise1Main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

/**
 * @brief Exercises 1.1 f) and 1.2 f): the program prints both example outputs, in order.
 */
TEST(Program, PrintsTheExampleOutput)
{
    EXPECT_OUTPUT(captureOutput([] { exercise1Main(); }), "Initial state: Off\n"
                                                          "After on(): On\n"
                                                          "After toggle(): Off\n"
                                                          "Button pin: 2\n"
                                                          "Pressed: Yes\n");
}

#endif
#endif
