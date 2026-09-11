/**
 * @file Tests for Exercise Set 7: validateFirmware(), run through std::async, in exercise7.cpp.
 *
 *       validateFirmware() sleeps for 2000 ms before it looks at the buffer, because the exercise
 *       says a flash read is slow, so the tests that call it run their three images at once and
 *       take about two seconds together, and the program takes about two more.
 */
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <future>
#include <sstream>
#include <string>
#include <vector>

#include "qacademy/test/test.hpp"
#include "support/output.hpp"
#include "threads.hpp"

// Your program, with its main() renamed. A main() may leave out its return statement and any
// other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise7Main
#include "exercise7.cpp"
#undef main
#pragma GCC diagnostic pop

using support::captureOutput;

/**
 * @brief Exercise 7.1: no buffer, or an empty one, is not valid firmware. validateFirmware() is
 *        noexcept.
 */
TEST(Validate, RejectsAMissingOrEmptyBuffer)
{
    const std::uint8_t image[]{0x01U, 0x02U};
    EXPECT_FALSE(validateFirmware(nullptr, 2U));
    EXPECT_FALSE(validateFirmware(image, 0U));
    EXPECT_TRUE(noexcept(validateFirmware(image, 2U)));
}

/**
 * @brief Exercise 7.1: an image is valid unless a byte is 0xFF, wherever that byte is.
 */
TEST(Validate, RejectsUnprogrammedFlashAnywhere)
{
    const std::uint8_t valid[]{0x32U, 0xAFU, 0x07U, 0x55U, 0x00U, 0xFEU};
    const std::uint8_t erasedFirst[]{0xFFU, 0xAFU, 0x07U, 0x55U, 0x00U, 0xFEU};
    const std::uint8_t erasedLast[]{0x32U, 0xAFU, 0x07U, 0x55U, 0x00U, 0xFFU};
    constexpr std::uint16_t length{6U};

    // All three at once, so that the three simulated flash reads overlap.
    auto validFuture{std::async(std::launch::async, validateFirmware, valid, length)};
    auto firstFuture{std::async(std::launch::async, validateFirmware, erasedFirst, length)};
    auto lastFuture{std::async(std::launch::async, validateFirmware, erasedLast, length)};
    const bool validResult{validFuture.get()};
    const bool firstResult{firstFuture.get()};
    const bool lastResult{lastFuture.get()};

    EXPECT_TRUE(validResult);
    EXPECT_FALSE(firstResult);
    EXPECT_FALSE(lastResult);
}

/**
 * @brief Exercise 7.2: the program announces the boot, reports that it is still waiting at most
 *        once per 200 ms poll, and then prints the result.
 */
TEST(Program, PollsWhileTheValidationRuns)
{
    const auto start{std::chrono::steady_clock::now()};
    const std::string output{captureOutput([] { exercise7Main(); })};
    const long long elapsed_ms{l06::elapsedSince(start)};

    std::istringstream stream{output};
    std::vector<std::string> lines{};
    for (std::string line{}; std::getline(stream, line);)
    {
        lines.push_back(line);
    }

    std::size_t waiting{};
    bool onlyWaitingInBetween{true};
    for (std::size_t i{1U}; i + 1U < lines.size(); ++i)
    {
        if ("Still waiting for firmware validation..." == lines[i]) { ++waiting; }
        else { onlyWaitingInBetween = false; }
    }

    const bool printedBootWaitingAndResult{lines.size() >= 3U};
    EXPECT_TRUE(printedBootWaitingAndResult);
    const bool reportedWaitingAtLeastOnce{waiting >= 1U};
    const bool polledAtMostOncePer200ms{waiting <= static_cast<std::size_t>(elapsed_ms / 200 + 2)};
    const bool endedWithTheResult{"Still waiting for firmware validation..." != lines.back()};
    EXPECT_EQ(lines.front(), std::string{"Booting system while validating firmware..."});
    EXPECT_TRUE(onlyWaitingInBetween);
    EXPECT_TRUE(reportedWaitingAtLeastOnce);
    EXPECT_TRUE(polledAtMostOncePer200ms);
    EXPECT_TRUE(endedWithTheResult);
}
