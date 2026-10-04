/**
 * @file Serial driver stub.
 */
#pragma once

#include <cstdint>
#include <optional>

#include "driver/serial/interface.hpp"

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
        : myLastByte{std::nullopt}
        , myInitialized{true}
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
    [[nodiscard]] bool isInitialized() const noexcept override { return myInitialized; }

    /**
     * @brief Transmit one byte of data.
     *
     * @param[in] byte Byte to transmit.
     */
    void write(const std::uint8_t byte) noexcept override
    {
        if (!myInitialized) { return; }
        myLastByte = byte;
    }

    /**
     * @brief Receive one byte of data.
     *
     * @return Received byte, or std::nullopt if no byte was available.
     */
    [[nodiscard]] std::optional<std::uint8_t> read() noexcept override
    {
        if (!myInitialized) { return std::nullopt; }
        const auto byte = myLastByte;
        myLastByte      = std::nullopt;
        return byte;
    }

    /**
     * @brief Set the simulated initialization state.
     *
     * @param[in] initialized True if the driver shall be considered initialized, false otherwise.
     */
    void setInitialized(const bool initialized) noexcept
    {
        myInitialized = initialized;
        myLastByte    = std::nullopt;
    }

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy assignment.
    Stub& operator=(Stub&&)      = delete; // No move assignment.

private:
    /** Most recently transmitted byte,. */
    std::optional<std::uint8_t> myLastByte;

    /** True if initialized, false if not. */
    bool myInitialized;
};
} // namespace driver::serial
