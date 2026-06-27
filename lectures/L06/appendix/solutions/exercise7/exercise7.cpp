/**
 * @brief Multithreading solutions - Exercise Set 7.
 */
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <future>
#include <thread>

namespace
{
/**
 * @brief Halt the running thread.
 * 
 * @param[in] ms Sleep duration in milliseconds.
 */
void sleep_ms(const std::uint16_t ms) noexcept
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

bool validateFirmware(const std::uint8_t* buf, const std::uint16_t bufLen) noexcept
{
    constexpr std::uint16_t flashReadDuration_ms{2000U};
    constexpr std::uint8_t unprogrammedFlash{0xFFU};
    if ((nullptr == buf) || (0U == bufLen)) { return false; }

    // Simulate slow read.
    sleep_ms(flashReadDuration_ms);

    // Look for unprogrammed flash, return false if found.
    for (std::uint16_t i{}; i < bufLen; ++i)
    {
        if (unprogrammedFlash == buf[i]) { return false; }
    }
    return true;
}

} // namespace

/**
 * @brief Application entry point.
 *
 * @return 0 on success.
 */
int main()
{
    constexpr std::uint16_t pollSpeed_ms{200U};
    const std::uint8_t firmware[]{0x32U, 0xAFU, 0x07U, 0x55U, 0x00U, 
                                  0xFEU, 0xEFU, 0xA1U, 0x47U, 0x63U};
    const auto fwLen = static_cast<std::uint16_t>(sizeof(firmware) / sizeof(firmware[0U]));

    auto future = std::async(std::launch::async, validateFirmware, firmware, fwLen);
    std::printf("Booting system while validating firmware...\n");

    while (std::future_status::ready != future.wait_for(std::chrono::milliseconds(pollSpeed_ms)))
    {
        std::printf("Still waiting for firmware validation...\n");
    }

    const auto fwValid = future.get();
    std::printf("Firmware valid: %s!\n", fwValid ? "yes" : "no");
    return 0;
}
