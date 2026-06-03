/**
 * @brief Application using a driver factory with raw pointers.
 */
#include <cstdint>

#include "app/logic/logic.h"

// Define STUB to use stub drivers instead of ESP32-S3 drivers.
#define STUB

// Use stub drivers for testing.
#ifdef STUB
#include "driver/factory/stub.h"
using Factory = driver::factory::Stub;
#else
#include "driver/factory/esp32s3.h"
using Factory = driver::factory::Esp32s3;
#endif // STUB

/**
 * @brief Run application with serial driver.
 * 
 * @return 0 (should never occur).
 */
int main()
{
    constexpr std::uint8_t txPin{17U};
    constexpr std::uint8_t rxPin{18U};

    Factory factory{};
    app::logic::Logic app{factory, txPin, rxPin};
    app.run();
    return 0;
}