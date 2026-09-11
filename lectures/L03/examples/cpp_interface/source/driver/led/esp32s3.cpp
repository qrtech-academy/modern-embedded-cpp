/**
 * @file LED driver implementation details for ESP32-S3.
 */
#include <cstdint>
#include <iostream>

#include "driver/led/esp32s3.hpp"

namespace driver::led
{
// -----------------------------------------------------------------------------
Esp32s3::Esp32s3(const std::uint8_t pin, const bool initialState) noexcept
    : myPin{pin}
    , myIsEnabled{false}
{
    init(initialState);
}

// -----------------------------------------------------------------------------
Esp32s3::~Esp32s3() noexcept
{
    // Insert code to release allocated resources here.
    std::cout << "Deleting LED connected to pin " << static_cast<int>(myPin)
              << " on processor ESP32-S3!\n";
}

// -----------------------------------------------------------------------------
std::uint8_t Esp32s3::pin() const noexcept { return myPin; }

// -----------------------------------------------------------------------------
bool Esp32s3::isEnabled() const noexcept { return myIsEnabled; }

// -----------------------------------------------------------------------------
void Esp32s3::setEnabled(const bool enable) noexcept
{
    // Insert code to enable the LED here.
    myIsEnabled = enable;
    const auto status{myIsEnabled ? "Enabling" : "Disabling"};
    std::cout << status << " LED connected to pin " << static_cast<int>(myPin) << "!\n";
}

// -----------------------------------------------------------------------------
void Esp32s3::toggle() noexcept { setEnabled(!myIsEnabled); }

// -----------------------------------------------------------------------------
void Esp32s3::init(const bool initialState) noexcept
{
    // Insert code to initialize the hardware here.
    std::cout << "Initializing LED connected to pin " << static_cast<int>(myPin)
              << " on processor ESP32-S3, starting value = " << initialState << "!\n";
    setEnabled(initialState);
}
} // namespace driver::led
