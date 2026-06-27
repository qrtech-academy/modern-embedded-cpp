/**
 * @brief Multithreading solutions - Exercise Set 6.
 */
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <thread>

namespace
{
/**
 * @brief Shared memory structure.
 */
struct SharedMem
{
    /** Shared data. */
    std::uint16_t data{};

    /** True if new data is available, false otherwise. */
    bool newData{false};
};

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
 * @brief Check if new data is available or the stop flag has been set.
 *
 * @param[in] shared Shared memory to check.
 * @param[in] stop Atomic stop flag.
 * 
 * @return True if new data is available or stop is set, false otherwise.
 */
bool hasNewData(SharedMem& shared, const std::atomic<bool>& stop) noexcept
{
    return stop.load() || shared.newData;
}

/**
 * @brief Transmit incrementing values to shared memory at a fixed interval.
 *
 * @param[in,out] shared Shared memory to write to.
 * @param[in,out] mutex Mutex protecting shared memory access.
 * @param[in,out] cv Condition variable used to notify the RX thread.
 * @param[in] stop Atomic flag; thread exits when set to true.
 */
void txThread(SharedMem& shared, std::mutex& mutex, std::condition_variable& cv,
              const std::atomic<bool>& stop) noexcept
{
    constexpr std::uint16_t txSpeed_ms{1000U};
    std::uint16_t value{};

    while (!stop.load())
    {
        {
            std::lock_guard<std::mutex> lock{mutex};
            shared.data    = value++;
            shared.newData = true;
        }
        cv.notify_one();
        sleep_ms(txSpeed_ms);
    }
}

/**
 * @brief Receive and print new data from shared memory at a fixed interval.
 *
 * @param[in,out] shared Shared memory to read from.
 * @param[in,out] mutex  Mutex protecting shared memory access.
 * @param[in,out] cv Condition variable used to wait for new data.
 * @param[in] stop Atomic flag; thread exits when set to true.
 */
void rxThread(SharedMem& shared, std::mutex& mutex, std::condition_variable& cv,
              const std::atomic<bool>& stop) noexcept
{
    constexpr std::uint16_t rxSpeed_ms{100U};

    while (!stop.load())
    {
        {
            auto predicate = [&shared, &stop] { return hasNewData(shared, stop); };
            std::unique_lock<std::mutex> lock{mutex};
            cv.wait(lock, predicate);
            if (stop.load()) { break; }

            if (shared.newData)
            {
                std::printf("RX thread received new data: %u!\n", shared.data);
                shared.newData = false;
            }
        }
        sleep_ms(rxSpeed_ms);
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
    constexpr std::uint16_t timeout_ms{10000U};
    SharedMem shared{};
    std::mutex mutex{};
    std::condition_variable cv{};
    std::atomic<bool> stop{false};

    std::thread t1{txThread, std::ref(shared), std::ref(mutex), std::ref(cv), std::cref(stop)};
    std::thread t2{rxThread, std::ref(shared), std::ref(mutex), std::ref(cv), std::cref(stop)};

    sleep_ms(timeout_ms);
    stop.store(true);
    cv.notify_all();
    t1.join();
    t2.join();
    return 0;
}
