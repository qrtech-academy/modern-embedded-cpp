/**
 * @file Serial driver stub implementation details.
 */
#include <cstdint>
#include <optional>

#include "driver/serial/stub.hpp"

namespace driver::serial
{
// -----------------------------------------------------------------------------
Stub::Stub() noexcept
    : myLastByte{std::nullopt}
    , myInitialized{true}
{}

// -----------------------------------------------------------------------------
bool Stub::isInitialized() const noexcept { return myInitialized; }

// -----------------------------------------------------------------------------
void Stub::write(const std::uint8_t byte) noexcept
{
    if (myInitialized) { myLastByte = byte; }
}

// -----------------------------------------------------------------------------
std::optional<std::uint8_t> Stub::read() noexcept
{
    const auto byte = myLastByte;
    myLastByte      = std::nullopt;
    return byte;
}

// -----------------------------------------------------------------------------
void Stub::setInitialized(const bool initialized) noexcept
{
    myInitialized = initialized;
    if (!myInitialized) { myLastByte = std::nullopt; }
}
} // namespace driver::serial
