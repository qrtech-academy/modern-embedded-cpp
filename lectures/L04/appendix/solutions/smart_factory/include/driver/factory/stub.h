/**
 * @brief Stub factory with smart pointers.
 */
#pragma once

#include <cstdint>

#include "driver/factory/interface.h"
#include "driver/serial/stub.h"

namespace driver::factory
{
/**
 * @brief Stub factory with smart pointers.
 * 
 *        This class is non-copyable and non-movable.
 */
class Stub final : public Interface
{
public:
    /**
     * @brief Constructor.
     */
    Stub() noexcept = default;

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /**
     * @brief Create serial driver.
     * 
     * @param[in] txPin Transfer pin number.
     * @param[in] rxPin Receive pin number.
     * 
     * @return Pointer to the serial device.
     */
    std::unique_ptr<serial::Interface> serial(
        const std::uint8_t txPin, const std::uint8_t rxPin) noexcept override
    {
        (void) (txPin);
        (void) (rxPin);
        return std::make_unique<serial::Stub>();
    }

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.
};
} // namespace driver::factory
