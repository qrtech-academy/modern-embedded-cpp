/**
 * @file LED driver implementation details for ATmega328P.
 */
#include <cstdint>
#include <iostream>

#include "driver/led/atmega328p.hpp"

namespace driver::led
{
// -----------------------------------------------------------------------------
Atmega328p::Atmega328p(const std::uint8_t pin) noexcept
    : myPin{pin}
    , myIsEnabled{false}
{
    // Insert code to initialize the hardware here.
    std::cout << "Initializing LED connected to pin " << static_cast<int>(myPin)
              << " on processor ATmega328P!\n";
}

// -----------------------------------------------------------------------------
Atmega328p::~Atmega328p() noexcept
{
    // Insert code to release allocated resources here.
    std::cout << "Deleting LED connected to pin " << static_cast<int>(myPin)
              << " on processor ATmega328P!\n";
}

// -----------------------------------------------------------------------------
std::uint8_t Atmega328p::pin() const noexcept { return myPin; }

// -----------------------------------------------------------------------------
bool Atmega328p::isEnabled() const noexcept { return myIsEnabled; }

// -----------------------------------------------------------------------------
void Atmega328p::setEnabled(const bool enable) noexcept
{
    // Insert code to enable the LED here.
    myIsEnabled = enable;
    const auto status{myIsEnabled ? "Enabling" : "Disabling"};
    std::cout << status << " LED connected to pin " << static_cast<int>(myPin) << "!\n";
}

// -----------------------------------------------------------------------------
void Atmega328p::toggle() noexcept { setEnabled(!myIsEnabled); }

} // namespace driver::led
