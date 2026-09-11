/**
 * @file Run your program, the executable, and check what it printed and how it ended.
 *
 *       A test could call your main() directly, renamed, but only the real ::main may leave out its
 *       return statement: renamed, the same code falls off the end of an ordinary function, which
 *       GCC compiles into a trap. So the suite builds your program as the program it is, with its
 *       own main(), and a test runs it as a separate process, the way you would. The suite passes
 *       its path to the test as PROGRAM (see ci/suite.mk, <b>_PROGRAM).
 */
#pragma once

#include <cstdio>
#include <string>

#include <sys/wait.h>

#include "support/output.hpp"

namespace support
{
/**
 * @brief What a program printed, and how it ended.
 */
struct ProgramResult
{
    /** Everything the program wrote to standard output. */
    std::string output;

    /** The exit status if the program exited, otherwise -1. */
    int exitStatus;

    /** The signal that killed the program, otherwise 0. exitStatus 124 means it timed out. */
    int signal;
};

/**
 * @brief Run a program and collect its output.
 *
 * @param[in] path The program to run.
 * @param[in] timeoutSeconds How long it may run before it is stopped.
 *
 * @return What the program printed, and how it ended.
 */
inline ProgramResult runProgram(const std::string& path, const unsigned timeoutSeconds = 30U)
{
    ProgramResult result{"", -1, 0};
    const std::string command{"timeout " + std::to_string(timeoutSeconds) + "s '" + path + "'"};
    std::FILE* pipe{popen(command.c_str(), "r")};
    if (nullptr == pipe) { return result; }
    for (int c{std::fgetc(pipe)}; EOF != c; c = std::fgetc(pipe))
    {
        result.output += static_cast<char>(c);
    }
    const int status{pclose(pipe)};
    if (WIFEXITED(status)) { result.exitStatus = WEXITSTATUS(status); }
    if (WIFSIGNALED(status)) { result.signal = WTERMSIG(status); }

    // timeout reports a program killed by a signal as 128 plus the signal, and one it had to stop
    // as 124; either way the program did not exit, so say which signal ended it.
    if (result.exitStatus > 128)
    {
        result.signal     = result.exitStatus - 128;
        result.exitStatus = -1;
    }
    return result;
}

namespace detail
{
/**
 * @brief Fail unless the program ran to the end and returned 0, saying how it ended otherwise.
 *
 * @param[in] result How the program ended.
 * @param[in] file The test's source file.
 * @param[in] line The line of the expectation.
 */
inline void expectCleanExit(const ProgramResult& result, const char* file, const int line)
{
    if (0 == result.exitStatus && 0 == result.signal) { return; }
    if (124 == result.exitStatus)
    {
        fail("the program was still running after its timeout", file, line);
    }
    if (0 != result.signal)
    {
        fail("the program crashed (signal " + std::to_string(result.signal) +
                 "): a null or dangling pointer, or a double delete?",
             file, line);
    }
    fail("the program exited with status " + std::to_string(result.exitStatus) + ", not 0", file,
         line);
}
} // namespace detail
} // namespace support

/**
 * @brief Fail unless a program run with runProgram() ended normally, returning 0. For a test that
 *        checks the output its own way, e.g. only its first line, or lines from several threads.
 */
#define EXPECT_CLEAN_EXIT(result) support::detail::expectCleanExit((result), __FILE__, __LINE__)

/**
 * @brief Run the set's program: it must end normally, returning 0, and print exactly the expected
 *        output.
 */
#define EXPECT_PROGRAM_OUTPUT(expected)                                           \
    do                                                                            \
    {                                                                             \
        const support::ProgramResult programResult{support::runProgram(PROGRAM)}; \
        support::detail::expectCleanExit(programResult, __FILE__, __LINE__);      \
        EXPECT_OUTPUT(programResult.output, (expected));                          \
    } while (false)
