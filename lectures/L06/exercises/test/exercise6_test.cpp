/**
 * @file Tests for Exercise Set 6: hasNewData(), and the condition variable versions of txThread()
 *       and rxThread(), in exercise6.cpp.
 *
 *       Your main() runs for ten seconds, so it is not run. The tests start the threads
 *       themselves, and stop the receiver the way Exercise 6.1 says main() must: the stop flag set
 *       while holding the mutex, then notify_all().
 *
 *       A receiver whose predicate forgets the stop flag never wakes up to stop. The tests do not
 *       hang on it: after the deadline they give it new data and notify it until it finishes,
 *       and then report that it did not stop when asked.
 */
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include "qacademy/test/test.hpp"
#include "support/output.hpp"
#include "threads.hpp"

// Your program, with its main() renamed. A main() may leave out its return statement and any
// other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise6Main
#include "exercise6.cpp"
#undef main
#pragma GCC diagnostic pop

using support::captureOutput;

namespace
{
/** A value no transmitter starts from, so a test can tell when the first one has arrived. */
constexpr std::uint16_t notYetSent{0xBEEFU};

/**
 * @brief Everything the two threads share, and the way Exercise 6.1 stops them.
 */
struct Channel
{
    SharedMem shared{};
    std::mutex mutex{};
    std::condition_variable cv{};
    std::atomic<bool> stop{false};

    /**
     * @brief Stop the threads as main() must: set the flag under the mutex, then notify.
     */
    void requestStop()
    {
        {
            std::lock_guard<std::mutex> lock{mutex};
            stop.store(true);
        }
        cv.notify_all();
    }

    /**
     * @brief Wake a receiver that ignores the stop flag, by giving it data to consume.
     */
    void nudge()
    {
        {
            std::lock_guard<std::mutex> lock{mutex};
            shared.newData = true;
        }
        cv.notify_all();
    }

    /**
     * @brief Hand the receiver new data, as the transmitter would.
     *
     * @param[in] value The data.
     */
    void send(const std::uint16_t value)
    {
        {
            std::lock_guard<std::mutex> lock{mutex};
            shared.data    = value;
            shared.newData = true;
        }
        cv.notify_one();
    }

    /**
     * @brief Check whether the receiver has consumed the last data sent.
     *
     * @return True if newData is false.
     */
    bool consumed()
    {
        std::lock_guard<std::mutex> lock{mutex};
        return !shared.newData;
    }
};

/**
 * @brief Count the lines of printed output.
 *
 * @param[in] output The printed output.
 *
 * @return The number of lines.
 */
unsigned lineCount(const std::string& output)
{
    std::istringstream stream{output};
    unsigned count{};
    for (std::string line{}; std::getline(stream, line);)
    {
        ++count;
    }
    return count;
}
} // namespace

/**
 * @brief Exercise 6.1: hasNewData() is true when there is new data or the stop flag is set, and
 *        is noexcept.
 */
TEST(HasNewData, IsTrueForNewDataOrStop)
{
    SharedMem shared{};
    std::atomic<bool> stop{false};
    EXPECT_FALSE(hasNewData(shared, stop));
    shared.newData = true;
    EXPECT_TRUE(hasNewData(shared, stop));
    stop.store(true);
    EXPECT_TRUE(hasNewData(shared, stop));
    shared.newData = false;
    EXPECT_TRUE(hasNewData(shared, stop));
    EXPECT_TRUE(noexcept(hasNewData(shared, stop)));
}

/**
 * @brief Exercise 6.1: a waiting receiver wakes for each value sent, prints it once, and clears
 *        newData.
 */
TEST(Receiver, WakesForEachValueSent)
{
    Channel channel{};
    bool rxConsumedTheFirst{}, rxConsumedTheSecond{}, rxStoppedWhenAsked{};

    const std::string output{captureOutput(
        [&]
        {
            std::thread rx{rxThread, std::ref(channel.shared), std::ref(channel.mutex),
                           std::ref(channel.cv), std::cref(channel.stop)};
            std::this_thread::sleep_for(l06::Milliseconds{20});
            channel.send(41U);
            rxConsumedTheFirst =
                l06::waitUntil([&] { return channel.consumed(); }, l06::Milliseconds{1000});
            channel.send(42U);
            rxConsumedTheSecond =
                l06::waitUntil([&] { return channel.consumed(); }, l06::Milliseconds{1000});
            channel.requestStop();
            rxStoppedWhenAsked =
                l06::joinWithin(rx, l06::Milliseconds{1000}, [&] { channel.nudge(); });
        })};

    const bool printedBothValues{(std::string::npos != output.find("41")) &&
                                 (std::string::npos != output.find("42"))};
    EXPECT_TRUE(rxConsumedTheFirst);
    EXPECT_TRUE(rxConsumedTheSecond);
    EXPECT_TRUE(rxStoppedWhenAsked);
    EXPECT_EQ(lineCount(output), 2U);
    EXPECT_TRUE(printedBothValues);
}

/**
 * @brief Exercise 6.1: a receiver waiting with no data wakes and returns when stopped, every time.
 *        Run many times over, because a lost wakeup is a matter of timing.
 */
TEST(Receiver, StopsPromptlyWhileWaiting)
{
    constexpr int attempts{50};
    int timesStoppedWhenAsked{};
    std::string output{};

    for (int i{}; i < attempts; ++i)
    {
        Channel channel{};
        output += captureOutput(
            [&]
            {
                std::thread rx{rxThread, std::ref(channel.shared), std::ref(channel.mutex),
                               std::ref(channel.cv), std::cref(channel.stop)};
                // Stop at a different moment each time, from at once to a few milliseconds in.
                std::this_thread::sleep_for(std::chrono::microseconds{(i % 10) * 500});
                channel.requestStop();
                if (l06::joinWithin(rx, l06::Milliseconds{1000}, [&] { channel.nudge(); }))
                {
                    ++timesStoppedWhenAsked;
                }
            });
    }
    EXPECT_EQ(timesStoppedWhenAsked, attempts);
}

/**
 * @brief Exercise 6.1: the transmitter notifies the receiver, which prints what was sent at once
 *        rather than at its next poll, exactly once.
 */
TEST(Threads, TheReceiverPrintsWhatTheTransmitterSends)
{
    Channel channel{};
    channel.shared.data = notYetSent;
    bool rxConsumedWhatTxSent{}, txStoppedInTime{}, rxStoppedWhenAsked{};
    std::uint16_t sent{};

    const std::string output{captureOutput(
        [&]
        {
            std::thread rx{rxThread, std::ref(channel.shared), std::ref(channel.mutex),
                           std::ref(channel.cv), std::cref(channel.stop)};
            std::thread tx{txThread, std::ref(channel.shared), std::ref(channel.mutex),
                           std::ref(channel.cv), std::cref(channel.stop)};
            rxConsumedWhatTxSent = l06::waitUntil(
                [&]
                {
                    std::lock_guard<std::mutex> lock{channel.mutex};
                    sent = channel.shared.data;
                    return (notYetSent != channel.shared.data) && !channel.shared.newData;
                },
                l06::Milliseconds{900});
            channel.requestStop();
            txStoppedInTime = l06::joinWithin(tx, l06::Milliseconds{3000});
            rxStoppedWhenAsked =
                l06::joinWithin(rx, l06::Milliseconds{1000}, [&] { channel.nudge(); });
        })};

    const bool printedWhatTxSent{std::string::npos != output.find(std::to_string(sent))};
    EXPECT_TRUE(rxConsumedWhatTxSent);
    EXPECT_TRUE(txStoppedInTime);
    EXPECT_TRUE(rxStoppedWhenAsked);
    EXPECT_EQ(lineCount(output), 1U);
    EXPECT_TRUE(printedWhatTxSent);
}
