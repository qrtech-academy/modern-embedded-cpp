/**
 * @file Solution for exercise set 1.
 */
#include <cstdint>
#include <cstdio>

#include "driver/gpio/button.hpp"
#include "driver/gpio/led.hpp"

using namespace driver;

namespace
{
// -----------------------------------------------------------------------------
void printState(const char* label, const gpio::Led& led) noexcept
{
    const char* state{led.isOn() ? "On" : "Off"};
    std::printf("%s: %s\n", label, state);
}

// -----------------------------------------------------------------------------
void printState(const gpio::Button& button) noexcept
{
    const char* state{button.isPressed() ? "Yes" : "No"};
    std::printf("Button pin: %u\n", button.pin());
    std::printf("Pressed: %s\n", state);
}
} // namespace

/**
 * @brief Create and use GPIO devices.
 *
 * @return Exit status (0 = success).
 */
int main()
{
    constexpr std::uint8_t ledPin{13U};
    constexpr std::uint8_t buttonPin{2U};

    // ------------------------------ Exercise 1.1 ---------------------------------
    gpio::Led led{ledPin};
    printState("Initial state", led);

    led.on();
    printState("After on()", led);

    led.toggle();
    printState("After toggle()", led);

    // ------------------------------ Exercise 1.2 ---------------------------------
    driver::gpio::Button button{buttonPin};
    button.setPressed(true);
    printState(button);
    return 0;
}
