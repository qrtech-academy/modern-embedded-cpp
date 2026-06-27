/**
 * @brief Multithreading solutions - Exercise Set 4-5.
 */
#include <cstdio>
#include <thread>

#include "driver/counter/stub.h"

namespace
{
/**
 * @brief Increment a counter a given number of times.
 *
 * @param[in,out] counter Counter to increment.
 * @param[in] increments Number of increments to perform.
 */
void counterThread(driver::counter::Interface& counter, const std::uint32_t increments) noexcept
{
    for (std::uint32_t i{}; i < increments; ++i)
    {
        counter.increment();
    }
}
} // namespace

/**
 * @brief Application entry point.
 *
 * @return 0 on success.
 */
int main()
{
    constexpr std::uint32_t increments1{100U};
    constexpr std::uint32_t increments2{200U};
    driver::counter::Stub counter{};

    std::thread t1{counterThread, std::ref(counter), increments1};
    std::thread t2{counterThread, std::ref(counter), increments2};
    t1.join();
    t2.join();

    std::printf("Counter value: %u!\n", counter.value());
    return 0;
}
