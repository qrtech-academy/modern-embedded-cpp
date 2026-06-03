/**
 * @brief Factory interface with smart pointers.
 */
#pragma once

#include <cstdint>
#include <memory>

/** Serial driver interface. */
namespace driver::serial { class Interface; }

namespace driver::factory
{
/**
 * @brief Factory interface with smart pointers.
 */
class Interface
{
public:
    /**
     * @brief Destructor.
     */
    virtual ~Interface() noexcept = default;

    /**
     * @brief Create serial driver.
     * 
     * @param[in] txPin Transfer pin number.
     * @param[in] rxPin Receive pin number.
     * 
     * @return Pointer to the serial device.
     */
    virtual std::unique_ptr<serial::Interface> serial(
        std::uint8_t txPin, std::uint8_t rxPin) noexcept = 0;
};
} // namespace driver::factory
