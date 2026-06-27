/**
 * @brief Multithreading solutions - Exercise Set 2.
 */
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <thread>

namespace
{
/**
 * @brief Increment an atomic counter a given number of times.
 *
 * @param[in,out] counter    Atomic counter to increment.
 * @param[in]     iterations Number of increments to perform.
 */
void incrementCounter(std::atomic<std::uint32_t>& counter, const std::uint32_t iterations) noexcept
{
    for (std::uint32_t i{}; i < iterations; ++i)
    {
        counter++;
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
    constexpr std::uint32_t iterations{100000U};
    std::atomic<std::uint32_t> counter{};

    std::thread t1{incrementCounter, std::ref(counter), iterations};
    std::thread t2{incrementCounter, std::ref(counter), iterations};
    t1.join();
    t2.join();

    std::printf("Final counter value: %u!\n", counter.load());
    return 0;
}
