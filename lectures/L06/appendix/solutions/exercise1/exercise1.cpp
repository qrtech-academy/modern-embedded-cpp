/**
 * @brief Multithreading solutions - Exercise Set 1.
 */
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
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

/**
 * @brief Print a message repeatedly at a fixed interval until stopped.
 *
 * @param[in] printSpeed_ms Delay between prints in milliseconds.
 * @param[in] stop Atomic flag; thread exits when set to true.
 */
void workerThread( const std::uint16_t printSpeed_ms, const std::atomic<bool>& stop) noexcept
{
    while (!stop.load())
    {
        std::printf("Worked thread running!\n");
        sleep_ms(printSpeed_ms);
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
    constexpr std::uint16_t timeout_ms{3000U};
    constexpr std::uint16_t printSpeed_ms{100U};
    std::atomic<bool> stop{false};

    std::thread t1{workerThread, printSpeed_ms, std::cref(stop)};
    std::printf("Main thread running!\n");
    sleep_ms(timeout_ms);
    stop.store(true);
    t1.join();
    return 0;
}
