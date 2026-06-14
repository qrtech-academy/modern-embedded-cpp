/**
 * @brief Timer driver.
 */
#pragma once

#include <cstdint>
#include <cstdio>

namespace driver::timer
{
/**
 * @brief Enumeration of supported timers.
 */
enum class Type : std::uint8_t
{
    Stm32, ///< STM32 timer.
    Stub,  ///< Stub timer.
};

/**
 * @brief Timer implementation.
 *
 * @tparam T Timer type (default = stub timer).
 */
template<Type T = Type::Stub>
class Timer final
{
public:
    /**
     * @brief Constructor.
     * 
     * @param[in] timeout_ms Timeout in milliseconds. Must be greater than 0.
     */
    explicit Timer(const std::uint16_t timeout_ms) noexcept
        : myTimeout_ms{timeout_ms}
        , myCounter_ms{}
        , myRunning{false}
        , myInitialized{0U < timeout_ms}
    {
        if (myInitialized) 
        { 
            std::printf("Created stub timer with timeout %u ms!\n", myTimeout_ms); 
        }
        else
        { 
            std::printf("Failed to initialize stub timer: invalid timeout 0 ms!\n");
        }
    }

    /**
     * @brief Destructor.
     */
    ~Timer() noexcept
    {
        stop();
        std::printf("Destroying stub timer!\n");
    }

    /**
     * @brief Get timeout.
     * 
     * @return Timeout in milliseconds.
     */
    std::uint16_t timeout_ms() const noexcept { return myTimeout_ms; }

    /**
     * @brief Check if the timer is running.
     * 
     * @return True if running, false otherwise.
     */
    bool isRunning() const noexcept { return myRunning; }

    /**
     * @brief Check whether the timer has been initialized.
     * 
     * @return True if initialized, false otherwise.
     */
    bool isInitialized() const noexcept { return myInitialized; }

    /**
     * @brief Start timer.
     */
    void start() noexcept
    {
        if (myInitialized && !myRunning)
        {
            std::printf("Starting stub timer!\n");
            myRunning = true;
        }
    }

    /**
     * @brief Stop timer.
     */
    void stop() noexcept
    {
        if (myInitialized && myRunning)
        {
            std::printf("Stopping stub timer!\n");
            myRunning = false;
        }
    }

    /**
     * @brief Toggle timer.
     */
    void toggle() noexcept
    {
        if (myInitialized)
        {
            std::printf("Toggling stub timer!\n");
            myRunning = !myRunning;
        }
    }

    /**
     * @brief Tick the timer to simulate that one millisecond has passed.
     */
    void tick() noexcept
    {
        if (myInitialized && myRunning)
        {
            myCounter_ms++;
        }
    }

    /**
     * @brief Check if the timer has timed out.
     * 
     * @return True if timed out, false otherwise.
     */
    bool hasTimedOut() noexcept
    {
        const bool timeout{myTimeout_ms <= myCounter_ms};

        // Clear the internal counter on timeout.
        if (timeout) { myCounter_ms = 0U; }
        return timeout;
    }

    Timer()                        = delete; // No default constructor.
    Timer(const Timer&)            = delete; // No copy constructor.
    Timer(Timer&&)                 = delete; // No move constructor.
    Timer& operator=(const Timer&) = delete; // No copy assignment.
    Timer& operator=(Timer&&)      = delete; // No move assignment.

private:
    /** Timeout in milliseconds. */
    const std::uint32_t myTimeout_ms;

    /** Internal tick counter. */
    std::uint32_t myCounter_ms;

    /** True if the timer is running, false if stopped. */
    bool myRunning;

    /** True if the timer is initialized, false otherwise. */
    bool myInitialized;
};

/**
 * @brief STM32 timer.
 */
template<>
class Timer<Type::Stm32> final
{
public:
    /**
     * @brief Constructor.
     * 
     * @param[in] timeout_ms Timeout in milliseconds. Must be greater than 0.
     */
    explicit Timer(const std::uint16_t timeout_ms) noexcept
        : myTimeout_ms{timeout_ms}
        , myCounter_ms{}
        , myRunning{false}
        , myInitialized{0U < timeout_ms}
    {
        if (myInitialized) 
        { 
            std::printf("Created STM32 timer with timeout %u ms!\n", myTimeout_ms); 
        }
        else
        { 
            std::printf("Failed to initialize STM32 timer: invalid timeout 0 ms!\n");
        }
    }

    /**
     * @brief Destructor.
     */
    ~Timer() noexcept
    {
        stop();
        std::printf("Destroying STM32 timer!\n");
    }

    /**
     * @brief Get timeout.
     * 
     * @return Timeout in milliseconds.
     */
    std::uint16_t timeout_ms() const noexcept { return myTimeout_ms; }

    /**
     * @brief Check if the timer is running.
     * 
     * @return True if running, false otherwise.
     */
    bool isRunning() const noexcept { return myRunning; }

    /**
     * @brief Check whether the timer has been initialized.
     * 
     * @return True if initialized, false otherwise.
     */
    bool isInitialized() const noexcept { return myInitialized; }

    /**
     * @brief Start timer.
     */
    void start() noexcept
    {
        if (myInitialized && !myRunning)
        {
            std::printf("Starting STM32 timer!\n");
            myRunning = true;
        }
    }

    /**
     * @brief Stop timer.
     */
    void stop() noexcept
    {
        if (myInitialized && myRunning)
        {
            std::printf("Stopping STM32 timer!\n");
            myRunning = false;
        }
    }

    /**
     * @brief Toggle timer.
     */
    void toggle() noexcept
    {
        if (myInitialized)
        {
            std::printf("Toggling STM32 timer!\n");
            myRunning = !myRunning;
        }
    }

    /**
     * @brief Tick the timer to simulate that one millisecond has passed.
     */
    void tick() noexcept
    {
        if (myInitialized && myRunning)
        {
            myCounter_ms++;
        }
    }

    /**
     * @brief Check if the timer has timed out.
     * 
     * @return True if timed out, false otherwise.
     */
    bool hasTimedOut() noexcept
    {
        const bool timeout{myTimeout_ms <= myCounter_ms};

        // Clear the internal counter on timeout.
        if (timeout)
        {
            myCounter_ms = 0U;
        }
        return timeout;
    }

    Timer()                        = delete; // No default constructor.
    Timer(const Timer&)            = delete; // No copy constructor.
    Timer(Timer&&)                 = delete; // No move constructor.
    Timer& operator=(const Timer&) = delete; // No copy assignment.
    Timer& operator=(Timer&&)      = delete; // No move assignment.

private:
    /** Timeout in milliseconds. */
    const std::uint32_t myTimeout_ms;

    /** Internal tick counter. */
    std::uint32_t myCounter_ms;

    /** True if the timer is running, false if stopped. */
    bool myRunning;

    /** True if the timer is initialized, false otherwise. */
    bool myInitialized;
};
} // namespace driver::timer
