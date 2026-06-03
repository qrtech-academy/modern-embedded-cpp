/**
 * @brief Serial driver interface.
 */
#pragma once

#include <cstdint>

namespace driver::serial
{
/**
 * @brief Serial driver interface.
 */
class Interface
{
public:
    /**
     * @brief Destructor.
     */
    virtual ~Interface() noexcept = default;

    /**
     * @brief Check if the serial driver has been initialized.
     * 
     * @return True if initialized, false otherwise.
     */
    virtual bool isInitialized() const noexcept = 0;

    /**
     * @brief Transmit one byte of data.
     * 
     * @param[in] byte Byte to transmit.
     */
    virtual void write(std::uint8_t byte) noexcept = 0;

    /**
     * @brief Receive one byte of data.
     * 
     * @param[out] byte Received byte (if any).
     * 
     * @return True if a byte was received, false otherwise.
     */
    virtual bool read(std::uint8_t& byte) noexcept = 0;
};
} // namespace driver::serial
