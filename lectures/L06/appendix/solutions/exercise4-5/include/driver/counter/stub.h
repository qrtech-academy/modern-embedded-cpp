/**
 * @brief Stub counter.
 */
#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>

#include "driver/counter/interface.h"

namespace driver::counter
{
/**
 * @brief Stub counter.
 */
class Stub final : public Interface
{
public:
    /**
     * @brief Constructor.
     */
    Stub() noexcept
        : myMutex{}
        , myValue{}
        , myInitialized{true}
    {}

    /**
     * @brief Destructor.
     */
    ~Stub() noexcept override = default;

    /**
     * @brief Check if the counter is initialized.
     * 
     * @return True if initialized, false otherwise.
     */
    bool isInitialized() const noexcept override
    {
        return myInitialized.load();
    }

    /**
     * @brief Get the current counter value.
     * 
     * @return The current counter value.
     */
    std::uint32_t value() const noexcept override
    {
        if (!isInitialized()) { return 0U; }
        std::lock_guard<std::mutex> lock{myMutex};
        return myValue;
    }

    /**
     * @brief Increment the counter by one.
     */
    void increment() noexcept override
    {
        if (!isInitialized()) { return; }
        std::lock_guard<std::mutex> lock{myMutex};
        myValue++;
    }

    /**
     * @brief Reset the counter to zero.
     */
    void reset() noexcept override
    {
        if (!isInitialized()) { return; }
        std::lock_guard<std::mutex> lock{myMutex};
        myValue = 0U;
    }

    /**
     * @brief Set initialization state.
     * 
     * @return True to mark the counter as initialized, false otherwise.
     */
    void setInitialized(const bool initialized) noexcept
    {
        myInitialized.store(initialized);
    }

    Stub(const Stub&)            = delete; // No copy constructor.
    Stub(Stub&&)                 = delete; // No move constructor.
    Stub& operator=(const Stub&) = delete; // No copy constructor.
    Stub& operator=(Stub&&)      = delete; // No move constructor.

private:
    /** Mutex to protect the counter value. */
    mutable std::mutex myMutex;

    /** Internal counter. */
    std::uint32_t myValue;

    /** True if the counter is initialized, false otherwise. */
    std::atomic<bool> myInitialized;
};
} // namespace driver::counter
