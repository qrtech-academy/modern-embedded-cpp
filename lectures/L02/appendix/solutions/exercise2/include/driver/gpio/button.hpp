/**
 * @file Button driver.
 */
#pragma once

#include <cstdint>

namespace driver::gpio
{
/**
 * @brief Button driver.
 *
 *        This class is non-copyable and non-movable.
 */
class Button final
{
public:
    /**
     * @brief Constructor.
     *
     * @param[in] pin Button pin.
     */
    explicit Button(const std::uint8_t pin) noexcept
        : myPin{pin}
        , myPressed{false}
    {}

    /**
     * @brief Destructor.
     */
    ~Button() noexcept = default;

    /**
     * @brief Get the button pin.
     *
     * @return The button pin number.
     */
    [[nodiscard]] std::uint8_t pin() const noexcept { return myPin; }

    /**
     * @brief Check if the button is pressed.
     *
     * @return True if the button is pressed, false otherwise.
     */
    [[nodiscard]] bool isPressed() const noexcept { return myPressed; }

    /**
     * @brief Set button state.
     *
     * @param[in] pressed True if the button is pressed, false otherwise.
     */
    void setPressed(const bool pressed) noexcept { myPressed = pressed; }

    Button()                         = delete; // No default constructor.
    Button(const Button&)            = delete; // No copy constructor.
    Button(Button&&)                 = delete; // No move constructor.
    Button& operator=(const Button&) = delete; // No copy assignment.
    Button& operator=(Button&&)      = delete; // No move assignment.

private:
    /** Button pin. */
    const std::uint8_t myPin;

    /** True if the button is pressed, false otherwise. */
    bool myPressed;
};
} // namespace driver::gpio