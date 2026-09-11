/**
 * @file Multithreading solutions - Exercise Set 4-5.
 */
#include <cstdint>
#include <cstdio>
#include <functional>
#include <thread>

#include "driver/counter/stub.hpp"

namespace
{
/**
 * @brief Increment a counter a given number of times.
 *
 * @param[in,out] counter Counter to increment.
 * @param[in] iterations Number of increments to perform.
 */
void counterThread(driver::counter::Interface& counter, const std::uint32_t iterations) noexcept
{
    for (std::uint32_t i{}; i < iterations; ++i)
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
    constexpr std::uint32_t iterations1{100U};
    constexpr std::uint32_t iterations2{200U};
    driver::counter::Stub counter{};

    std::thread t1{counterThread, std::ref(counter), iterations1};
    std::thread t2{counterThread, std::ref(counter), iterations2};
    t1.join();
    t2.join();

    std::printf("Counter value: %u!\n", counter.value());
    return 0;
}
