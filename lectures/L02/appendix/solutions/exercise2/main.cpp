/**
 * @file Solution for exercise set 2.
 */
#include <cstdint>
#include <cstdio>

#include "driver/gpio/button.hpp"
#include "driver/gpio/led.hpp"

using namespace driver;

namespace
{
// -----------------------------------------------------------------------------
void controlLed(const gpio::Button& button, gpio::Led& led) noexcept
{
    if (button.isPressed()) { led.on(); }
    else { led.off(); }

    const char* buttonState{button.isPressed() ? "pressed" : "released"};
    const char* ledState{led.isOn() ? "On" : "Off"};
    std::printf("Button %-8s -> LED: %s\n", buttonState, ledState);
}

// -----------------------------------------------------------------------------
void blinkLed(gpio::Led& led, const std::size_t iterationCount = 1U) noexcept
{
    for (std::size_t i{}; i < iterationCount; ++i)
    {
        led.toggle();
        const std::size_t blink{i + 1U};
        const char* state{led.isOn() ? "On" : "Off"};
        std::printf("Blink %zu: %s\n", blink, state);
    }
}
} // namespace

/**
 * @brief Create and use GPIO devices.
 *
 * @return Exit status (0 = success).
 */
int main()
{
    constexpr std::uint8_t buttonPin{2U};
    constexpr std::uint8_t ledPin{13U};

    // Create GPIO devices.
    driver::gpio::Button button{buttonPin};
    gpio::Led led{ledPin};

    // ------------------------------ Exercise 2.1 ---------------------------------
    controlLed(button, led);

    // Simulate button pressdown.
    button.setPressed(true);
    controlLed(button, led);

    // ------------------------------ Exercise 2.2 ---------------------------------
    // Start from the off state, so that the first toggle turns the LED on.
    constexpr std::size_t iterationCount{6U};
    led.off();
    blinkLed(led, iterationCount);
    return 0;
}
