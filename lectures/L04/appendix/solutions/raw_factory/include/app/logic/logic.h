/**
 * @brief Application logic.
 */
#pragma once

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <thread>

#include "driver/factory/interface.h"
#include "driver/serial/interface.h"

namespace app::logic
{
/**
 * @brief Application logic.
 * 
 *        This class is non-copyable and non-movable.
 */
class Logic final
{
public:
    /**
     * @brief Constructor.
     * 
     * @param[in] factory Driver factory.
     * @param[in] txPin Transfer pin.
     * @param[in] rxPin Receive pin.
     */
    explicit Logic(driver::factory::Interface& factory, const std::uint8_t txPin, 
                   const std::uint8_t rxPin) noexcept
        : mySerial{factory.serial(txPin, rxPin)}
    {}

    /**
     * @brief Destructor.
     */
    ~Logic() noexcept
    {
        // Manually release the allocated memory to avoid a memory leak.
        delete mySerial;
    }

    /**
     * @brief Run system logic.
     */
    void run() noexcept
    {
        constexpr std::uint8_t sleep_ms{100U};
        std::uint8_t txByte{}, rxByte{};

        while (true)
        {
            mySerial->write(txByte++);

            // Print byte if received.
            if (mySerial->read(rxByte))
            {
                std::printf("RX byte: 0x%02X!\n", rxByte);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
        }
    }

    Logic()                        = delete; // No default constructor.
    Logic(const Logic&)            = delete; // No copy constructor.
    Logic(Logic&&)                 = delete; // No move constructor.
    Logic& operator=(const Logic&) = delete; // No copy assignment.
    Logic& operator=(Logic&&)      = delete; // No move assignment.

private:
    /** Serial driver. */
    driver::serial::Interface* mySerial;
};
} // namespace app::logic

