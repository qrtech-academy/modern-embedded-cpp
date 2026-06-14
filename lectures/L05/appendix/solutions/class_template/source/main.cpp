/**
 * @brief Timer driver class template demonstration.
 */
#include <chrono>
#include <cstdint>
#include <thread>

#include "driver/timer/timer.h"

using namespace driver;

namespace
{
// -----------------------------------------------------------------------------
template<timer::Type T>
constexpr const char* timerType() noexcept
{
    if constexpr (T == timer::Type::Stub) { return "Stub"; }
    else if (T == timer::Type::Stm32) { return "STM32"; }
    return "Unknown";
}

// -----------------------------------------------------------------------------
template<timer::Type T>
void tickTimer(timer::Timer<T>& timer) noexcept
{
    constexpr const char* type{timerType<T>()};
    timer.tick();

    if (timer.hasTimedOut())
    {
        std::printf("%s timer has timed out after %u ms!\n", type, timer.timeout_ms());
    }
}

// -----------------------------------------------------------------------------
void delay_ms(const std::uint16_t ms) noexcept
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
} // namespace

/**
 * @brief Demonstrate the timer driver class template.
 *
 * @return 0 on successful program termination.
 */
int main()
{
    timer::Timer timer1{500U};
    timer::Timer<timer::Type::Stm32> timer2{1500U};
    timer1.start();
    timer2.start();

    constexpr std::uint16_t iterationCount{2000U};

    for (std::uint16_t i{}; i < iterationCount; ++i)
    {
        tickTimer(timer1);
        tickTimer(timer2);
        delay_ms(1U);
    }
    return 0;
}