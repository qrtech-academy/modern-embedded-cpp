/**
 * @file Watch app::logic::Logic::run() for half a second, from outside.
 *
 *       run() never returns, so a test cannot call it and wait. Instead runProbe() forks: the child
 *       process builds a Logic around a ProbeSerial and calls run(), and the ProbeSerial reports
 *       every write() and read() through a pipe. The parent collects those reports, and whatever
 *       the child printed, for a fixed time, then kills the child. What comes back is enough to
 *       check what Exercises 3.1 d) and 4.3 d) ask of run(): an incrementing byte, a read after
 *       every write, a pause of about 100 ms each time round, and a print only when a byte was
 *       received.
 *
 *       Include this after your driver/serial/interface.hpp, whose Interface ProbeSerial
 *       implements.
 */
#pragma once

#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <fcntl.h>
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>

#include "driver/serial/interface.hpp"

namespace probe
{
/**
 * @brief A serial driver that reports what is done to it through a file descriptor.
 *
 *        Each write() sends 'W' followed by the byte, and each read() sends 'R'. read() answers
 *        with a received byte, 0x5A, or with nothing, depending on how the driver was created.
 */
class ProbeSerial final : public driver::serial::Interface
{
public:
    /**
     * @brief Constructor.
     *
     * @param[in] fd Where to report writes and reads.
     * @param[in] receives True if read() shall report a received byte, false otherwise.
     */
    explicit ProbeSerial(const int fd, const bool receives) noexcept
        : myFd{fd}
        , myReceives{receives}
    {}

    [[nodiscard]] bool isInitialized() const noexcept override { return true; }

    void write(const std::uint8_t byte) noexcept override
    {
        const char report[2]{'W', static_cast<char>(byte)};
        [[maybe_unused]] const auto written{::write(myFd, report, sizeof(report))};
    }

    bool read(std::uint8_t& byte) noexcept override
    {
        [[maybe_unused]] const auto written{::write(myFd, "R", 1U)};
        if (myReceives) { byte = 0x5AU; }
        return myReceives;
    }

private:
    const int myFd;
    const bool myReceives;
};

/**
 * @brief What run() did while it was watched.
 */
struct Report
{
    /** The bytes written, in order. */
    std::vector<std::uint8_t> written{};

    /** Every call in order: 'W' for a write, 'R' for a read. */
    std::string calls{};

    /** Everything run() printed. */
    std::string printed{};

    /** True if the child stopped before it was killed: run() returned, or crashed. */
    bool stoppedEarly{};

    /**
     * @brief Count the lines printed.
     *
     * @return The number of newline characters in what was printed.
     */
    [[nodiscard]] std::size_t printedLines() const noexcept
    {
        std::size_t lines{};
        for (const char c : printed)
        {
            if ('\n' == c) { ++lines; }
        }
        return lines;
    }
};

/**
 * @brief Run body() in a child process for the given time, and report what it did.
 *
 * @param[in] body Code that builds a Logic around a ProbeSerial writing to the given file
 *                 descriptor, and calls run().
 * @param[in] watch How long to watch.
 *
 * @return What the child's serial driver saw, and what it printed.
 */
inline Report runProbe(const std::function<void(int)>& body, const std::chrono::milliseconds watch)
{
    int calls[2]{};
    int output[2]{};
    Report report{};
    if ((0 != pipe(calls)) || (0 != pipe(output))) { return report; }

    // The child inherits the parent's buffers, and would print them a second time.
    std::cout.flush();
    std::fflush(stdout);

    const pid_t child{fork()};
    if (0 == child)
    {
        close(calls[0]);
        close(output[0]);
        dup2(output[1], STDOUT_FILENO);
        // Unbuffered, so nothing run() prints is lost when the child is killed.
        std::setvbuf(stdout, nullptr, _IONBF, 0U);
        body(calls[1]);
        _exit(0);
    }
    close(calls[1]);
    close(output[1]);

    // Collect reports until the time is up.
    std::string raw{};
    const auto collect = [&](const int fd, std::string& into)
    {
        char buffer[4096];
        const auto count{::read(fd, buffer, sizeof(buffer))};
        if (0 < count) { into.append(buffer, static_cast<std::size_t>(count)); }
        return count;
    };
    const auto deadline{std::chrono::steady_clock::now() + watch};
    while (std::chrono::steady_clock::now() < deadline)
    {
        pollfd fds[2]{{calls[0], POLLIN, 0}, {output[0], POLLIN, 0}};
        const auto left{std::chrono::duration_cast<std::chrono::milliseconds>(
            deadline - std::chrono::steady_clock::now())};
        if (0 < poll(fds, 2U, static_cast<int>(left.count()) + 1))
        {
            if (0 != (fds[0].revents & POLLIN)) { collect(calls[0], raw); }
            if (0 != (fds[1].revents & POLLIN)) { collect(output[0], report.printed); }
        }
    }

    // Stop the child, then drain what it wrote before it stopped.
    int status{};
    report.stoppedEarly = (child == waitpid(child, &status, WNOHANG));
    if (!report.stoppedEarly)
    {
        kill(child, SIGKILL);
        waitpid(child, &status, 0);
    }
    while (0 < collect(calls[0], raw)) {}
    while (0 < collect(output[0], report.printed)) {}
    close(calls[0]);
    close(output[0]);

    // Decode the reports: 'W' and its byte, or 'R'.
    for (std::size_t i{}; i < raw.size(); ++i)
    {
        if (('W' == raw[i]) && (i + 1U < raw.size()))
        {
            report.calls += 'W';
            report.written.push_back(static_cast<std::uint8_t>(raw[++i]));
        }
        else if ('R' == raw[i]) { report.calls += 'R'; }
    }
    return report;
}

/**
 * @brief Run code in a child process and return the text it produces, so that code which may
 *        crash, such as a destructor that deletes a driver twice, fails one test with an
 *        explanation instead of ending every test after it.
 *
 * @param[in] body The code to run. Its return value is sent back to the parent.
 * @param[in] crashed Set to the signal that stopped the child, or 0 if it finished.
 *
 * @return What body() returned, or an empty string if the child crashed.
 */
inline std::string inChild(const std::function<std::string()>& body, int& crashed)
{
    int result[2]{};
    crashed = 0;
    if (0 != pipe(result)) { return {}; }
    std::cout.flush();
    std::fflush(stdout);

    const pid_t child{fork()};
    if (0 == child)
    {
        close(result[0]);
        const std::string text{body()};
        [[maybe_unused]] const auto written{::write(result[1], text.data(), text.size())};
        _exit(0);
    }
    close(result[1]);
    std::string text{};
    char buffer[256];
    for (auto count{::read(result[0], buffer, sizeof(buffer))}; 0 < count;
         count = ::read(result[0], buffer, sizeof(buffer)))
    {
        text.append(buffer, static_cast<std::size_t>(count));
    }
    close(result[0]);
    int status{};
    waitpid(child, &status, 0);
    if (WIFSIGNALED(status)) { crashed = WTERMSIG(status); }
    return text;
}

/**
 * @brief Fail the current test with a message, the way the framework's EXPECT_* macros do.
 *
 * @param[in] message What went wrong.
 * @param[in] file The test's source file.
 * @param[in] line The line of the check.
 */
[[noreturn]] inline void fail(const std::string& message, const char* file, const int line)
{
    throw std::runtime_error{message + " (" + file + ":" + std::to_string(line) + ")"};
}

/**
 * @brief The first calls of a report, for a failure message.
 *
 * @param[in] report The report.
 *
 * @return The first calls, as W and R, followed by ... if there were more.
 */
inline std::string firstCalls(const Report& report)
{
    constexpr std::size_t shown{16U};
    return report.calls.substr(0U, shown) + (shown < report.calls.size() ? "..." : "");
}

/**
 * @brief Check what Exercises 3.1 d) and 4.3 d) ask of run()'s loop: it keeps running, sends
 *        0, 1, 2, ... about every 100 ms, and tries to read after every write.
 *
 * @param[in] report What run() did in 450 ms.
 * @param[in] file The test's source file.
 * @param[in] line The line of the check.
 */
inline void expectTheLoop(const Report& report, const char* file, const int line)
{
    const std::size_t writes{report.written.size()};
    if (report.stoppedEarly)
    {
        fail("run() returned, or crashed, after " + std::to_string(writes) +
                 " write(s); it should repeat forever",
             file, line);
    }
    if ((writes < 3U) || (6U < writes))
    {
        fail("run() wrote " + std::to_string(writes) +
                 " byte(s) in 450 ms; one every 100 ms would be 4 or 5",
             file, line);
    }
    for (std::size_t i{}; i < writes; ++i)
    {
        if (report.written[i] != static_cast<std::uint8_t>(i))
        {
            std::ostringstream message{};
            message << "write number " << (i + 1U) << " sent "
                    << static_cast<unsigned>(report.written[i]) << ", expected " << i
                    << ": start at 0 and add one each time";
            fail(message.str(), file, line);
        }
    }
    for (std::size_t i{}; i < report.calls.size(); ++i)
    {
        if (report.calls[i] != ((0U == (i % 2U)) ? 'W' : 'R'))
        {
            fail("the calls were " + firstCalls(report) +
                     " (W = write, R = read); expected a write, then a read, each time round",
                 file, line);
        }
    }
}

/**
 * @brief Check that run() printed one line for each byte it received, and nothing otherwise.
 *
 * @param[in] report What run() did in 450 ms.
 * @param[in] receives True if the serial driver reported a received byte on every read.
 * @param[in] file The test's source file.
 * @param[in] line The line of the check.
 */
inline void expectPrintsWhatIsReceived(const Report& report, const bool receives, const char* file,
                                       const int line)
{
    if (!receives && !report.printed.empty())
    {
        fail("nothing was received, but run() printed \"" + report.printed.substr(0U, 60U) + "\"",
             file, line);
    }
    // The child may be killed between a read and its print, so one line may be missing.
    const std::size_t reads{report.written.size()};
    if (receives && ((report.printedLines() + 1U < reads) || (reads < report.printedLines())))
    {
        fail("a byte was received on each of " + std::to_string(reads) +
                 " reads, but run() printed " + std::to_string(report.printedLines()) +
                 " line(s); expected one per byte",
             file, line);
    }
}
} // namespace probe

/** Check run()'s loop, reporting what it did if it is wrong. */
#define EXPECT_THE_LOOP(report) probe::expectTheLoop((report), __FILE__, __LINE__)

/** Check what run() printed, reporting what it printed if it is wrong. */
#define EXPECT_PRINTS_WHAT_IS_RECEIVED(report, receives) \
    probe::expectPrintsWhatIsReceived((report), (receives), __FILE__, __LINE__)
