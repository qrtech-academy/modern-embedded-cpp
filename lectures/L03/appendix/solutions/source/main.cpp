/**
 * @brief Solution for L03 exercises.
 */
#include <cstdint>
#include <cstdio>

#include "driver/serial/console.h"
#include "driver/serial/stub.h"

using namespace driver;

namespace
{
// -----------------------------------------------------------------------------
void sendMessage(serial::Interface& serial) noexcept
{
    constexpr const char* msg{"Transmitting data with a serial driver!"};
    constexpr char null{'\0'};

    // Transmit the message byte by byte.
    for (const char* c{msg}; *c != null; ++c)
    {
        const std::uint8_t byte{static_cast<std::uint8_t>(*c)};
        serial.write(byte);
    }
}
} // namespace

/**
 * @brief Create and use serial drivers.
 * 
 * @return Exit status (0 = success).
 */
int main()
{
    // Test case 1 - Trying the console driver.
    {
        serial::Interface& serial{serial::Console::instance()};
        sendMessage(serial);
    }

    // Test case 2 - Trying the stub driver.
    {
        serial::Stub serial{};
        sendMessage(serial);
        std::uint8_t byte{};

        if (serial.read(byte))
        {
            const auto lastChar = static_cast<int>(byte);
            std::printf("\nLast received character from stub driver: %c\n", lastChar);
        }
    }
    return 0;
}
