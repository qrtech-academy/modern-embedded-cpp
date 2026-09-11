/**
 * @file Tests for Exercise Set 4: sendMessage() and the program, in source/main.cpp.
 *
 *       sendMessage() is written against the interface, so it is tested with an implementation
 *       that records every byte it is given, as well as with the stub the exercise uses. The
 *       functions live in your main.cpp, so this file includes it, with main() renamed so it does
 *       not collide with the test runner's; that also makes your program runnable here.
 */
#include <cstdint>
#include <string>

#include "driver/serial/interface.hpp"
#include "driver/serial/stub.hpp"
#include "qacademy/test/test.hpp"
#include "support/output.hpp"

// Your program, with its main() renamed. A main() may leave out its return statement and any
// other function may not, so that one warning is silenced for your file alone.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"
#define main exercise4Main
#include "main.cpp"
#undef main
#pragma GCC diagnostic pop

using support::captureOutput;

namespace
{
/** The message Exercise 4.1 a) asks sendMessage() to transmit. */
const std::string expectedMessage{"Transmitting data with a serial driver!"};

/**
 * @brief An implementation of the serial interface that records every byte written to it.
 */
class Recorder final : public driver::serial::Interface
{
public:
    [[nodiscard]] bool isInitialized() const noexcept override { return true; }

    void write(const std::uint8_t byte) noexcept override
    {
        myBytes += static_cast<char>(byte);
        ++myWrites;
    }

    bool read(std::uint8_t&) noexcept override { return false; }

    /** Every byte written, in order. */
    std::string myBytes{};

    /** The number of calls to write(). */
    unsigned myWrites{};
};
} // namespace

/**
 * @brief Exercise 4.1 a): sendMessage() transmits exactly the message, one byte per write(), with
 *        nothing before it, nothing after it, and no terminating null byte.
 */
TEST(SendMessage, TransmitsTheMessageOneByteAtATime)
{
    Recorder recorder{};
    sendMessage(recorder);
    EXPECT_EQ(recorder.myBytes, expectedMessage);
    EXPECT_EQ(recorder.myWrites, static_cast<unsigned>(expectedMessage.size()));
}

/**
 * @brief Exercise 4.1 c): after sendMessage(), the stub holds the last byte of the message, '!'.
 */
TEST(SendMessage, LeavesTheLastByteInTheStub)
{
    driver::serial::Stub stub{};
    std::uint8_t byte{};
    sendMessage(stub);
    EXPECT_TRUE(stub.read(byte));
    EXPECT_EQ(static_cast<char>(byte), '!');
}

/**
 * @brief Exercise 4.1 b): the program prints the message through the console driver, as the first
 *        thing it prints. What follows it, from the stub test in c), is up to you.
 */
TEST(Program, PrintsTheMessageThroughTheConsole)
{
    const std::string output{captureOutput([] { exercise4Main(); })};
    EXPECT_OUTPUT(output.substr(0U, output.find('\n')), expectedMessage);
}
