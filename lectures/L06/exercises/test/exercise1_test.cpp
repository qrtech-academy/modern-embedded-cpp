/**
 * @file Tests for Exercise Set 1: workerThread() with an atomic stop flag, in exercise1.cpp.
 *
 *       The file evolves through the set, and the tests check where it ends up: Exercise 1.3's
 *       workerThread(printSpeed_ms, stop). Your main() sleeps for three seconds, so it is not run;
 *       the tests start workerThread() themselves, with short intervals.
 */
#include <atomic>
#include <chrono>
#include <cstddef>
#include <functional>
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
#define main exercise1Main
#include "exercise1.cpp"
#undef main
#pragma GCC diagnostic pop

using support::captureOutput;

namespace
{
/**
 * @brief Count the lines of printed output, and check that every one is the worker's message.
 *
 * @param[in] output The printed output.
 * @param[out] everyLineIsTheMessage True if every line is "Worker thread running!".
 *
 * @return The number of lines.
 */
std::size_t countWorkerLines(const std::string& output, bool& everyLineIsTheMessage)
{
    std::istringstream stream{output};
    std::size_t count{};
    everyLineIsTheMessage = true;
    for (std::string line{}; std::getline(stream, line); ++count)
    {
        if ("Worker thread running!" != line) { everyLineIsTheMessage = false; }
    }
    return count;
}
} // namespace

/**
 * @brief Exercise 1.3: the worker prints its message and stops soon after the flag is set.
 */
TEST(Worker, StopsWhenTheFlagIsSet)
{
    std::atomic<bool> stop{false};
    bool stoppedWithinASecondOfTheFlag{};
    const std::string output{captureOutput(
        [&]
        {
            std::thread worker{workerThread, std::uint16_t{5U}, std::cref(stop)};
            std::this_thread::sleep_for(l06::Milliseconds{30});
            stop.store(true);
            stoppedWithinASecondOfTheFlag = l06::joinWithin(worker, l06::Milliseconds{1000});
        })};

    bool everyLineIsTheMessage{};
    const bool printedAtLeastOnce{countWorkerLines(output, everyLineIsTheMessage) >= 1U};
    EXPECT_TRUE(stoppedWithinASecondOfTheFlag);
    EXPECT_TRUE(printedAtLeastOnce);
    EXPECT_TRUE(everyLineIsTheMessage);
}

/**
 * @brief Exercise 1.3: the worker prints once every printSpeed_ms, so it cannot print more often
 *        than that. A worker that forgets to sleep prints thousands of lines in the same time.
 */
TEST(Worker, PrintsNoMoreOftenThanItsInterval)
{
    constexpr std::uint16_t printSpeed_ms{20U};
    std::atomic<bool> stop{false};
    long long elapsed_ms{};
    const std::string output{captureOutput(
        [&]
        {
            const auto start{std::chrono::steady_clock::now()};
            std::thread worker{workerThread, printSpeed_ms, std::cref(stop)};
            std::this_thread::sleep_for(l06::Milliseconds{200});
            stop.store(true);
            worker.join();
            elapsed_ms = l06::elapsedSince(start);
        })};

    bool everyLineIsTheMessage{};
    const auto lines{countWorkerLines(output, everyLineIsTheMessage)};
    const bool printedAtLeastOnce{lines >= 1U};
    const bool printedAtMostOncePerInterval{
        lines <= static_cast<std::size_t>(elapsed_ms / printSpeed_ms + 2)};
    EXPECT_TRUE(printedAtLeastOnce);
    EXPECT_TRUE(printedAtMostOncePerInterval);
    EXPECT_TRUE(everyLineIsTheMessage);
}

/**
 * @brief Exercise 1.3: the worker runs as long as the flag is false, so with the flag already
 *        set it prints nothing and returns at once.
 */
TEST(Worker, DoesNothingOnceStopped)
{
    const std::atomic<bool> stop{true};
    EXPECT_OUTPUT(captureOutput([&] { workerThread(std::uint16_t{1000U}, stop); }), "");
}
