/**
 * @file Timer driver.
 */
#pragma once

#include <cstdint>

namespace driver
{
/**
 * @brief Timer driver.
 *
 *        This class is non-copyable and non-movable.
 */
class Timer final
{
public:
    /**
     * @brief Constructor.
     *
     * @param[in] timeout_ms Timeout in milliseconds. Must be greater than 0.
     */
    explicit Timer(std::uint32_t timeout_ms) noexcept;

    /**
     * @brief Destructor.
     */
    ~Timer() noexcept;

    /**
     * @brief Get the set timeout of the timer.
     *
     * @return Timeout in milliseconds.
     */
    [[nodiscard]] std::uint32_t timeout_ms() const noexcept;

    /**
     * @brief Check if the timer is running.
     *
     * @return True if the timer is running, false otherwise.
     */
    [[nodiscard]] bool isRunning() const noexcept;

    /**
     * @brief Check if the timer is initialized.
     *
     * @return True if the timer is initialized, false otherwise.
     */
    [[nodiscard]] bool isInitialized() const noexcept;

    /**
     * @brief Start the timer.
     */
    void start() noexcept;

    /**
     * @brief Stop the timer.
     */
    void stop() noexcept;

    /**
     * @brief Toggle the timer.
     */
    void toggle() noexcept;

    /**
     * @brief Tick the timer.
     */
    void tick() noexcept;

    /**
     * @brief Check if the timer has timed out.
     *
     * @return True if the timer has timed out, false otherwise.
     */
    [[nodiscard]] bool hasTimedOut() noexcept;

    Timer()                        = delete; // No default constructor.
    Timer(const Timer&)            = delete; // No copy constructor.
    Timer(Timer&&)                 = delete; // No move constructor.
    Timer& operator=(const Timer&) = delete; // No copy assignment.
    Timer& operator=(Timer&&)      = delete; // No move assignment.

private:
    /** Timeout in milliseconds. */
    const std::uint32_t myTimeout_ms;

    /** Internal millisecond counter. */
    std::uint32_t myCounter_ms;

    /** True if the timer is running, false otherwise. */
    bool myRunning;

    /** True if the timer is initialized, false otherwise. */
    bool myInitialized;
};
} // namespace driver
