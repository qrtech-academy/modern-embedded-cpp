/**
 * @brief Serial console driver.
 */
#pragma once

#include <cstdint>

#include "driver/serial/interface.h"

namespace driver::serial
{
/**
 * @brief Serial console driver.
 * 
 *        This class follows the singleton pattern and is non-copyable and non-movable.
 */
class Console final : public Interface
{
public:
    /**
     * @brief Get the console driver instance.
     * 
     * @return Reference to the singleton console driver instance.
     */
    static Interface& instance() noexcept;

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

    Console(const Console&)            = delete; // No copy constructor.
    Console(Console&&)                 = delete; // No move constructor.
    Console& operator=(const Console&) = delete; // No copy assignment.
    Console& operator=(Console&&)      = delete; // No move assignment.

private:
    Console() noexcept           = default;
    ~Console() noexcept override = default;
};
} // namespace driver::serial
