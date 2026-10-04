/**
 * @file Serial console driver implementation details.
 */
#include <cstdint>
#include <cstdio>
#include <optional>

#include "driver/serial/console.hpp"

namespace driver::serial
{
// -----------------------------------------------------------------------------
Interface& Console::instance() noexcept
{
    // Create singleton console driver instance (done once).
    static Console instance{};

    // Return reference to the instance.
    return instance;
}

// -----------------------------------------------------------------------------
bool Console::isInitialized() const noexcept
{
    // The console driver is always available.
    return true;
}

// -----------------------------------------------------------------------------
void Console::write(const std::uint8_t byte) noexcept
{
    // Print byte to the console, cast to the corresponding character.
    std::printf("%c", static_cast<int>(byte));
}

// -----------------------------------------------------------------------------
std::optional<std::uint8_t> Console::read() noexcept
{
    // Reading is not supported in this implementation.
    return std::nullopt;
}
} // namespace driver::serial
