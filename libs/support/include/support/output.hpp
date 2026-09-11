/**
 * @file Capture what a piece of code prints, so a test can compare it with the output an exercise
 *       specifies.
 *
 *       Most exercises in this course say what a program must print, character for character,
 *       and a driver that prints the wrong thing is wrong. captureOutput() runs a callable with
 *       standard output redirected to a temporary file, restores it, and returns everything that
 *       was written, whether through std::printf or std::cout: both end up on file descriptor 1.
 *
 *       A failed EXPECT_* throws, so code under capture may leave by exception. Standard output
 *       is restored on the way out regardless; otherwise the failure message, and every line the
 *       test runner printed after it, would disappear into the temporary file.
 */
#pragma once

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <unistd.h>

namespace support
{
namespace detail
{
/**
 * @brief Redirect standard output to a file for as long as this object lives.
 */
class Redirect final
{
public:
    /**
     * @brief Redirect standard output to the given file.
     *
     * @param[in] file The file to write to.
     */
    explicit Redirect(std::FILE* file) noexcept
        : mySaved{dup(fileno(stdout))}
    {
        // Anything already buffered belongs to the test runner, not to the code under test.
        flush();
        dup2(fileno(file), fileno(stdout));
    }

    /**
     * @brief Restore standard output, flushing what was written to the file first.
     */
    ~Redirect() noexcept
    {
        flush();
        dup2(mySaved, fileno(stdout));
        close(mySaved);
    }

    Redirect(const Redirect&)            = delete; // No copy constructor.
    Redirect(Redirect&&)                 = delete; // No move constructor.
    Redirect& operator=(const Redirect&) = delete; // No copy assignment.
    Redirect& operator=(Redirect&&)      = delete; // No move assignment.

private:
    /**
     * @brief Flush std::cout and stdout. std::cout is synchronized with stdio by default, so
     *        both end up in the same place, in the order they were written.
     */
    static void flush() noexcept
    {
        std::cout.flush();
        std::fflush(stdout);
    }

    /** Standard output as it was before the redirection. */
    const int mySaved;
};
} // namespace detail

/**
 * @brief Run a callable and return everything it printed to standard output.
 *
 * @tparam Function Any callable taking no arguments.
 *
 * @param[in] function The code to run.
 *
 * @return The text the callable printed, exactly as printed.
 */
template<typename Function>
std::string captureOutput(Function&& function)
{
    std::FILE* file{std::tmpfile()};
    if (nullptr == file) { return "<captureOutput: no temporary file>"; }
    try
    {
        detail::Redirect redirect{file};
        function();
    }
    catch (...)
    {
        std::fclose(file);
        throw;
    }

    std::string text{};
    std::rewind(file);
    for (int c{std::fgetc(file)}; EOF != c; c = std::fgetc(file))
    {
        text += static_cast<char>(c);
    }
    std::fclose(file);
    return text;
}

namespace detail
{
/**
 * @brief Split text into lines, without their newlines.
 *
 * @param[in] text The text to split.
 *
 * @return The lines, in order. Text ending in a newline has no empty last line.
 */
inline std::vector<std::string> lines(const std::string& text)
{
    std::vector<std::string> result{};
    std::istringstream stream{text};
    for (std::string line{}; std::getline(stream, line);)
    {
        result.push_back(line);
    }
    return result;
}

/**
 * @brief Fail the current test, the way the framework's own EXPECT_* macros do: by throwing.
 *
 * @param[in] message What went wrong.
 * @param[in] file The test's source file.
 * @param[in] line The line of the expectation.
 */
[[noreturn]] inline void fail(const std::string& message, const char* file, const int line)
{
    throw std::runtime_error{message + " (" + file + ":" + std::to_string(line) + ")"};
}

/**
 * @brief Compare a program's output with the output it should have printed.
 *
 *        A program that prints four thousand lines where it should print nine would make a plain
 *        EXPECT_EQ print both, in full. This reports the first line that differs and how many
 *        lines each has, which is what you need to find the fault.
 *
 * @param[in] actual What the code printed.
 * @param[in] expected What it should have printed.
 * @param[in] file The test's source file.
 * @param[in] line The line of the expectation.
 */
inline void expectOutput(const std::string& actual, const std::string& expected, const char* file,
                         const int line)
{
    if (actual == expected) { return; }
    const auto got{lines(actual)};
    const auto want{lines(expected)};
    std::ostringstream message{};
    message << "the output differs from the expected output";
    for (std::size_t i{}; i < got.size() || i < want.size(); ++i)
    {
        const std::string gotLine{i < got.size() ? got[i] : "<no more output>"};
        const std::string wantLine{i < want.size() ? want[i] : "<no more output>"};
        if (gotLine != wantLine)
        {
            message << " at line " << (i + 1U) << ":\n      expected: \"" << wantLine
                    << "\"\n      printed:  \"" << gotLine << "\"\n     ";
            break;
        }
    }
    if (got == want) { message << ": the lines match, but not the final newline;"; }
    message << " expected " << want.size() << " line(s), printed " << got.size();
    fail(message.str(), file, line);
}
} // namespace detail
} // namespace support

/**
 * @brief Fail if the printed output is not exactly the expected output, and say where they part.
 */
#define EXPECT_OUTPUT(actual, expected) \
    support::detail::expectOutput((actual), (expected), __FILE__, __LINE__)
