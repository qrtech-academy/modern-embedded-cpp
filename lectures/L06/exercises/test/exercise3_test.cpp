/**
 * @file Tests for Exercise Set 3: SharedMem, txThread() and rxThread(), in exercise3.cpp.
 *
 *       Your main() runs for ten seconds, so it is not run; the tests start the threads
 *       themselves. The transmitter's 1000 ms between transmissions is part of the
 *       specification, so the two tests that run it take a second or two each; the others take a
 *       fraction of one.
 *
 *       What a test cannot see is whether the mutex is actually locked around every access: a
 *       missing lock is a data race, which may or may not misbehave on any given run. The tests
 *       check what correct locking guarantees; ThreadSanitizer (-fsanitize=thread) is the tool
 *       that proves the locking itself.
 */
#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>

#include "qacademy/test/test.hpp"
#include "support/output.hpp"
#include "threads.hpp"

// Your program, with its main() renamed. A main() may leave out its return statement and any
// other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise3Main
#include "exercise3.cpp"
#undef main
#pragma GCC diagnostic pop

using support::captureOutput;

namespace
{
/** A value no transmitter starts from, so a test can tell when the first one has arrived. */
constexpr std::uint16_t notYetSent{0xBEEFU};

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
 * @brief Exercise 3.1: SharedMem holds a 16-bit unsigned value and a flag.
 */
TEST(SharedMem, HasTheSpecifiedMembers)
{
    EXPECT_TRUE((std::is_same<decltype(SharedMem::data), std::uint16_t>::value));
    EXPECT_TRUE((std::is_same<decltype(SharedMem::newData), bool>::value));
}

/**
 * @brief Exercise 3.3: the receiver prints new data, and clears newData once it has consumed it.
 */
TEST(Receiver, ConsumesAndPrintsNewData)
{
    SharedMem shared{};
    shared.data    = 42U;
    shared.newData = true;
    std::mutex mutex{};
    std::atomic<bool> stop{false};
    bool rxClearedNewData{};
    bool rxStoppedInTime{};

    const std::string output{captureOutput(
        [&]
        {
            std::thread rx{rxThread, std::ref(shared), std::ref(mutex), std::cref(stop)};
            rxClearedNewData = l06::waitUntil(
                [&]
                {
                    std::lock_guard<std::mutex> lock{mutex};
                    return !shared.newData;
                },
                l06::Milliseconds{1000});
            stop.store(true);
            rxStoppedInTime = l06::joinWithin(rx, l06::Milliseconds{1000});
        })};

    const bool printedTheData{std::string::npos != output.find("42")};
    EXPECT_TRUE(rxClearedNewData);
    EXPECT_TRUE(rxStoppedInTime);
    EXPECT_EQ(lineCount(output), 1U);
    EXPECT_TRUE(printedTheData);
}

/**
 * @brief Exercise 3.3: without new data, the receiver prints nothing.
 */
TEST(Receiver, PrintsNothingWithoutNewData)
{
    SharedMem shared{};
    std::mutex mutex{};
    std::atomic<bool> stop{false};
    bool rxStoppedInTime{};

    const std::string output{captureOutput(
        [&]
        {
            std::thread rx{rxThread, std::ref(shared), std::ref(mutex), std::cref(stop)};
            std::this_thread::sleep_for(l06::Milliseconds{250});
            stop.store(true);
            rxStoppedInTime = l06::joinWithin(rx, l06::Milliseconds{1000});
        })};

    EXPECT_TRUE(rxStoppedInTime);
    EXPECT_OUTPUT(output, "");
}

/**
 * @brief Exercise 3.2: the transmitter publishes a value and sets newData, then, a transmission
 *        interval later, publishes the next value up.
 */
TEST(Transmitter, PublishesIncrementingValues)
{
    SharedMem shared{};
    shared.data = notYetSent;
    std::mutex mutex{};
    std::atomic<bool> stop{false};
    bool txSentAValue{}, txSentTheNextValue{}, txStoppedInTime{};
    std::uint16_t first{}, second{};

    std::thread tx{txThread, std::ref(shared), std::ref(mutex), std::cref(stop)};
    txSentAValue = l06::waitUntil(
        [&]
        {
            std::lock_guard<std::mutex> lock{mutex};
            if (!shared.newData) { return false; }
            first          = shared.data;
            shared.newData = false;
            return true;
        },
        l06::Milliseconds{1000});
    txSentTheNextValue = l06::waitUntil(
        [&]
        {
            std::lock_guard<std::mutex> lock{mutex};
            second = shared.data;
            return shared.newData;
        },
        l06::Milliseconds{3000});
    stop.store(true);
    txStoppedInTime = l06::joinWithin(tx, l06::Milliseconds{3000});

    EXPECT_TRUE(txSentAValue);
    EXPECT_TRUE(txSentTheNextValue);
    EXPECT_TRUE(txStoppedInTime);
    EXPECT_EQ(static_cast<unsigned>(second), static_cast<unsigned>(first) + 1U);
}

/**
 * @brief Exercise 3.4: with both threads running, what the transmitter sends the receiver prints,
 *        exactly once.
 */
TEST(Threads, TheReceiverPrintsWhatTheTransmitterSends)
{
    SharedMem shared{};
    shared.data = notYetSent;
    std::mutex mutex{};
    std::atomic<bool> stop{false};
    bool rxConsumedWhatTxSent{}, txStoppedInTime{}, rxStoppedInTime{};
    std::uint16_t sent{};

    const std::string output{captureOutput(
        [&]
        {
            std::thread tx{txThread, std::ref(shared), std::ref(mutex), std::cref(stop)};
            std::thread rx{rxThread, std::ref(shared), std::ref(mutex), std::cref(stop)};
            rxConsumedWhatTxSent = l06::waitUntil(
                [&]
                {
                    std::lock_guard<std::mutex> lock{mutex};
                    sent = shared.data;
                    return (notYetSent != shared.data) && !shared.newData;
                },
                l06::Milliseconds{900});
            stop.store(true);
            txStoppedInTime = l06::joinWithin(tx, l06::Milliseconds{3000});
            rxStoppedInTime = l06::joinWithin(rx, l06::Milliseconds{1000});
        })};

    const bool printedWhatTxSent{std::string::npos != output.find(std::to_string(sent))};
    EXPECT_TRUE(rxConsumedWhatTxSent);
    EXPECT_TRUE(txStoppedInTime);
    EXPECT_TRUE(rxStoppedInTime);
    EXPECT_EQ(lineCount(output), 1U);
    EXPECT_TRUE(printedWhatTxSent);
}
