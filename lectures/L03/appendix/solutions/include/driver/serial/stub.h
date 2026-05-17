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
    Stub() noexcept;

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /**
     * @brief Check if the driver is initialized.
     * 
     * @return True if initialized, false otherwise.
     */
    bool isInitialized() const noexcept override;

    /**
     * @brief Transmit one byte of data.
     * 
     * @param[in] byte Byte to transmit.
     */
    void write(std::uint8_t byte) noexcept override;

    /**
     * @brief Receive one byte of data.
     * 
     * @param[out] byte Received byte (if any).
     * 
     * @return True if a byte was received, false otherwise.
     */
    bool read(std::uint8_t& byte) noexcept override;

    /**
     * @brief Set the simulated initialization state.
     * 
     * @param[in] initialized True if the driver shall be considered initialized, false otherwise.
     */
    void setInitialized(bool initialized) noexcept;

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.

private:
    /** Last transmitted byte. */
    std::uint8_t myLastByte;

    /** Initialization status (true = initialized). */
    bool myInitialized;

    /** Data availability flag (true = available). */
    bool myHasData;
};
} // namespace driver::serial
