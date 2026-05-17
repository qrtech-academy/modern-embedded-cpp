/**
 * @brief Serial driver stub implementation details.
 */
#include <cstdint>

#include "driver/serial/stub.h"

namespace driver::serial
{
// -----------------------------------------------------------------------------
Stub::Stub() noexcept
    : myLastByte{}
    , myInitialized{true}
    , myHasData{false}
{}

// -----------------------------------------------------------------------------
bool Stub::isInitialized() const noexcept { return myInitialized; }

// -----------------------------------------------------------------------------
void Stub::write(const std::uint8_t byte) noexcept
{
    if (myInitialized)
    {
        myLastByte = byte;
        myHasData  = true;
    }
}

// -----------------------------------------------------------------------------
bool Stub::read(std::uint8_t& byte) noexcept
{
    // Retrieve last byte if present.
    if (myInitialized && myHasData)
    {
        byte      = myLastByte;
        myHasData = false;
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
void Stub::setInitialized(const bool initialized) noexcept
{
    myInitialized = initialized;
}
} // namespace driver::serial
