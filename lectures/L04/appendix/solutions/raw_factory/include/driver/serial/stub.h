/**
 * @brief Serial driver stub.
 */
#pragma once

#include <cstdint>

#include "driver/serial/interface.h"

namespace driver::serial
{
/**
 * @brief Serial driver stub.
 * 
 *        This class is non-copyable and non-movable.
 */
class Stub final : public Interface
{
public:
    /**
     * @brief Constructor.
     */
    Stub() noexcept
        : myLastByte{}
        , myInitialized{true}
        , myHasData{false}
    {}

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /**
     * @brief Check if the serial driver has been initialized.
     * 
     * @return True if initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return myInitialized; }

    /**
     * @brief Transmit one byte of data.
     * 
     * @param[in] byte Byte to transmit.
     */
    void write(const std::uint8_t byte) noexcept override
    {
        if (!myInitialized) { return; }
        myLastByte = byte;
        myHasData = true;
    }

    /**
     * @brief Receive one byte of data.
     * 
     * @param[out] byte Received byte (if any).
     * 
     * @return True if a byte was received, false otherwise.
     */
    bool read(std::uint8_t& byte) noexcept override
    {
        if (!myInitialized || !myHasData) { return false; }
        byte = myLastByte;
        myHasData = false;
        return true;
    }

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.

private:
    /** Most recently transmitted byte. */
    std::uint8_t myLastByte;

    /** True if initialized, false if not. */
    bool myInitialized;

    /** True if data is available, false otherwise. */
    bool myHasData;
};
} // namespace driver::serial
