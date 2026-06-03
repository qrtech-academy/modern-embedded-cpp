/**
 * @brief Serial driver for ESP32-S3.
 */
#pragma once

#include <cstdint>
#include <cstdio>

#include "driver/serial/interface.h"

namespace driver::serial
{
/**
 * @brief Serial driver for ESP32-S3.
 * 
 *        This class is non-copyable and non-movable.
 */
class Esp32s3 final : public Interface
{
public:
    /**
     * @brief Constructor.
     * 
     * @param[in] txPin Transfer pin number.
     * @param[in] rxPin Receive pin number.
     */
    Esp32s3(const std::uint8_t txPin, const std::uint8_t rxPin) noexcept
        : myTxPin{txPin}
        , myRxPin{rxPin}
    {}

    /**
     * @brief Destructor.
     */
    ~Esp32s3() noexcept override = default;

    /**
     * @brief Check if the serial driver has been initialized.
     * 
     * @return True if initialized, false otherwise.
     */
    bool isInitialized() const noexcept override { return true; }

    /**
     * @brief Transmit one byte of data.
     * 
     * @param[in] byte Byte to transmit.
     */
    void write(const std::uint8_t byte) noexcept override
    {
        std::printf("Transmitting byte 0x%02X via TX pin %u!\n", byte, myTxPin);
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
        (void) (byte);
        return false;
    }

    Esp32s3()                          = delete; // No default constructor.
    Esp32s3(const Esp32s3&)            = delete; // No copy constructor.
    Esp32s3(Esp32s3&&)                 = delete; // No move constructor.
    Esp32s3& operator=(const Esp32s3&) = delete; // No copy assignment.
    Esp32s3& operator=(Esp32s3&&)      = delete; // No move assignment.

private:
    /** Transmit pin number. */
    const std::uint8_t myTxPin;

    /** Receive pin number. */
    const std::uint8_t myRxPin;
};
} // namespace driver::serial
