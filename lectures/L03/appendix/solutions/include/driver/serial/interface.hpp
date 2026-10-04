/**
 * @file Serial driver interface.
 */
#pragma once

#include <cstdint>
#include <optional>

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
     * @brief Check if the driver is initialized.
     *
     * @return True if initialized, false otherwise.
     */
    [[nodiscard]] virtual bool isInitialized() const noexcept = 0;

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
     * @return Received byte, or std::nullopt if no byte was available.
     */
    [[nodiscard]] virtual std::optional<std::uint8_t> read() noexcept = 0;
};
} // namespace driver::serial
