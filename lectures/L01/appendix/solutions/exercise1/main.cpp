/**
 * @file Solution for exercise set 1.
 */
#include <cstdint>
#include <cstdio>

namespace debug
{
/**
 * @brief Log message.
 *
 * @param[in] message The message to log.
 * @param[in] level Log level (default = 0).
 */
void log(const char* message, const std::uint8_t level = 0U) noexcept
{
    std::printf("%s, log level = %u\n", message, level);
}
} // namespace debug

namespace app
{
/**
 * @brief Generate a software delay.
 *
 * @param[in] ms Duration in milliseconds (approximate, software-based delay).
 */
void delay_ms(const std::uint32_t ms = 1U) noexcept
{
    constexpr std::uint32_t maxCount{10000000UL};
    volatile std::uint32_t dummy{};

    for (std::uint32_t i{}; i < ms; ++i)
    {
        for (std::uint32_t j{}; j < maxCount; ++j)
        {
            dummy++;
        }
    }
}
} // namespace app

/**
 * @brief Print two debug logs with a short delay in between.
 *
 * @return Exit status (0 = success).
 */
int main()
{
    constexpr std::uint32_t delay_ms{100U};
    constexpr std::uint8_t errorLog{2U};

    debug::log("System started");
    app::delay_ms(delay_ms);
    debug::log("Sensor failure", errorLog);
    return 0;
}
