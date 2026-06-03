/**
 * @brief ESP32-S3 factory with raw pointers.
 */
#pragma once

#include <cstdint>

#include "driver/factory/interface.h"
#include "driver/serial/esp32s3.h"

namespace driver::factory
{
/**
 * @brief ESP32-S3 factory with raw pointers.
 * 
 *        This class is non-copyable and non-movable.
 */
class Esp32s3 final : public Interface
{
public:
    /**
     * @brief Constructor.
     */
    Esp32s3() noexcept = default;

    /**
     * @brief Destructor.
     */
    ~Esp32s3() noexcept override = default;

    /**
     * @brief Create serial driver.
     * 
     * @param[in] txPin Transfer pin number.
     * @param[in] rxPin Receive pin number.
     * 
     * @return Pointer to the serial device.
     */
    serial::Interface* serial(const std::uint8_t txPin, const std::uint8_t rxPin) noexcept override
    {
        return new serial::Esp32s3(txPin, rxPin);
    }

    Esp32s3(const Esp32s3&)            = delete; // No copy constructor.
    Esp32s3(Esp32s3&&)                 = delete; // No move constructor.
    Esp32s3& operator=(const Esp32s3&) = delete; // No copy assignment.
    Esp32s3& operator=(Esp32s3&&)      = delete; // No move assignment.
};
} // namespace driver::factory
